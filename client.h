#ifndef CLIENT_H
#define CLIENT_H

#include<QtNetwork/QTcpSocket>
#include<QtNetwork/QtNetwork>

class Client
{
public:
    Client();
    void setTimeout(int);
    int getTimeout();
    bool connect(QString,int);
    int getErrorCode();
    QString getErrorString();
    QStringList query(QString);
    bool isConnected();
    bool disconnect();
private:
    QTcpSocket *socket;
    int timeout;
    bool sendRequest(QString);
    QStringList readResponse();
};

#endif // CLIENT_H
