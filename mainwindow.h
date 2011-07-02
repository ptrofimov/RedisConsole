#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "client.h"
#include "console.h"
#include "connectdlg.h"
#include "contexthelp.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
private:
    QListWidget *keys;
    QPlainTextEdit *values;
    QSplitter *vertical;
    Console *console;
    Client *client;
    bool isConsoleMode;
    QAction *consoleModeAction;
    QString host;
    QString port;
    QString dbNumber;
    QString prefix;
    bool isConnected;
    QLineEdit *filter;
    ConnectDlg *connectDlg;
    ContextHelp *contextHelp;

    void setupPanels();
    void buildMenu();
    void setTitle(QString);
    bool checkConnection();
    bool openConnection(QString, QString, QString, QString);
private slots:
    void updateKeys();
    void openConnectionDialog();
    void closeConnection();
    void onCommand(QString);
    void onKeySelect();
    void consoleMode();
    void showAbout();
    void showReference();
    void onFilter(QString);
    void showServerInfo();
};

#endif // MAINWINDOW_H
