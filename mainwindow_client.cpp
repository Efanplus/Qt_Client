#include "mainwindow_client.h"
#include "ui_mainwindow_client.h"
#include <QGridLayout>

MainWindow_Client::MainWindow_Client(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow_Client)
{
    ui->setupUi(this);
    Form_client *fc = new Form_client;
    QGridLayout *glayout = new QGridLayout(ui->centralWidget);
    glayout->addWidget(fc);
    this->setLayout(glayout);
}

MainWindow_Client::~MainWindow_Client()
{
    delete ui;
}
