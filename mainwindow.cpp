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
	reload();
	sync();
	search("");
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::reload() {
	int i, j, k;
	i = 0;
	j = 0;
	QWidget *child;
	while ( ui->grid->count() > 0) {
		child = ui->grid->itemAt(0)->widget();
		ui->grid->removeWidget(child);
		delete child;
	}
	for (i = 0; i < containers.size(); i++) {
		delete containers[i];
	}
	containers.clear();
	settings->beginGroup("languages");
	QStringList groups = settings->childGroups();
	for (i = 0; i < groups.size(); i++) {
		QLabel *label = new QLabel(groups.at(i), this);
		QListWidget *qlw = new QListWidget(this);
		QPushButton *pb = new QPushButton(tr("Add word"), this);
		Language *lang = new Language(groups.at(i));
		langs.push_back(lang);
		containers.push_back(new QLWContainer(qlw, pb, lang));
		ui->grid->addWidget(label, j, i%5);
		ui->grid->addWidget(qlw, j+1, i%5);
		ui->grid->addWidget(pb, j+2, i%5);
		label->show();
		if ((i+1)%5 == 0) {
			j+=3;
		}
	}
	settings->endGroup();
	QStringList tmpWords;
	int size = settings->beginReadArray("words");
	for (i = 0; i < size; i++) {
		Word *tmp = new Word();
		settings->setArrayIndex(i);
		for (j = 0; j < langs.size(); j++) {
			settings->beginGroup(langs[j]->name);
			tmpWords = settings->childKeys();
			for (k = 0; k < tmpWords.size(); k++) {
				tmp->trans.push_back(RefWord(tmpWords[i], langs[j]));
			}
			settings->endGroup();
		}
		this->words.push_back(tmp);
	}
	settings->endArray();
	/*Word *tmp = new Word();
	tmp->trans.push_back(RefWord("Hei", langs[0]));
	tmp->trans.push_back(RefWord("lol", langs[0]));
	tmp->trans.push_back(RefWord("Hei", langs[1]));
	tmp->trans.push_back(RefWord("Hei", langs[2]));
	tmp->trans.push_back(RefWord("Hei", langs[3]));
	this->words.push_back(tmp);*/
}

void MainWindow::sync() {
	settings->remove("words");
	settings->beginWriteArray("words");
	for (int i = 0; i < words.size(); i++) {
		settings->setArrayIndex(i);
		for (int j = 0; j < words[i]->trans.size(); j++) {
			settings->beginGroup(words[i]->trans[j].l->name);
			settings->setValue(words[i]->trans[j].word,"");
			settings->endGroup();
		}
	}
	settings->endArray();
}

void MainWindow::addToContainer(QString text, Language *lang) {
	for (int i = 0; i < containers.size(); i++) {
		if (containers[i]->l == lang) {
			new QListWidgetItem(text, containers[i]->widget);
		}
	}
}

void MainWindow::showWords(QString text) {
	int i, j;
	for (int i = 0; i < containers.size(); i++) {
		while ( containers[i]->widget->count() > 0) {
			delete containers[i]->widget->item(0);
		}
	}
	for (i = 0; i < words.size(); i++) {
		for (j = 0; j < words[i]->trans.size(); j++) {
			if (words[i]->trans[j].word.contains(text)) {
				qDebug() << words[i]->trans[j].word;
				addToContainer(words[i]->trans[j].word, words[i]->trans[j].l);
			}
		}
	}
}

void MainWindow::search(QString text) {
	if (text.length() != 0){
		ui->curStatus->setText(tr("searching for: ") + text);
	} else {
		ui->curStatus->setText(tr("showing all words."));
	}
	showWords(text);
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
		settings->setValue("exists", true);
		settings->endGroup();
		settings->endGroup();
		reload();
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
		reload();
	}
}

Language::Language() {
	name = "none";
}

Language::Language(QString n) {
	name = n;
}
QLWContainer::QLWContainer(QListWidget *qlw, QPushButton *qpb, Language *lang) {
	button = qpb;
	widget = qlw;
	l = lang;
}
QLWContainer::QLWContainer() {
	l = NULL;
	widget = NULL;
}
RefWord::RefWord() {
	word = "None";
	l = NULL;
}
RefWord::RefWord(QString w, Language *lang) {
	word = w;
	l = lang;
}
bool Word::hasPhrase(QString ph) {
	for (int i = 0; i < trans.size(); i++) {
		if (trans[i].word.contains(ph)) {
			return true;
		}
	}
	return false;
}
