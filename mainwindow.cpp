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
	connect(ui->clearSearch, SIGNAL(clicked()), this, SLOT(clearField()));
	reload();
	sync();
	search("");
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::reload() {
	selected = NULL;
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
		connect(qlw, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(findWord(QListWidgetItem*)));
		QPushButton *add = new QPushButton(tr("Add word"), this);
		QPushButton *remove = new QPushButton(tr("Remove word"), this);
		Language *lang = new Language(groups.at(i));
		QLineEdit *qle = new QLineEdit();
		langs.push_back(lang);
		QLWContainer *tmp = new QLWContainer(qlw, add, remove, qle, lang);
		connect(add, SIGNAL(clicked()), tmp, SLOT(addClick()));
		connect(remove, SIGNAL(clicked()), tmp, SLOT(removeClick()));
		connect(tmp, SIGNAL(passAddClick(QString,Language*)), this, SLOT(addWord(QString,Language*)));
		connect(tmp, SIGNAL(passRemoveClick(RefWord*)), this, SLOT(removeWord(RefWord*)));
		containers.push_back(tmp);
		ui->grid->addWidget(label, j, i%5);
		ui->grid->addWidget(qlw, j+1, i%5);
		ui->grid->addWidget(remove, j+2, i%5);
		ui->grid->addWidget(qle, j+3, i%5);
		ui->grid->addWidget(add, j+4, i%5);
		label->show();
		if ((i+1)%5 == 0) {
			j+=5;
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
				tmp->trans.push_back(new RefWord(tmpWords[k], langs[j]));
			}
			settings->endGroup();
		}
		this->words.push_back(tmp);
	}
	settings->endArray();
	/*Word *tmp = new Word();
	tmp->trans.push_back(RefWord("hm", langs[0]));
	tmp->trans.push_back(RefWord("a", langs[0]));
	tmp->trans.push_back(RefWord("b", langs[1]));
	tmp->trans.push_back(RefWord("c", langs[2]));
	tmp->trans.push_back(RefWord("d", langs[3]));
	this->words.push_back(tmp);*/
}

void MainWindow::sync() {
	settings->remove("words");
	settings->beginWriteArray("words");
	for (int i = 0; i < words.size(); i++) {
		settings->setArrayIndex(i);
		for (int j = 0; j < words[i]->trans.size(); j++) {
			settings->beginGroup(words[i]->trans[j]->l->name);
			settings->setValue(words[i]->trans[j]->text(),"");
			settings->endGroup();
		}
	}
	settings->endArray();
}

void MainWindow::addToContainer(RefWord *word) {
	for (int i = 0; i < containers.size(); i++) {
		if (containers[i]->l == word->l) {
			containers[i]->widget->addItem(word);
		}
	}
}

void MainWindow::clearContainers() {
	for (int i = 0; i < containers.size(); i++) {
		while (containers[i]->widget->count() > 0) {
			containers[i]->widget->takeItem(0);
		}
	}
}

void MainWindow::showWords(QString text) {
	clearContainers();
	text = text.toLower();
	int i, j;
	for (i = 0; i < words.size(); i++) {
		for (j = 0; j < words[i]->trans.size(); j++) {
			if (words[i]->trans[j]->text().toLower().contains(text)) {
				addToContainer(words[i]->trans[j]);
			}
		}
	}
}

void MainWindow::search(QString text) {
	selected = NULL;
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
		reload();
	}
}
void MainWindow::findWord(QListWidgetItem *word){
	clearContainers();
	RefWord *tmp = (RefWord*) word;
	ui->curStatus->setText("Showing relatives for " + tmp->text() + "/" + tmp->l->name);
	for (int i = 0; i < words.size(); i++) {
		if (words[i]->containsRef(tmp)) {
			selected = words[i];
			for (int j = 0; j < words[i]->trans.size(); j++) {
				addToContainer(words[i]->trans[j]);
			}
		}
	}
}

void MainWindow::addWord(QString w, Language *l) {
	if (selected) {
		selected->trans.push_back(new RefWord(w, l));
		addToContainer(selected->trans[selected->trans.size()-1]);
		sync();
	} else {
		Word *tmp = new Word();
		tmp->trans.push_back(new RefWord(w, l));
		words.push_back(tmp);
		sync();
		findWord(tmp->trans[0]);
	}
}

void MainWindow::removeWord(RefWord *word) {
	for (int i = 0; i < words.size(); i++) {
		for (int j = 0; j < words[i]->trans.size(); j++) {
			if (words[i]->trans[j] == word) {
				RefWord *tmp = words[i]->trans[j];
				words[i]->trans.remove(j);
				delete tmp;
				if ( words[i]->trans.isEmpty()) {
					Word *tmpW = words[i];
					words.remove(i);
					if (tmpW == selected) {
						selected = NULL;
					}
					delete tmpW;
				}
			}
		}
	}
	sync();
	if (selected) {
		findWord(selected->trans[0]);
	} else {
		search(ui->searchLine->text());
	}
}
void MainWindow::clearField() {
	ui->searchLine->setText("");
	search("");
}

Language::Language() {
	name = "none";
}

Language::Language(QString n) {
	name = n;
}
QLWContainer::QLWContainer(QListWidget *qlw, QPushButton *a, QPushButton *r, QLineEdit *qle, Language *lang) {
	addLine = qle;
	add = a;
	remove = r;
	widget = qlw;
	l = lang;
}
QLWContainer::QLWContainer() {
	addLine = NULL;
	add = NULL;
	remove = NULL;
	l = NULL;
	widget = NULL;
}
void QLWContainer::addClick() {
	emit passAddClick(addLine->text(), l);
}
void QLWContainer::removeClick() {
	QList<QListWidgetItem *> sel = widget->selectedItems();
	for (int i = 0; i < sel.size(); i++ ){
		emit passRemoveClick((RefWord*)sel[i]);
	}
}

RefWord::RefWord() {
	setText("None");
	l = NULL;
}
RefWord::RefWord(QString w, Language *lang) {
	setText(w);
	l = lang;
}
bool Word::containsRef(RefWord *word) {
	for (int i = 0; i < trans.size(); i++) {
		if (trans[i] == word) {
			return true;
		}
	}
	return false;
}
