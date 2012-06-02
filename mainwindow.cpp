#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	settings = new QSettings("TZer0", "translate", this);
	this->connect(ui->searchLine, SIGNAL(textChanged(QString)), this, SLOT(search(QString)));;
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::search(QString text) {
	qDebug() << text;
}
