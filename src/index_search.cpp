#include "index_search.h"

#include <QDirIterator>
#include <QMimeType>
#include <QMimeDatabase>
#include <iostream>
#include <QDebug>
#include <QVector>

#include <fstream>
#include <set>
#include <stdio.h>

index_search::index_search(QMutex &mtx, QMap<QString, std::set<tgram>> &files_triggers, QString const dir_path) : filewatcher(new QFileSystemWatcher), mtx(mtx), files_triggers(files_triggers), dir_path(dir_path){
    //send notification about file change
    connect(filewatcher.get(), SIGNAL(fileChanged(QString)), this, SLOT(file_change_slot(QString)));
    //scan modified file
    connect(filewatcher.get(), SIGNAL(fileChanged(QString)), this, SLOT(add_to_map(QString)));
    //scan modified dir
    connect(filewatcher.get(), SIGNAL(directoryChanged(QString)), this, SLOT(start_index(QString)));
    connect(filewatcher.get(), SIGNAL(directoryChanged(QString)), this, SLOT(file_change_slot(QString)));
}

void index_search::file_change_slot(QString fname){
//    filewatcher->addPath(fname);
    emit file_change(fname);
}


void index_search::start_index(){
    start_index(dir_path);
}

index_search::~index_search(){}

// get all files path from chosen directory
void index_search::start_index(QString cur_path) {
    QDirIterator it(cur_path, QDir::NoDotAndDotDot | QDir::Hidden | QDir::NoSymLinks | QDir::AllEntries, QDirIterator::Subdirectories);
    emit set_max_progress(0);
    emit set_progress(0);

    QVector<QString> paths;
    /*iterate over all files from the selected folder and add filenames to vector*/
    while (it.hasNext()) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            break;
        }
        QFileInfo fi(it.next());
        if(fi.permission(QFile::ReadUser)) {
            if(fi.isFile()) {
                paths.push_back(fi.filePath());
            }
            //std::cout<<fi.filePath().toStdString()<<'\n';
            filewatcher->addPath(fi.filePath());
        }
    }

    /*set toolbar max value*/
    emit set_max_progress(paths.size());

    for(int i = 0; i < paths.size(); ++i) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            break;
        }
        /*upgrade toolbar */
        emit set_progress(i);
        add_to_map(paths[i]);
    }

    emit set_max_progress(1);
    emit set_progress(1);
    emit index_end();
//    emit finished();
}

//for all files from chosen directory save all triggers
void index_search::add_to_map(QString const &path) {
    static QMimeDatabase mimeDatabase;
       const QMimeType mimeType = mimeDatabase.mimeTypeForFile(path);
       if (mimeType.isValid() && !mimeType.inherits(QStringLiteral("text/plain"))) {
           return;
       }

    std::ifstream fin(path.toStdString(), std::ios::in | std::ios::binary);
    if (!fin) {
        qDebug()<<path;
        return;
    }
    std::set<tgram> tgram_set;
    std::vector<char> buffer(3 * READ_BLOCK);
    size_t count = 0;
    bool flag = true;
    char symbol[2] = {0, 0};
   //    a s d a s д/2 д/2 a b s
    while (!fin.eof() && flag) {
        fin.read(buffer.data(), READ_BLOCK);

        count = static_cast<size_t>(fin.gcount());

        for (size_t i = 0; i < count; ++i) {
            if(i >= 2) {
                tgram_set.insert(tgram(buffer[i-2], buffer[i-1], buffer[i]));
            } else if(symbol[0] != 0){
                tgram_set.insert(tgram(symbol[0], symbol[1], buffer[i]));
                symbol[0] = symbol[1];
                symbol[1] = buffer[i];
            }
        }
        if(count >= 2)
            symbol[0] = buffer[count - 2];
        if(count >= 1)
            symbol[1] = buffer[count - 1];

        if(tgram_set.size() > 20000){
            flag = false;
        }
    }

    if(flag) {
        mtx.lock();
        files_triggers.insert(path, tgram_set);
        mtx.unlock();
    }

    fin.close();
}
