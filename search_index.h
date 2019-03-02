#ifndef INDEX_SEARCH_H
#define INDEX_SEARCH_H

#include <QString>
#include <QVector>
#include <QObject>
#include <QThread>
#include <QFile>
#include <QMap>
#include <QMutex>
#include <limits>
#include <QSet>
#include <memory>
#include <QFileSystemWatcher>
#include "trigram.h"
#include <set>

const size_t READ_BLOCK = 4096 * 16;


class index_search : public QObject {
    Q_OBJECT
public:
    index_search(QMutex &mtx, QMap<QString, std::set<tgram>> &paths_to_tgram, QString dir_path);
    ~index_search();
signals:
    void set_progress(int value);
    void file_change();
    void index_finished();
    void finished();
    void index_end();
    void set_max_progress(int value);

public slots:
    void file_change_slot();
    void start_index(QString path);
    void add_to_map(QString const &path);
    void start_index();

private:
    std::unique_ptr<QFileSystemWatcher> filewatcher;
    QMap<QString, std::set<tgram>> &paths_to_tgram;
    QMutex& mtx;
    QString dir_path;

};

#endif // INDEX_SEARCH_H
