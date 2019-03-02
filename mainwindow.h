#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "search_index.h"
#include "find_string.h"
#include "trigram.h"

#include <QMainWindow>
#include <QMetaType>
#include <QThread>
#include <QTime>
#include <QMutex>
#include <QTreeWidgetItem>
#include <QVector>
#include <QFileSystemWatcher>

#include <memory>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:

public slots:
    void add_path(std::pair<QString, QVector<QString>>);
    void file_change();
    void index_end();
    void search_end();
    void select_directory();

private:
    QString cur_dir;
    int cnt = 0;
    bool index_now;
    std::unique_ptr<QThread> index_thread;
    std::unique_ptr<QThread> search_thread;
    std::unique_ptr<Ui::MainWindow> ui;

    QTime search_time;
    QTime index_time;

    QMap<QString, std::set<tgram>> paths_to_tgram;
    QMutex mtx;

    void start_search();
    void start_index();

    void stop_search();
    void stop_index();
};

#endif // MAINWINDOW_H
