#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>

namespace Ui {
class QLWContainer;
class MainWindow;
}
class QLWContainer : public QObject {
	Q_OBJECT
public:
	QListWidget *widget;
	QString l;
	QLWContainer(QListWidget *qlw, QString lang) {
		widget = qlw;
		l = lang;
	}
	QLWContainer() {
		l = "None";
		widget = NULL;
	}
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	QSettings *settings;
	QString requestString(QString displayText);
	void reloadLanguages();
	QVector<QLWContainer> containers;
public slots:
	void search(QString text);
	void add();
	void remove();
private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
