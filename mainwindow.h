#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	QSettings *settings;
	QString requestString(QString displayText);
	void reloadLanguages();
public slots:
	void search(QString text);
	void add();
	void remove();
private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
