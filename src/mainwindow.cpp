#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QSizePolicy>
#include <QDebug>
#include <QFileDialog>
#include <iostream>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), index_now(false), ui(new Ui::MainWindow), index_thread(new QThread()), search_thread(new QThread) {
    ui->setupUi(this);

    qRegisterMetaType<std::pair<QString, QVector<QString>>>();
    connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(open_file(QTreeWidgetItem*)));

    connect(ui->SelectDirectory, &QPushButton::clicked, this, &MainWindow::select_directory);
    connect(ui->IndexStop, &QPushButton::clicked, this, &MainWindow::cancel_indexing);
    connect(ui->StartStopSearch, &QPushButton::clicked, this, &MainWindow::start_search);
    ui->treeWidget->setColumnWidth(0, 460);
    ui->treeWidget->setStyleSheet("background-color: rgb(95,95,95);");

    ui->progressBar->setMaximum(1);
    ui->progressBar->setValue(0);
}

MainWindow::~MainWindow() {
    stop_index();
    stop_search();
}
void MainWindow::cancel_indexing(){
    ui->label_2->setText("Scanner Status: canceled");
    ui->label_2->setStyleSheet("background-color: rgb(255 , 0, 27); ");
    cancel=true;
    stop_index();
}

void MainWindow::select_directory() {
    cancel=false;
    cur_dir = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning", QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    start_index();
    ui->label->setText("current directory:" + cur_dir);
}

void MainWindow::start_index() {
    stop_index();
    ui->label_2->setText("Scanner Status: processing...");
    ui->label_2->setStyleSheet("background-color: rgb(255 , 239, 126); ");

    index_now = true;
    files_triggers.clear();
    index_search* worker = new index_search(mtx, files_triggers, cur_dir);
    worker->moveToThread(index_thread.get());

    connect(worker, SIGNAL(file_change(QString)), this, SLOT(file_change(QString)));
    connect(worker, SIGNAL(set_max_progress(int)), ui->progressBar, SLOT(setMaximum(int)));
    connect(worker, SIGNAL(set_progress(int)), ui->progressBar, SLOT(setValue(int)));

    connect(index_thread.get(), SIGNAL (started()), worker, SLOT (start_index()));
    connect(worker, SIGNAL (finished()), this, SLOT (index_end()));
    connect(worker, SIGNAL (index_end()), this, SLOT (index_end()));
    //index_end();
    index_thread->start();
}

void MainWindow::start_search() {
    if(index_now) {
        ui->treeWidget->clear();
        ui->label_3->setText("Seraching Status: Wait! scanning in progress");
        ui->label_3->setStyleSheet("background-color: rgb(255 , 0, 27); ");
        return;
    }
    stop_search();
    ui->treeWidget->clear();
    ui->lcdNumber->display(0);

    search_time.start();
    match_string* worker = new match_string(mtx, files_triggers, ui->MatchString->text());
    worker->moveToThread(search_thread.get());

    connect(worker, SIGNAL(set_max_progress(int)), ui->progressBar, SLOT(setMaximum(int)));
    connect(worker, SIGNAL(set_progress(int)), ui->progressBar, SLOT(setValue(int)));
    connect(worker, SIGNAL(matched(std::pair<QString, QVector<QString>>)), this, SLOT(add_path(std::pair<QString, QVector<QString>>)));

    connect(search_thread.get(), SIGNAL(started()), worker, SLOT(start_search()));
    connect(worker, SIGNAL (finished()), search_thread.get(), SLOT (quit()));
    connect(worker, SIGNAL (finished()), worker, SLOT (deleteLater()));
    connect(worker, SIGNAL (finished()), this, SLOT (search_end()));

    search_thread->start();
    ui->label_3->setText("Seraching Status: processing...");
    ui->label_3->setStyleSheet("background-color: rgb(255 , 239, 126); ");

}

void MainWindow::stop_index() {

    if(index_thread != nullptr && index_thread->isRunning()){
        index_thread->requestInterruption();
        index_thread->quit();
        index_thread->wait();
        index_thread.reset(new QThread);
    }
}

void MainWindow::stop_search() {
    if(search_thread != nullptr && search_thread->isRunning()){
        search_thread->requestInterruption();
        search_thread->quit();
        search_thread->wait();
        //ui->label_3->setText("status:search canceled");
    }
}

void MainWindow::add_path(std::pair<QString, QVector<QString>> v){
    QTreeWidgetItem *ti = new QTreeWidgetItem();
    cnt++;int color1 = rand() % 200 + 55;
    int color2 = rand() % 200 + 55;
    int color3 = rand() % 200 + 55;
    ti->setTextColor(0, QColor::fromRgb(color1, color2, color3));
    ti->setTextColor(1, QColor::fromRgb(color1, color2, color3));
    ti->setText(0, v.first);
    ti->setText(1, QString::number(v.second.size()));
  //  ti->setText(2, QString::number(v.second.first().size()));
   // int tsize = 0;
    for (auto const & k: v.second) {
       // tsize += k.size();
        QTreeWidgetItem *child = new QTreeWidgetItem();
        child->setText(0, k);
        child->setText(1,QString("samar"));
        child->setTextColor(0, QColor::fromRgb(color1, color2, color3));
        ti->addChild(child);
    }
   // ti->setText(3, QString::number(tsize));
    ui->treeWidget->addTopLevelItem(ti);

    ui->lcdNumber->display(ui->treeWidget->topLevelItemCount());
}

void MainWindow::file_change(QString fname){
    ui->label_3->setText(QString("file") + QString("\"") + fname + QString("\"") +  QString(" has been changed"));
    ui->label_3->setStyleSheet("background-color: rgb(255 , 0, 27); ");
}


void MainWindow::index_end() {
    index_now = false;
    if(!cancel){
        ui->label_2->setText("Scanner Status: completed");
        ui->label_2->setStyleSheet("background-color: rgb(0 , 255, 4); ");
    }
    cancel=false;
}

void MainWindow::search_end() {
    ui->label_3->setText("Seraching Status:  is complete");
    ui->label_3->setStyleSheet("background-color: rgb(0 , 255, 4); ");

}


