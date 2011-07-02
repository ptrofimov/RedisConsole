#ifndef CONTEXTHELP_H
#define CONTEXTHELP_H

#include <QObject>
#include <QtGui>

class ContextHelp : public QObject
{
    Q_OBJECT
public:
    explicit ContextHelp(QStatusBar *, QObject *parent = 0);

private:
    QStatusBar *statusBar;
    QHash<QString, QString> commands;
    QString lastCmd;
    QString lastPrint;

    void loadCommands();
    void print(QString);
    void showPrompt();

signals:

public slots:
    void onUserInput(QString);

};

#endif // CONTEXTHELP_H
