#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QSizePolicy>
#include <QDebug>
#include <QFileDialog>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), index_now(false), ui(new Ui::MainWindow), index_thread(new QThread()), search_thread(new QThread) {
    ui->setupUi(this);

    qRegisterMetaType<std::pair<QString, QVector<QString>>>();
    connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(open_file(QTreeWidgetItem*)));

    connect(ui->SelectDirectory, &QPushButton::clicked, this, &MainWindow::select_directory);
    connect(ui->IndexStop, &QPushButton::clicked, this, &MainWindow::stop_index);
    connect(ui->StartStopSearch, &QPushButton::clicked, this, &MainWindow::start_search);
    ui->treeWidget->setColumnWidth(0, 460);
    ui->treeWidget->setStyleSheet("background-color: rgb(0,0,0);");

    ui->progressBar->setMaximum(1);
    ui->progressBar->setValue(0);
}

MainWindow::~MainWindow() {
    stop_index();
    stop_search();
}

void MainWindow::select_directory() {
    cur_dir = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning", QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->label_2->setText("status:indexing...");
    start_index();
    ui->label->setText("current directory:" + cur_dir);
}

void MainWindow::start_index() {
    stop_index();
    index_now = true;
    paths_to_tgram.clear();
    index_search* worker = new index_search(mtx, paths_to_tgram, cur_dir);
    worker->moveToThread(index_thread.get());

    connect(worker, SIGNAL(file_change()), this, SLOT(file_change()));
    connect(worker, SIGNAL(set_max_progress(int)), ui->progressBar, SLOT(setMaximum(int)));
    connect(worker, SIGNAL(set_progress(int)), ui->progressBar, SLOT(setValue(int)));

    connect(index_thread.get(), SIGNAL (started()), worker, SLOT (start_index()));
    connect(worker, SIGNAL (finished()), this, SLOT (index_end()));
    connect(worker, SIGNAL (index_end()), this, SLOT (index_end()));

    index_thread->start();
}

void MainWindow::start_search() {
    if(index_now) {
        ui->treeWidget->clear();
        ui->label_3->setText("status:Directory now indexing");
        return;
    }
    stop_search();

    ui->treeWidget->clear();
    ui->lcdNumber->display(0);

    search_time.start();
    match_string* worker = new match_string(mtx, paths_to_tgram, ui->MatchString->text());
    worker->moveToThread(search_thread.get());

    connect(worker, SIGNAL(set_max_progress(int)), ui->progressBar, SLOT(setMaximum(int)));
    connect(worker, SIGNAL(set_progress(int)), ui->progressBar, SLOT(setValue(int)));
    connect(worker, SIGNAL(matched(std::pair<QString, QVector<QString>>)), this, SLOT(add_path(std::pair<QString, QVector<QString>>)));

    connect(search_thread.get(), SIGNAL(started()), worker, SLOT(start_search()));
    connect(worker, SIGNAL (finished()), search_thread.get(), SLOT (quit()));
    connect(worker, SIGNAL (finished()), worker, SLOT (deleteLater()));
    connect(worker, SIGNAL (finished()), this, SLOT (search_end()));

    search_thread->start();
    ui->label_3->setText("status:searching...");
}

void MainWindow::stop_index() {
    if(index_thread != nullptr && index_thread->isRunning()){
        index_thread->requestInterruption();
        index_thread->quit();
        index_thread->wait();
        index_thread.reset(new QThread);
        ui->label_2->setText("status:index canceled");
    }
}

void MainWindow::stop_search() {
    if(search_thread != nullptr && search_thread->isRunning()){
        search_thread->requestInterruption();
        search_thread->quit();
        search_thread->wait();
        ui->label_3->setText("status:search canceled");
    }
}

void MainWindow::add_path(std::pair<QString, QVector<QString>> v){
    QTreeWidgetItem *ti = new QTreeWidgetItem();
    cnt++;int color1 = rand() % 200 + 55;
    int color2 = rand() % 200 + 55;
    int color3 = rand() % 200 + 55;
    ti->setTextColor(0, QColor::fromRgb(color1, color2, color3));
    ti->setTextColor(1, QColor::fromRgb(color1, color2, color3));
    ti->setTextColor(2, QColor::fromRgb(color1, color2, color3));
    ti->setTextColor(3, QColor::fromRgb(color1, color2, color3));
    ti->setText(0, "Group" + QString::number(cnt));
    ti->setText(1, QString::number(v.second.size()));
    ti->setText(2, QString::number(v.second.first().size()));
    int tsize = 0;
    for (auto const & k: v.second) {
        tsize += k.size();
        QTreeWidgetItem *child = new QTreeWidgetItem();
        child->setText(0, k);
        child->setTextColor(0, QColor::fromRgb(color1, color2, color3));
        ti->addChild(child);
    }
    ti->setText(3, QString::number(tsize));
    ui->treeWidget->addTopLevelItem(ti);

    ui->lcdNumber->display(ui->treeWidget->topLevelItemCount());
}

void MainWindow::file_change(){
  ui->label_3->setText("status:some file or directory has been changed. please searching again ");
}

void MainWindow::index_end() {
    index_now = false;
    ui->label_2->setText("status:indexing is complete you can write a string");
}

void MainWindow::search_end() {
    ui->label_3->setText("status:searching is complete");
}


