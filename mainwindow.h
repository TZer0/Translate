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
class MapStore;
}
class MapStore : public QObject {
	Q_OBJECT
public:
	QPixmap *map;
	MapStore(QPixmap*);
public slots:
	void save();
};
class Language {
public:
	QString name;
	Language(QString n);
	Language();
};

class RefWord : public QListWidgetItem {
public:
	Language *l;
	RefWord();
	RefWord(QString, Language*);
};

class Word {
public:
	QVector<RefWord*> trans;
	Word() {}
	bool containsRef(RefWord*);
};

class QLWContainer : public QObject {
	Q_OBJECT
public:
	QLineEdit *addLine;
	QListWidget *widget;
	Language *l;
	QSpinBox *x, *y, *rot;
	QLWContainer(QListWidget *, QLineEdit*, QSpinBox*, QSpinBox*, QSpinBox*, Language*);
	QLWContainer ();
public slots:
	void addClick();
	void removeClick();
signals:
	void passAddClick(QString, Language*);
	void passRemoveClick(RefWord*);
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	QSettings *settings;
	QString requestString(QString displayText);
	QVector<QLWContainer *> containers;
	QVector<Language *> langs;
	QVector<Word *> words;
	Word *selected;
	void clearContainers();
	void addToContainer(RefWord*);
	void showWords(QString);
	void sync();
	void reload();
	void sort();
public slots:
	void findWord(QListWidgetItem*);
	void search(QString);
	void add();
	void remove();
	void clearField();
	void addWord(QString, Language *);
	void removeWord(RefWord*);
	void loadImage();
	void genImage();
	void aboutClick();
	void sourceClick();
	void storeSize(int size);
private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
