#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>

namespace Ui {
class QLWContainer;
class MainWindow;
class Word;
class refWord;
class Language;
}
class Language {
public:
	QString name;
	Language(QString n);
	Language();
};

class RefWord {
public:
	QString word;
	Language *l;
	RefWord();
	RefWord(QString, Language*);
};

class Word {
public:
	QVector<RefWord> trans;
	Word() {}
	bool hasPhrase(QString);
};

class QLWContainer : public QObject {
	Q_OBJECT
public:
	QListWidget *widget;
	Language *l;
	QLWContainer(QListWidget *qlw, Language *lang);
	QLWContainer ();
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	QSettings *settings;
	QString requestString(QString displayText);
	QVector<QLWContainer> containers;
	QVector<Language *> langs;
	QVector<Word *> words;
	void sync();
	void reload();
public slots:
	void search(QString text);
	void add();
	void remove();
private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
