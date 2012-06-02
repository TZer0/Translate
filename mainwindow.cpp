#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "TZer0", "translate", this);
	qDebug() << settings->fileName();
	connect(ui->searchLine, SIGNAL(textChanged(QString)), this, SLOT(search(QString)));;
	connect(ui->addLanguage, SIGNAL(clicked()), this, SLOT(add()));
	connect(ui->removeLanguage, SIGNAL(clicked()), this, SLOT(remove()));
	ui->curStatus->setText(tr("showing all words"));
	reloadLanguages();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::reloadLanguages() {
	int i, j;
	i = 0;
	j = 0;
	QWidget *child;
	qDebug() << ui->grid->count();
	while ( ui->grid->count() > 0) {
		child = ui->grid->itemAt(0)->widget();
		ui->grid->removeWidget(child);
		delete child;
	}
	qDebug() << ui->grid->count();
	settings->beginGroup("languages");
	QStringList groups = settings->childGroups();
	for (i = 0; i < groups.size(); i++) {
		qDebug() << groups.at(i);
		QLabel *label = new QLabel(groups.at(i), this);
		QListWidget *qlw = new QListWidget(this);
		QPushButton *pb = new QPushButton(tr("Add word"), this);
		ui->grid->addWidget(label, j, i%5);
		ui->grid->addWidget(qlw, j+1, i%5);
		ui->grid->addWidget(pb, j+2, i%5);
		label->show();
		if ((i+1)%5 == 0) {
			j+=3;
		}
	}
	settings->endGroup();
}

void MainWindow::search(QString text) {
	qDebug() << text;
}

void MainWindow::add() {
	bool ok;
	QString reply = QInputDialog::getText(this, "Translate", tr("Please enter language to add: "),QLineEdit::Normal,
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
		reloadLanguages();
	}
}
void MainWindow::remove() {
	bool ok;
	QString reply = QInputDialog::getText(this, "Translate", tr("Please enter language to remove: ") ,QLineEdit::Normal,
					      QString::null, &ok, this->windowFlags());
	if (ok && !reply.isEmpty()) {
		settings->beginGroup("languages");
		settings->remove(reply);
		settings->endGroup();
		qDebug() << reply;
		reloadLanguages();
	}
}

