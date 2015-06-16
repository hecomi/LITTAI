#include <numeric>
#include "tracker.h"

using namespace Littai;



Tracker::Tracker(QQuickItem *parent)
    : Image(parent)
    , templateThreshold_(0.2)
{
}


void Tracker::setInputImage(const QVariant &image)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        image.value<cv::Mat>().copyTo(inputImage_);
    }
    emit inputImageChanged();
}


QVariant Tracker::inputImage() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return QVariant::fromValue(inputImage_);
}


void Tracker::setTemplateImage(const QVariant &image)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        image.value<cv::Mat>().copyTo(templateImage_);
    }
    emit templateImageChanged();
}


QVariant Tracker::templateImage() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return QVariant::fromValue(templateImage_);
}


void Tracker::track()
{
    cv::Mat outputImage;

    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (inputImage_.empty() || templateImage_.empty()) {
            return;
        }
        outputImage = inputImage_.clone();
    }

    cv::Mat grayInput;
    cv::cvtColor(outputImage, grayInput, cv::COLOR_BGR2GRAY);
    cv::threshold(grayInput, grayInput, 100, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    cv::Mat grayTemplate;
    cv::cvtColor(templateImage_, grayTemplate, cv::COLOR_BGR2GRAY);
    cv::threshold(grayTemplate, grayTemplate, 100, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    cv::resize(grayTemplate, grayTemplate, cv::Size(), 0.5, 0.5, cv::INTER_LINEAR);

    const auto templateWidth = grayTemplate.rows;
    const auto templateHeight = grayTemplate.cols;
    const cv::Point templateSize(templateWidth, templateHeight);
    const auto templateScale = (templateWidth + templateHeight) / 2;

    cv::Mat result;
    cv::matchTemplate(grayInput, grayTemplate, result, cv::TM_CCOEFF_NORMED);

    double maxVal = 1;
    for (int maxTry = 0; maxTry < 10; ++maxTry) {
        double minVal;
        cv::Point minPos, maxPos;
        cv::minMaxLoc(result, &minVal, &maxVal, &minPos, &maxPos);

        if (maxVal < templateThreshold_) {
            break;
        }

        if (maxPos.x < 0 || maxPos.x + templateWidth  >= grayInput.cols ||
            maxPos.y < 0 || maxPos.y + templateHeight >= grayInput.rows) {
            break;
        }

        cv::rectangle(
            outputImage,
            maxPos,
            maxPos + templateSize,
            CV_RGB(255, 0, 0),
            2);

        cv::rectangle(
            result,
            maxPos - templateSize * 0.5,
            maxPos + templateSize * 0.5,
            CV_RGB(0, 0, 0),
            -1);

        auto roi = grayInput(cv::Rect(maxPos, maxPos + templateSize));
        std::vector<cv::Mat> contours;
        cv::findContours(roi, contours, cv::RETR_EXTERNAL, 2);
        auto moments = cv::moments(contours[0]);

        if (moments.m00 == 0) continue;

        int centerX = moments.m10 / moments.m00;
        int centerY = moments.m01 / moments.m00;
        std::vector<double> holeAngles;
        const int div = 100;
        for (int i = 0; i < div; ++i) {
            const auto angle = 2 * M_PI * i / div;

            bool isHit = false;

            for (int r = 0; r < templateScale; r += 5) {
                int x = centerX + r * cos(angle);
                int y = centerY + r * sin(angle);
                if (x < 0 || y < 0 || x >= templateWidth || y >= templateHeight) {
                    break;
                }
                if (roi.at<unsigned char>(y, x) > 0) {
                    isHit = true;
                    break;
                } else {
                    cv::circle(outputImage, maxPos + cv::Point(x, y), 1, CV_RGB(255,0,0), 1);
                }
            }

            if (!isHit) {
                if (!holeAngles.empty() && abs(holeAngles.front() - angle) > M_PI) {
                    holeAngles.push_back(angle - 2 * M_PI);
                } else {
                    holeAngles.push_back(angle);
                }
            }
        }

        cv::circle(outputImage, maxPos + cv::Point(centerX, centerY), 6, CV_RGB(0, 255, 0), -1);
        if (!holeAngles.empty()) {
            const auto averageHoleAngle = std::accumulate(holeAngles.begin(), holeAngles.end(), 0.0) / holeAngles.size();
            const auto center = maxPos + cv::Point(centerX, centerY);
            const auto dir = cv::Point(
                static_cast<int>(templateScale / 2 * cos(averageHoleAngle)),
                static_cast<int>(templateScale / 2 * sin(averageHoleAngle)));
            cv::arrowedLine(outputImage, center, center + dir, CV_RGB(0, 255, 0), 3);
        }
    }

    setImage(outputImage);
    update();
}


QVariant Tracker::result()
{
    return QVariant();
}
