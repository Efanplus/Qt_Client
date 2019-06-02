#ifndef FORM_CLIENT_H
#define FORM_CLIENT_H

#include <QWidget>
#include <QString>

#include <QDataStream>
#include <QDialog>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTcpSocket;
class QNetworkSession;
QT_END_NAMESPACE

namespace Ui {
class Form_client;
}

class Form_client : public QWidget
{
    Q_OBJECT

public:
    explicit Form_client(QWidget *parent = nullptr);
    ~Form_client();

    QString directory;
    QTcpSocket *tcpSocket = nullptr;
    //QDataStream in;
    QString currentFortune;
    QString sendstr;
    QNetworkSession *networkSession = nullptr;

private slots:
    void on_toolButton_clicked();

private slots:
    void readFortune();
    void displayError(QAbstractSocket::SocketError socketError);
    void enableGetFortuneButton();
    void sessionOpened();

    void on_pushButton_connect_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

private:
    Ui::Form_client *ui;
    void sleepmsc(int msc);
};

#endif // FORM_CLIENT_H
