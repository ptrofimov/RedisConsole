#include "connectdlg.h"

ConnectDlg::ConnectDlg(QWidget *parent) :
	QDialog(parent)
{
    setWindowTitle("Open connection");
    setModal(true);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowCloseButtonHint);
    resize(QSize(220, 120));

    host = new QLineEdit;
    port = new QLineEdit;
    dbNumber = new QLineEdit;
    prefix = new QLineEdit;
    buttonBox = new QDialogButtonBox;
    QPushButton *openButton = buttonBox->addButton(QDialogButtonBox::Open);
    buttonBox->addButton(QDialogButtonBox::Cancel);

    host->setText("localhost");
    host->setMaxLength(255);
    port->setText("6379");
    port->setValidator(new QIntValidator(0, 32768, this));
    dbNumber->setText("0");
    dbNumber->setValidator(new QIntValidator(0, 255, this));
    prefix->setText("");
    prefix->setMaxLength(32);
    openButton->setDefault(true);

    QFormLayout *layout = new QFormLayout(this);
    layout->addRow(new QLabel("Host:"), host);
    layout->addRow(new QLabel("Port:"), port);
    layout->addRow(new QLabel("DB number:"), dbNumber);
    layout->addRow(new QLabel("Prefix:"), prefix);
    layout->addWidget(buttonBox);
    setLayout(layout);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(onOpen()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));

    host->setFocus();
    host->selectAll();
}

QString ConnectDlg::getHost()
{
    return host->text().trimmed();
}

QString ConnectDlg::getPort()
{
    return port->text();
}

QString ConnectDlg::getDbNumber()
{
    return dbNumber->text();
}

QString ConnectDlg::getPrefix()
{
    return prefix->text().trimmed();
}

void ConnectDlg::onOpen()
{
    if(!getHost().isEmpty() && !getPort().isEmpty() && !getDbNumber().isEmpty())
    {
	accept();
    }
    else
    {
	QMessageBox::information(this, "Notice", "Host, port and db number are required fields");
    }
}
