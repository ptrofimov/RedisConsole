#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(QSize(800,500));
    client=new Client();

    connectDlg = new ConnectDlg(this);
    contextHelp = new ContextHelp(statusBar(), this);

    setupPanels();
    buildMenu();
    console->setFocus();
    setTitle("[disconnected]");
    setWindowIcon(QIcon(":/icons/terminal.png"));
    isConsoleMode = false;
    isConnected = false;

    showAbout();
}

void MainWindow::openConnectionDialog()
{
    if(connectDlg->exec() == QDialog::Accepted)
    {
	closeConnection();
	if(openConnection(connectDlg->getHost(), connectDlg->getPort(), connectDlg->getDbNumber(), connectDlg->getPrefix()))
	    updateKeys();
    }
}

bool MainWindow::openConnection(QString host, QString port, QString dbNumber, QString prefix)
{
    if(!client->connect(host, port.toInt()))
    {
	QMessageBox::critical(this, "Error", client->getErrorString());
	isConnected = false;
	return false;
    }
    QStringList selectResult = client->query(QString("SELECT %1").arg(dbNumber));
    if(selectResult.at(0) != "string" || selectResult.at(1) != "OK")
    {
	QMessageBox::critical(this, "Error", QString("Can't select database %1").arg(dbNumber));
	closeConnection();
	return false;
    }
    this->host = host;
    this->port = port;
    this->dbNumber = dbNumber;
    this->prefix = prefix;
    isConnected = true;
    filter->setText("");
    if(!prefix.isEmpty())
	setTitle(QString("[%1:%2,db%3#%4]").arg(host).arg(port).arg(dbNumber).arg(prefix));
    else
	setTitle(QString("[%1:%2,db%3]").arg(host).arg(port).arg(dbNumber));
    return true;
}

void MainWindow::updateKeys()
{
    if(!checkConnection())
	return;
    QList<QListWidgetItem *> list = keys->selectedItems();
    QString oldKey = "";
    if(!list.isEmpty())
	oldKey = list.at(0)->text();
    QStringList response=client->query(QString("KEYS \"%1*\"").arg(prefix));
    keys->clear();
    QString key;
    QListWidgetItem *item;
    for(int i=1;i<response.length();i++)
    {
	key = response.at(i);
	item = new QListWidgetItem(QIcon(":/icons/key.png"), key);
	keys->addItem(item);
	if(!oldKey.isEmpty() && oldKey == key)
	    item->setSelected(true);
	if(filter->text().isEmpty() || item->text().startsWith(filter->text()))
	    item->setHidden(false);
	else
	    item->setHidden(true);
    }
    keys->sortItems();
}

void MainWindow::setupPanels()
{
    keys = new QListWidget;
    values = new QPlainTextEdit;
    console = new Console;
    filter = new QLineEdit;

    values->setTabStopWidth(8);
    values->setReadOnly(true);
    filter->setPlaceholderText("quick key filter");

    QFont font;
    font.setFamily("Courier New");
    font.setPointSize(10);
    font.setStyleStrategy(QFont::NoAntialias);
    keys->setFont(font);
    values->setFont(font);
    console->setFont(font);
    filter->setFont(font);

    QWidget *keysPanel = new QWidget;
    QVBoxLayout *keysPanelLayout = new QVBoxLayout;
    keysPanelLayout->addWidget(filter);
    keysPanelLayout->addWidget(keys);
    keysPanelLayout->setMargin(0);
    keysPanelLayout->setSpacing(0);
    keysPanel->setLayout(keysPanelLayout);

    QSplitter *horizontal = new QSplitter(Qt::Horizontal, this);
    horizontal->addWidget(keysPanel);
    horizontal->addWidget(values);
    horizontal->setStretchFactor(0, 1);
    horizontal->setStretchFactor(1, 3);

    vertical = new QSplitter(Qt::Vertical, this);
    vertical->addWidget(horizontal);
    vertical->addWidget(console);
    QList<int> list;
    list << 200 << 100;
    vertical->setSizes(list);

    setCentralWidget(vertical);

    connect(keys, SIGNAL(itemSelectionChanged()), this, SLOT(onKeySelect()));
    connect(console, SIGNAL(onCommand(QString)), this, SLOT(onCommand(QString)));
    connect(filter, SIGNAL(textChanged(QString)), this, SLOT(onFilter(QString)));
    connect(console, SIGNAL(onChange(QString)), contextHelp, SLOT(onUserInput(QString)));
}

void MainWindow::buildMenu()
{
    QMenu *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&Open Connection", this, SLOT(openConnectionDialog()), QKeySequence("Ctrl+O"));
    fileMenu->addAction("&Close", this, SLOT(closeConnection()), QKeySequence("Ctrl+W"));
    fileMenu->addSeparator();
    fileMenu->addAction("&Quit", this, SLOT(close()), QKeySequence("Alt+F4"));

    QMenu *viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction("&Update", this, SLOT(updateKeys()), QKeySequence("F5"));
    viewMenu->addSeparator();
    viewMenu->addAction("Server &Info", this, SLOT(showServerInfo()));
    viewMenu->addSeparator();
    consoleModeAction = viewMenu->addAction("&Console Mode", this, SLOT(consoleMode()), QKeySequence("F4"));
    consoleModeAction->setCheckable(true);

    QMenu *helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("Online Reference", this, SLOT(showReference()), QKeySequence("Shift+F1"));
    helpMenu->addSeparator();
    helpMenu->addAction("&About", this, SLOT(showAbout()), QKeySequence("F1"));
}

void MainWindow::closeConnection()
{
    if(client->isConnected())
    {
	client->disconnect();
    }
    isConnected = false;
    keys->clear();
    setTitle("[disconnected]");
}

void MainWindow::setTitle(QString title)
{
    setWindowTitle("RedisConsole " + title);
}

void MainWindow::onCommand(QString cmd)
{
    if(!checkConnection())
    {
	console->output("(error) no connection");
	return;
    }

    QStringList response;
    response=client->query(cmd);

    if(response.isEmpty())
    {
	console->output("(error) empty response");
	return;
    }

    QString type=response.at(0);
    if(type=="string")
	console->output(response.at(1));
    else if(type=="error")
	console->output("(error) "+response.at(1));
    else if(type=="integer")
	console->output("(integer) "+response.at(1));
    else if(type=="bulk")
	console->output("\""+response.at(1)+"\"");
    else if(type=="list"){
	QStringList output;
	if(response.length()>1)
	{
	    for(int k=1;k<response.length();k++)
		output << QString("%1").arg(k)+". \""+response.at(k)+"\"";
	    console->output(output.join("\r\n"));
	}
	else
	    console->output("empty");
    }
}
void MainWindow::showServerInfo()
{
    if(!checkConnection())
	return;
    QStringList response = client->query("INFO");
    QString type = response.at(0);

    QString output;

    if(type == "bulk")
	output = "SERVER INFO:\r\n" + response.at(1);
    else
	output = "(none)";
    values->setPlainText(output);
}

void MainWindow::onKeySelect()
{
    QList<QListWidgetItem *> list = keys->selectedItems();
    if(list.isEmpty())
    {
	values->setPlainText("");
	return;
    }
    if(!checkConnection())
	return;
    QString key = list.at(0)->text();
    QStringList response;
    response = client->query("type " + key);
    QString type = response.at(1);

    QString output;

    if(type == "none")
	output = "(none)";
    else if(type == "string")
    {
	response = client->query("get " + key);
	output = QString("string \"%1\"").arg(response.at(1));
    }
    else if(type == "list")
    {
	response = client->query("lrange " + key + " 0 -1");
	output = QString("list[%1] {\r\n").arg(response.length() - 1);
	for(int i = 1; i < response.length(); i++)
	{
	    output.append(QString("\t \"%1\"\r\n").arg(response.at(i)));
	}
	output.append("}");
    }
    else if(type == "set")
    {
	response = client->query("smembers " + key);
	output = QString("set[%1] {\r\n").arg(response.length() - 1);
	for(int i = 1; i < response.length(); i++)
	{
	    output.append(QString("\t \"%1\"\r\n").arg(response.at(i)));
	}
	output.append("}");
    }
    else if(type == "zset")
    {
	response = client->query("zrange " + key + " 0 -1");
	output = QString("zset[%1] {\r\n").arg(response.length() - 1);
	for(int i = 1; i < response.length(); i++)
	{
	    output.append(QString("\t \"%1\"\r\n").arg(response.at(i)));
	}
	output.append("}");
    }
    else if(type == "hash")
    {
	response = client->query("hvals " + key);
	output = QString("hash[%1] {\r\n").arg(response.length() - 1);
	for(int i = 1; i < response.length(); i++)
	{
	    output.append(QString("\t \"%1\"\r\n").arg(response.at(i)));
	}
	output.append("}");
    }

    values->setPlainText(output);
}

void MainWindow::consoleMode()
{
    QList<int> list;
    if(!isConsoleMode)
	list << 0 << 300;
    else
	list << 200 << 100;
    vertical->setSizes(list);
    console->setFocus();
    console->scrollDown();
    isConsoleMode = ! isConsoleMode;
    consoleModeAction->setChecked(isConsoleMode);
}

void MainWindow::showAbout()
{
    QFile file(":/about.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
	return;
    QTextStream in(&file);
    values->setPlainText(in.readAll());
}

void MainWindow::showReference()
{
    QDesktopServices::openUrl(QUrl("http://redis.io/commands"));
}

bool MainWindow::checkConnection()
{
    if(isConnected)
    {
	if(!client->isConnected())
	    if(!openConnection(host, port, dbNumber, prefix))
	    {
	    closeConnection();
	    return false;
	}
	return true;
    }
    return false;
}

void MainWindow::onFilter(QString filter)
{
    QListWidgetItem *item;
    for(int i = 0; i < keys->count(); i++)
    {
	item = keys->item(i);
	if(filter.isEmpty() || item->text().startsWith(filter))
	    item->setHidden(false);
	else
	    item->setHidden(true);
    }
}
