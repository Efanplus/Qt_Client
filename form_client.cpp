#include "form_client.h"
#include "ui_form_client.h"

#include <QFileDialog>
#include <QDir>
#include <QtWidgets>
#include <QtNetwork>
#include <QDebug>
#include <QByteArray>
#include <QMessageBox>
#include <fstream>
#include <sstream>

Form_client::Form_client(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_client)
{
    ui->setupUi(this);
    this->setLayout(ui->verticalLayout);

    tcpSocket = new QTcpSocket(this);
    ui->hostCombo->setEditable(true);
    // find out name of this machine
    QString name = QHostInfo::localHostName();
    if (!name.isEmpty()) {
        ui->hostCombo->addItem(name);
        QString domain = QHostInfo::localDomainName();
        if (!domain.isEmpty())
            ui->hostCombo->addItem(name + QChar('.') + domain);
    }
    if (name != QLatin1String("localhost"))
        ui->hostCombo->addItem(QString("localhost"));
    // find out IP addresses of this machine
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // add non-localhost addresses
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (!ipAddressesList.at(i).isLoopback())
            ui->hostCombo->addItem(ipAddressesList.at(i).toString());
    }
    // add localhost addresses
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i).isLoopback())
            ui->hostCombo->addItem(ipAddressesList.at(i).toString());
    }
    ui->portLineEdit->setValidator(new QIntValidator(1,65535,this));

//! [1]
    //in.setDevice(tcpSocket);
    //in.setVersion(QDataStream::Qt_4_0);
//! [1]

    connect(ui->hostCombo, &QComboBox::editTextChanged, this, &Form_client::enableGetFortuneButton);
    connect(ui->portLineEdit, &QLineEdit::textChanged, this, &Form_client::enableGetFortuneButton);


//! [2] //! [3]
    connect(tcpSocket, &QIODevice::readyRead, this, &Form_client::readFortune);
//! [2] //! [4]
    connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Form_client::displayError);
//! [4]

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, &QNetworkSession::opened, this, &Form_client::sessionOpened);

        ui->pushButton_connect->setEnabled(false);
        //statusLabel->setText(tr("Opening network session."));
        networkSession->open();
    }
}

Form_client::~Form_client()
{
    delete ui;
}

void Form_client::on_toolButton_clicked()
{
    directory =
            QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("Save path"),QDir::currentPath()));
    if(!directory.isEmpty()){
        if(ui->comboBox_2->findText(directory) == -1)
            ui->comboBox_2->addItem(directory);
        ui->comboBox_2->setCurrentIndex(ui->comboBox_2->findText(directory));
    }
}



void Form_client::readFortune()
{
    //in.startTransaction();
    QByteArray buf;
    buf = tcpSocket->readAll();



    if(!buf.isEmpty()){
        ui->plainTextEdit_get->insertPlainText(buf);
        ui->plainTextEdit_get->insertPlainText("\n");
    }
    QString sstr;
    sstr.prepend(buf);
    qDebug()<<sstr <<endl;
    buf.clear();
}

void Form_client::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(tcpSocket->errorString()));
    }

    ui->pushButton_connect->setEnabled(true);
}

void Form_client::enableGetFortuneButton()
{
    ui->pushButton_connect->setEnabled((!networkSession || networkSession->isOpen()) &&
                                 !ui->hostCombo->currentText().isEmpty() &&
                                       !ui->portLineEdit->text().isEmpty());
}

void Form_client::sessionOpened()
{
    // Save the used configuration
    QNetworkConfiguration config = networkSession->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice)
        id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
    else
        id = config.identifier();

    QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();



    enableGetFortuneButton();
}

void Form_client::on_pushButton_connect_clicked()
{
    ui->pushButton_connect->setEnabled(false);
    tcpSocket->abort();
//! [7]
    tcpSocket->connectToHost(ui->hostCombo->currentText(),ui->portLineEdit->text().toInt());
    //! [7]
}

void Form_client::on_pushButton_2_clicked()
{
    QByteArray senddata;
    senddata = ui->plainTextEdit_send->toPlainText().toLatin1();
    sendstr.append(senddata);
    int sendflag = tcpSocket->write(senddata);
    if(sendflag == -1)QMessageBox::information(this,"网络通信","发送数据失败");
    senddata.clear();
    sendstr.clear();
    ui->plainTextEdit_send->clear();
}

void Form_client::on_pushButton_3_clicked()
{
    sendstr = "1";
    int sendflag = tcpSocket->write(sendstr.toLatin1());
    if(sendflag == -1)QMessageBox::information(this,"网络通信","发送数据失败");
    sleepmsc(50);
    sendstr = "2";
    sendflag = tcpSocket->write(sendstr.toLatin1());
    if(sendflag == -1)QMessageBox::information(this,"网络通信","发送数据失败");
    sendstr.clear();
}

void Form_client::sleepmsc(int msc)
{
    QTime dieTime = QTime::currentTime().addMSecs(msc);
    while( QTime::currentTime() <dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

}

void Form_client::on_pushButton_clicked()
{
    qDebug() << directory << endl;
    QDateTime da_time;
    QString time_str=da_time.currentDateTime().toString("yyyy-MM-dd HH-mm-ss");
    QString path = directory + "\\" + time_str + ".txt";
    qDebug() << path << endl;
    QFile file(path);
    if(file.open(QIODevice::ReadWrite | QIODevice::Text)){
        QTextStream stream(&file);
        stream << ui->plainTextEdit_get->toPlainText() << endl;
    }
    file.close();

}
