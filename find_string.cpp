#include "find_string.h"
#include <vector>
#include <QFile>
#include <QTextStream>
#include <QMutexLocker>
#include <QDebug>

match_string::match_string(QMutex &mtx, QMap<QString, std::set<tgram>> const &paths_to_tgram, QString match_s) : mtx(mtx), paths_to_tgram(paths_to_tgram), match_s(match_s){}

void match_string::start_search() {
    qDebug() << "search search";
    emit set_max_progress(paths_to_tgram.size());
    emit set_progress(0);
    std::set<tgram> match_tgram;
    auto buffer(match_s.toUtf8());
    for (int i = 2; i < buffer.size(); ++i) { //size int
        match_tgram.insert(tgram(buffer[i - 2], buffer[i - 1], buffer[i])); //operator[] from int and uint :(
    }
    QMutexLocker locker(&mtx);
    int k = 0;
    for(auto it = paths_to_tgram.keyValueBegin(); it != paths_to_tgram.keyValueEnd() && !QThread::currentThread()->isInterruptionRequested(); ++it) {
        if(std::includes((*it).second.begin(), (*it).second.end(),
                         match_tgram.begin(), match_tgram.end())) {
            QFile file((*it).first);
            QVector<QString> ans;
            if (file.open(QIODevice::ReadOnly)) {
                QString line;
                QTextStream in(&file);
                while (!in.atEnd()) {
                    if (QThread::currentThread()->isInterruptionRequested()){
                        break;
                    }
                    line = in.readLine();
                    if (line.contains(match_s, Qt::CaseInsensitive)) {
                        ans.push_back(line);
                    }
                }
            }
            if(ans.size()){
                emit matched(std::make_pair((*it).first, ans));
            }
        }
        emit set_progress(++k);
    }
    emit set_max_progress(1);
    emit set_progress(1);
    emit finished();
    qDebug() << "search end";
}
