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
	int i;
	QLayoutItem *child;
	while ((child = ui->grid->takeAt(0)) != 0) {
		delete child;
	}
	settings->beginGroup("languages");
	QStringList groups = settings->childGroups();
	for (i = 0; i < groups.size(); i++) {
		qDebug() << groups.at(i);
		QLabel *label = new QLabel(groups.at(i), this);
		QListWidget *qlw = new QListWidget();
		ui->grid->addWidget(label, 0, i);
		ui->grid->addWidget(qlw, 1, i);
		label->show();
	}
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

