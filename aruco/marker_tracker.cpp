#include <numeric>
#include <aruco.h>
#include <opencv2/opencv.hpp>
#include <polypartition.h>
#include "marker_tracker.h"

using namespace Littai;


namespace
{
    template <class T>
    double len(const T& v)
    {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }

    template <class T>
    T toUnit(const T& v, const double scaleX, const double scaleY)
    {
        return T(2.0 * v.x / scaleX - 1.0, 1.0 - 2.0 * v.y / scaleY);
    }
}



MarkerTracker::MarkerTracker(QQuickItem *parent)
    : Image(parent)
    , isFinished_(false)
    , contrastThreshold_(100)
    , fps_(30)
{
    thread_ = std::thread([&] {
        using namespace std::chrono;
        while (!isFinished_) {
            const auto t1 = high_resolution_clock::now();
            track();
            const auto t2 = high_resolution_clock::now();
            const auto dt = t2 - t1;
            const auto waitTime = microseconds(1000000 / fps_) - dt;
            if (waitTime > microseconds::zero()) {
                std::this_thread::sleep_for(waitTime);
            }
        }
    });
}


MarkerTracker::~MarkerTracker()
{
    isFinished_ = true;
    if (thread_.joinable()) {
        thread_.join();
    }
}


void MarkerTracker::setInputImage(const QVariant &image)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        image.value<cv::Mat>().copyTo(inputImage_);
        isImageUpdated_ = true;
    }
    emit inputImageChanged();
}


QVariant MarkerTracker::inputImage() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return QVariant::fromValue(inputImage_);
}


void MarkerTracker::track()
{
    if (!isImageUpdated_) return;

    if (inputImage_.empty()) return;

    cv::Mat image, gray;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        image = inputImage_.clone();
    }

    // ガンマ補正 / 複数枚平均
    preProcess(image);
    // グレースケール
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    // マーカ認識
    detectMarkers(image, gray);
    // ポリゴン認識
    detectPolygons(image, gray);

    setImage(image, false);

    isImageUpdated_ = false;
}


void MarkerTracker::preProcess(cv::Mat &image)
{
    // ノイズリダクションと 2 値化
    cv::Mat lut(1, 256, CV_8U);
    for (int i = 0; i < 256; ++i) {
        lut.at<unsigned char>(i) = (i < contrastThreshold_) ? 0 : 255;
    }
    cv::LUT(image, lut, image);
    cv::medianBlur(image, image, 3);
    cv::threshold(image, image, contrastThreshold_, 255, cv::THRESH_BINARY);

    /*
    imageCaches_.push_back(image);
    if (imageCaches_.size() > 3) {
        imageCaches_.pop_front();
    }
    cv::Mat input(image.rows, image.cols, image.type(), cv::Scalar(0));
    for (auto&& cache : imageCaches_) {
        input += cache * 1.0 / imageCaches_.size();
    }
    image = input;
    */
}


void MarkerTracker::detectMarkers(cv::Mat &resultImage, cv::Mat &inputImage)
{
    const double scale = 1.5;
    cv::Mat image;
    cv::resize(inputImage, image, cv::Size(), scale, scale, cv::INTER_LINEAR);

    // AruCo によるマーカの認識
    aruco::MarkerDetector detector;
    std::vector<aruco::Marker> markers;
    detector.detect(image, markers);

    // 結果を格納
    std::vector<TrackedMarker> newMarkers;
    for (auto&& marker : markers) {
        const auto sum = std::accumulate(marker.begin(), marker.end(), cv::Point2f(0.f));

        TrackedMarker info;
        info.id = marker.id;
        info.x = sum.x / marker.size() / scale;
        info.y = sum.y / marker.size() / scale;
        const auto side = marker[0] - marker[3];
        info.angle = std::atan2(side.x, side.y);
        info.size = len(side);

        newMarkers.push_back(info);
    }

    // フラグをオフにしておく
    for (auto&& marker : markers_) {
        marker.checked = false;
    }

    // 見つかったアイテムは情報を更新してフラグを立てる
    for (auto&& newMarker : newMarkers) {
        bool isFound = false;
        for (auto&& marker : markers_) {
            if (newMarker.id == marker.id) {
                isFound = true;
                marker.update(newMarker);
                marker.checked = true;
                break;
            }
        }
        if (!isFound) {
            markers_.push_back(newMarker);
        }
    }

    // しばらく認識されなかったマーカは削除
    // 認識されたアイテムはフレームカウントを増加
    for (auto it = markers_.begin(); it != markers_.end();) {
        auto& marker = *it;
        if (marker.checked) {
            marker.lostCount = 0;
        } else {
            marker.lostCount++;
            if (marker.lostCount > 30) {
                it = markers_.erase(it);
                continue;
            }
        }
        ++marker.frameCount;
        ++it;
    }

    emit markersChanged();
    /*
    for (auto&& marker : markers_) {
        marker.print();
    }
    */
}


void MarkerTracker::detectPolygons(cv::Mat &resultImage, cv::Mat &inputImage)
{
    // 領域を抽出
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(inputImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_TC89_KCOS);

    // マーカを囲む領域を認識
    std::map<unsigned int, std::vector<cv::Point>> contourMap;
    if (contours.size() > 0 && markers_.size() > 0) {
        // 領域を大きい順に並べる
        std::sort(contours.begin(), contours.end(),
            [](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) {
                return cv::contourArea(a) > cv::contourArea(b);
            });

        // マーカを内包する領域を調べる
        for (auto&& marker : markers_) {
            bool isFound = false;
            for (const auto& contour : contours) {
                // マーカの中心座標が領域内に含まれるか調べる
                // 含まれていればマップに登録
                const cv::Point pt(marker.x, marker.y);
                if (cv::pointPolygonTest(contour, pt, 0) == 1) {
                    contourMap.emplace(marker.id, contour);
                    isFound = true;
                    break;
                }
            }
            if (!isFound) {
                return;
                // qDebug("marker detected but contour not detected.");
            }
        }
    }

    for (auto&& marker : markers_) {
        // 中心と領域を描画
        const cv::Point center(marker.x, marker.y);
        cv::circle(resultImage, center, marker.size / 2, cv::Scalar(0, 255, 0), 2);

        auto it = contourMap.find(marker.id);
        if (it == contourMap.end()) return;
        auto& contour = (*it).second;

        auto boundingRect = cv::boundingRect(contour);
        marker.image = resultImage(boundingRect).clone();

        std::vector<std::vector<cv::Point>> contours = { contour };
        cv::drawContours(resultImage, contours, 0, cv::Scalar(255, 0, 0), 3);

        std::vector<cv::Point> polygon;
        cv::approxPolyDP(contour, polygon, 5, true);
        std::reverse(polygon.begin(), polygon.end());
        marker.polygon = polygon;
        marker.indices = triangulatePolygons(polygon);

        for (const auto& vertex : polygon) {
            cv::circle(resultImage, vertex, 3, cv::Scalar(0, 0, 255), 2);
        }

        // 突端認識
        if (polygon.size() >= 4) {
            std::vector<TrackedEdge> edges;
            const int N = polygon.size();
            for (int i = 0; i < N; ++i) {
                // 隣り合う 4 点
                const int i0 = i;
                const int i1 = ((i + 1) < N) ? (i + 1) : (i + 1 - N);
                const int i2 = ((i + 2) < N) ? (i + 2) : (i + 2 - N);
                const int i3 = ((i + 3) < N) ? (i + 3) : (i + 3 - N);
                const auto v0 = polygon[i0];
                const auto v1 = polygon[i1];
                const auto v2 = polygon[i2];
                const auto v3 = polygon[i3];

                // 4 点を作る辺
                const auto s1 = v1 - v0;
                const auto s2 = v2 - v1;
                const auto s3 = v3 - v2;

                // 4 点で囲まれる中心座標
                const auto center = (v0 + v1 + v2 + v3) * 0.25;
                const double ratio = 0.7;

                // 中心の辺が短く、1 番目と 2 番目の辺が逆を向き、中心が白くて、
                // 遠くにある場合、突端として認識する
                const bool isMiddleShort    = len(s2) / len(s1) < ratio && len(s2) / len(s3) < ratio;
                const bool isOpposite       = s1.dot(s3) < -0.5;
                const bool isCenterPosInner = inputImage.at<unsigned char>(center.y, center.x) > 0;
                const bool isFar            = len((v1 + v2) * 0.5 - center) > 30;
                if (isMiddleShort && isOpposite && isCenterPosInner && isFar) {
                    edges.emplace_back( (v1 + v2) * 0.5 );
                }
            }

            // Raw の Edge を描画
            for (const auto& edge : edges) {
                cv::circle(resultImage, edge, 6, cv::Scalar(0, 255, 0), 2);
            }

            // 過去に登録されたエッジと比較して近いものは更新
            // 新しいものは追加
            for (auto&& existingEdge : marker.edges) {
                existingEdge.checked = false;
            }
            std::vector<TrackedEdge> newEdges;
            for (auto&& newEdge : edges) {
                bool isFound = false;
                for (auto&& existingEdge : marker.edges) {
                    if (!existingEdge.checked && len(newEdge - existingEdge) < 50) {
                        existingEdge.x = newEdge.x;
                        existingEdge.y = newEdge.y;
                        existingEdge.checked = true;
                        isFound = true;
                        break;
                    }
                }
                if (!isFound) {
                    newEdges.push_back(newEdge);
                }
            }

            // 一定期間見つからなかったら削除
            for (auto it = marker.edges.begin(); it != marker.edges.end();) {
                auto& edge = *it;
                if (edge.checked) {
                    edge.lostCount = 0;
                    ++edge.frameCount;
                    if (edge.frameCount > 5) {
                        edge.activated = true;
                    }
                } else {
                    ++edge.lostCount;
                    if (edge.lostCount > 3) {
                        edge.activated = false;
                    }
                    if (edge.lostCount > 10) {
                        it = marker.edges.erase(it);
                        continue;
                    }
                }
                ++it;
            }

            // 新しいマーカを追加
            for (const auto& newEdge : newEdges) {
                marker.edges.push_back(newEdge);
            }

            // 認識したマーカを描画
            for (const auto& edge : marker.edges) {
                cv::circle(resultImage, edge, 6, cv::Scalar(0, 255, 255), -1);
            }
        }
    }
}


std::vector<int> MarkerTracker::triangulatePolygons(const std::vector<cv::Point> &polygon)
{
    TPPLPoly poly;
    poly.Init(polygon.size());
    poly.SetHole(false);

    for (int i = 0; i < polygon.size(); ++i) {
        poly[i].x = polygon[i].x;
        poly[i].y = polygon[i].y;
    }

    TPPLPartition pp;
    std::list<TPPLPoly> result;
    pp.Triangulate_EC(&poly, &result);

    std::vector<int> indices;
    for (auto&& triangle : result) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < poly.GetNumPoints(); ++j) {
                if (triangle[i] == poly[j]) {
                    indices.push_back(j);
                    break;
                }
            }
        }
    }

    return indices;
}


QVariantList MarkerTracker::markers() const
{
    std::lock_guard<std::mutex> lock(mutex_);

    QVariantList markers;
    if (inputImage_.empty()) return markers;

    const int width  = inputImage_.rows;
    const int height = inputImage_.cols;

    for (auto&& marker : markers_) {
        QVariantMap o;
        o.insert("id",         marker.id);
        o.insert("x",          2.0 * marker.x / width - 1.0);
        o.insert("y",          1.0 - 2.0 * marker.y / height);
        o.insert("size",       marker.size / ((width + height) / 2));
        o.insert("angle",      marker.angle);
        o.insert("frameCount", marker.frameCount);
        o.insert("image",      QVariant::fromValue(marker.image));

        QVariantList polygon, edges, indices;
        for (const auto& vertex : marker.polygon) {
            QVariantMap pos;
            pos.insert("x", 2.0 * vertex.x / width - 1.0);
            pos.insert("y", 1.0 - 2.0 * vertex.y / height);
            polygon.push_back(pos);
        }
        for (const auto& point : marker.edges) {
            if (point.activated) {
                QVariantMap pos;
                pos.insert("x", 2.0 * point.x / width - 1.0);
                pos.insert("y", 1.0 - 2.0 * point.y / height);
                edges.push_back(pos);
            }
        }
        for (int index : marker.indices) {
            indices.push_back(index);
        }
        o.insert("polygon", polygon);
        o.insert("edges", edges);
        o.insert("indices", indices);

        markers.append(o);
    }

    return markers;
}
