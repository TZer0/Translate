#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "TZer0", "translate", this);
	qDebug() << settings->fileName();
	this->connect(ui->searchLine, SIGNAL(textChanged(QString)), this, SLOT(search(QString)));;
	this->connect(ui->addLanguage, SIGNAL(clicked()), this, SLOT(add()));
	this->connect(ui->removeLanguage, SIGNAL(clicked()), this, SLOT(remove()));
	reloadLanguages();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::reloadLanguages() {
	settings->beginGroup("languages");
	QStringList groups = settings->childGroups();
	settings->endGroup();
}

void MainWindow::search(QString text) {
	qDebug() << text;
}

void MainWindow::add() {
	bool ok;
	QString reply = QInputDialog::getText(this, "Translate", "Please enter language to add: ",QLineEdit::Normal,
					      QString::null, &ok, this->windowFlags());
	if ( ok && !reply.isEmpty()) {
		settings->beginGroup("languages");
		settings->beginGroup(reply);
		if (settings->value("exists", false).toBool()) {
			settings->endGroup();
			settings->endGroup();
			return;
		}
		settings->setValue("exists", 1);
		settings->endGroup();
		settings->endGroup();
	}
	reloadLanguages();
}
void MainWindow::remove() {
	bool ok;
	QString reply = QInputDialog::getText(this, "Translate", "Please enter language to remove: ",QLineEdit::Normal,
					      QString::null, &ok, this->windowFlags());
	if (ok && !reply.isEmpty()) {
		settings->beginGroup("languages");
		settings->remove(reply);
		settings->endGroup();
		qDebug() << reply;
	}
}

