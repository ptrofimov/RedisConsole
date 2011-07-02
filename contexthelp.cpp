#include "contexthelp.h"

ContextHelp::ContextHelp(QStatusBar *statusBar, QObject *parent) :
	QObject(parent)
{
    this->statusBar = statusBar;
    loadCommands();
    showPrompt();
}

void ContextHelp::print(QString s)
{
    statusBar->showMessage(s);
}

void ContextHelp::onUserInput(QString input)
{
    if(input.isEmpty())
    {
	showPrompt();
	return;
    }
    if(commands.isEmpty())
    {
	print("Commands are not loaded");
	return;
    }
    int pos = input.indexOf(' ');
    QString cmd = input.toUpper().left(pos);
    if(pos > 0 && lastCmd == cmd)
    {
	print(lastPrint);
	return;
    }
    QString help = "Unknown command";
    QStringList found;
    QHashIterator<QString, QString> i(commands);
    while (i.hasNext()) {
	i.next();
	if((pos == -1 && i.key().startsWith(cmd)) || i.key() == cmd)
	    found << i.key();
    }
    found.sort();
    if(found.length() > 1)
	help = found.join(" ");
    if(found.length() == 1)
	help = commands[found.at(0)];
    if(pos != -1)
	lastCmd = cmd;
    else
	lastCmd = "";
    lastPrint = help;
    print(help);
}

void ContextHelp::showPrompt()
{
    print("Start typing in console to get context help");
}

void ContextHelp::loadCommands()
{
    QFile file(":/commands.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
	QMessageBox::critical(0, "Context help", "Can\'t open commands resource");
	return;
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
	QString line = in.readLine().trimmed();
	if(!line.isEmpty() && !in.atEnd())
	{
	    QString nextLine = in.readLine().trimmed();
	    commands[line.left(line.indexOf(' '))] = line + " - " + nextLine;
	}
    }
}
