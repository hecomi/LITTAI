import QtQuick 2.1
import QtQuick.LocalStorage 2.0

QtObject {
    property string name        : 'UnnamedDB'
    property string version     : '1.0'
    property string description : 'No description'
    property int estimatedSize  : 100000

    function getDB() {
        return LocalStorage.openDatabaseSync(name, version, description, estimatedSize);
    }

    Component.onCompleted: {
        var db = getDB();
        db.transaction(function(tx) {
            tx.executeSql('CREATE TABLE IF NOT EXISTS settings(key TEXT UNIQUE, value TEXT)');
        });
    }

    function set(key, value) {
        var db = getDB();
        var success = false;
        db.transaction(function(tx) {
            var rs = tx.executeSql('INSERT OR REPLACE INTO settings VALUES (?,?);', [key, value]);
            success = (rs.rowsAffected > 0);
        });
        return success;
    }

    function get(key) {
        var db = getDB();
        var value = null;
        db.readTransaction(function(tx) {
            var rs = tx.executeSql('SELECT value FROM settings WHERE key=?;', [key]);
            if (rs.rows.length > 0) {
                value = rs.rows.item(0).value;
            }
        });
        return value;
    }
}
