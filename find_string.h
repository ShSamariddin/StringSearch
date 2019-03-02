#ifndef MATCH_STRING_H
#define MATCH_STRING_H

#include <QObject>
#include <QMap>
#include <QSet>
#include <QByteArray>
#include <QThread>
#include <QMutex>
#include <set>
#include "trigram.h"

class match_string: public QObject {
    Q_OBJECT
public:
    match_string(QMutex &mtx, QMap<QString, std::set<tgram>> const &paths_to_tgram, QString match_s);

signals:
    void set_progress(int value);
    void search_finished();
    void finished();
    void matched(std::pair<QString, QVector<QString>> v);
    void set_max_progress(int value);

public slots:
    void start_search();

private:
    QMutex& mtx;
    QMap<QString, std::set<tgram>> const &paths_to_tgram;
    QString match_s;
};

#endif // MATCH_STRING_H
