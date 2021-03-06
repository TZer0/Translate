#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "TZer0", "translate", this);
	ui->location->setEnabled(false);
	ui->location->setText("Data saved in: " + settings->fileName());
	connect(ui->goToLocation, SIGNAL(clicked()), this, SLOT(openFileManager()));
	connect(ui->searchLine, SIGNAL(textChanged(QString)), this, SLOT(search(QString)));;
	connect(ui->addLanguage, SIGNAL(clicked()), this, SLOT(add()));
	connect(ui->removeLanguage, SIGNAL(clicked()), this, SLOT(remove()));
	connect(ui->clearSearch, SIGNAL(clicked()), this, SLOT(clearField()));
	connect(ui->load, SIGNAL(clicked()), this, SLOT(loadImage()));
	connect(ui->generate, SIGNAL(clicked()), this, SLOT(genImage()));
	connect(ui->about, SIGNAL(clicked()), this, SLOT(aboutClick()));
	connect(ui->source, SIGNAL(clicked()), this, SLOT(sourceClick()));
	connect(ui->size, SIGNAL(valueChanged(int)), this, SLOT(storeSize(int)));
	settings->beginGroup("image");
	ui->size->setValue(settings->value("fSize", 16).toInt());
	settings->endGroup();
	reload();
	sync();
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::sort() {
	for (int i = 0; i < containers.size(); i++) {
		containers[i]->widget->sortItems();
	}
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
		delete containers[i]->l;
		delete containers[i];
	}
	containers.clear();
	langs.clear();
	settings->beginGroup("languages");
	QStringList groups = settings->childGroups();
	if (groups.size() == 0) {
		ui->grid->addItem(new QSpacerItem(0, 0,QSizePolicy::Expanding, QSizePolicy::Expanding), 0, 0, 1, 1);
	}
	for (i = 0; i < groups.size(); i++) {
		settings->beginGroup(groups[i]);
		QLabel *label = new QLabel(groups.at(i), this);
		QListWidget *qlw = new QListWidget(this);
		connect(qlw, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(findWord(QListWidgetItem*)));
		QPushButton *add = new QPushButton(tr("Add word"), this);
		QPushButton *remove = new QPushButton(tr("Remove word"), this);
		Language *lang = new Language(groups.at(i));
		QLineEdit *qle = new QLineEdit();
		QSpinBox *xSpin = new QSpinBox();
		xSpin->setMaximum(10000);
		xSpin->setMinimum(0);
		xSpin->setValue(settings->value("x", 0).toInt());
		QSpinBox *ySpin = new QSpinBox();
		ySpin->setMaximum(10000);
		ySpin->setMinimum(0);
		ySpin->setValue(settings->value("y", 0).toInt());
		QSpinBox *rotate = new QSpinBox();
		rotate->setMaximum(360);
		rotate->setMinimum(0);
		rotate->setValue(settings->value("rotate", 0).toInt());
		QSpinBox *relSize = new QSpinBox();
		relSize->setMinimum(-100);
		relSize->setMaximum(100);
		relSize->setValue(settings->value("relSize", 0).toInt());

		langs.push_back(lang);
		QLWContainer *tmp = new QLWContainer(qlw, qle, xSpin, ySpin, rotate, relSize, lang);

		connect(add, SIGNAL(clicked()), tmp, SLOT(addClick()));
		connect(qle, SIGNAL(returnPressed()), tmp, SLOT(addClick()));
		connect(remove, SIGNAL(clicked()), tmp, SLOT(removeClick()));
		connect(tmp, SIGNAL(passAddClick(QString,Language*)), this, SLOT(addWord(QString,Language*)));
		connect(tmp, SIGNAL(passRemoveClick(RefWord*)), this, SLOT(removeWord(RefWord*)));

		containers.push_back(tmp);
		ui->grid->addWidget(label, j, (i%5)*3, 1, 3);
		ui->grid->addWidget(qlw, j+1, (i%5)*3, 1, 3);
		ui->grid->addWidget(remove, j+2, (i%5)*3, 1, 3);
		ui->grid->addWidget(qle, j+3, (i%5)*3, 1, 2);
		ui->grid->addWidget(add, j+4, (i%5)*3, 1, 2);
		ui->grid->addWidget(new QLabel("Rel. Size:"), j+3, (i%5)*3+2, 1, 1);
		ui->grid->addWidget(relSize, j+4, (i%5)*3+2, 1, 1);
		ui->grid->addWidget(new QLabel("X:"), j+5, (i%5)*3, 1, 1);
		ui->grid->addWidget(new QLabel("Y:"), j+5,(i%5)*3+1, 1, 1);
		ui->grid->addWidget(new QLabel("Rotate:"), j+5,(i%5)*3+2, 1, 1);
		ui->grid->addWidget(xSpin, j+6, (i%5)*3, 1, 1);
		ui->grid->addWidget(ySpin, j+6, (i%5)*3+1, 1, 1);
		ui->grid->addWidget(rotate, j+6, (i%5)*3+2, 1, 1);

		label->show();
		if ((i+1)%5 == 0) {
			j+=7;
		}
		settings->endGroup();
	}
	settings->endGroup();
	while (0 < words.size()) {
		while (0 < words[0]->trans.size()) {
			words[0]->trans.erase(words[0]->trans.begin());
		}
		words.erase(words.begin());
	}
	QStringList tmpWords;
	int size = settings->beginReadArray("words");
	for (i = 0; i < size; i++) {
		Word *tmp = new Word();
		settings->setArrayIndex(i);
		for (j = 0; j < langs.size(); j++) {
			settings->beginGroup(langs[j]->name);
			tmpWords = settings->childKeys();
			for (k = 0; k < tmpWords.size(); k++) {
				tmp->addWord(new RefWord(settings->value(tmpWords[k], "").toString(), langs[j]));
			}
			tmp->updateColors(langs.size());
			settings->endGroup();
		}
		this->words.push_back(tmp);
	}
	settings->endArray();
	search("");
	sort();
}

void MainWindow::syncLangSettings() {
	settings->beginGroup("languages");
	for (int i = 0; i < containers.size(); i++) {
		settings->beginGroup(containers[i]->l->name);
		settings->setValue("x", containers[i]->x->value());
		settings->setValue("y", containers[i]->y->value());
		settings->setValue("rotate", containers[i]->rot->value());
		settings->setValue("relSize", containers[i]->relSize->value());
		settings->endGroup();
	}
	settings->endGroup();
}
void MainWindow::sync() {
	syncLangSettings();
	settings->remove("words");
	settings->beginWriteArray("words");
	int k = 0;
	for (int i = 0; i < words.size(); i++) {
		if (words[i]->trans.size() == 0) {
			continue;
		}
		settings->setArrayIndex(k);
		for (int j = 0; j < words[i]->trans.size(); j++) {
			settings->beginGroup(words[i]->trans[j]->l->name);
			settings->setValue(QString(j),words[i]->trans[j]->text());
			settings->endGroup();
		}
		k++;
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
	sort();
}

void MainWindow::search(QString text) {
	selected = NULL;
	if (text.length() != 0){
		ui->curStatus->setText(tr("searching for: ") + text);
	} else {
		ui->curStatus->setText(tr("showing all words."));
	}
	showWords(text);
	sort();
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
		selected->addWord(new RefWord(w, l));
		addToContainer(selected->trans[selected->trans.size()-1]);
		sync();
		selected->updateColors(langs.size());
	} else {
		Word *tmp = new Word();
		tmp->addWord(new RefWord(w, l));
		words.push_back(tmp);
		sync();
		findWord(tmp->trans[0]);
		tmp->updateColors(langs.size());
	}
}

void MainWindow::removeWord(RefWord *word) {
	bool found = false;
	for (int i = 0; i < words.size(); i++) {
		for (int j = 0; j < words[i]->trans.size(); j++) {
			if (words[i]->trans[j] == word) {
				RefWord *tmp = words[i]->trans[j];
				words[i]->trans.remove(j);
				words[i]->updateColors(langs.size());
				delete tmp;
				if ( words[i]->trans.isEmpty()) {
					Word *tmpW = words[i];
					words.remove(i);
					if (tmpW == selected) {
						selected = NULL;
					}
					delete tmpW;
				}
				found = true;
				break;
			}
			if (found) {
				break;
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

void MainWindow::loadImage() {
	QString fileName = QFileDialog::getOpenFileName(this,
							tr("Open Image"), ".", tr("Image Files (*.png *.jpg *.bmp)"));
	if (fileName.size() != 0) {
		settings->beginGroup("image");
		settings->setValue("loaded", fileName);
		settings->endGroup();
	}
}
void MainWindow::genImage() {
	syncLangSettings();
	settings->beginGroup("image");
	QString imgLoc = settings->value("loaded","").toString();
	if (imgLoc.size() != 0) {
		QWidget *imgDisplay = new QWidget();
		imgDisplay->resize(700,700);
		QPixmap *map = new QPixmap(imgLoc);
		QPainter p;
		p.begin(map);
		for (int i = 0; i < containers.size(); i++) {
			p.setFont(QFont("arial", ui->size->value() + containers[i]->relSize->value()));
			QList<QListWidgetItem *> w = containers[i]->widget->selectedItems();
			if (w.size() == 0) {
				w = containers[i]->widget->findItems("", Qt::MatchContains);
			}
			if (w.size() > 0) {
				w[0]->text();
				p.save();
				p.translate(containers[i]->x->value(),containers[i]->y->value() +
					    ui->size->value() + containers[i]->relSize->value());
				p.rotate(containers[i]->rot->value());
				p.drawText(0, 0, w[0]->text());
				p.restore();
			}
		}
		p.end();
		QLabel *display = new QLabel(imgDisplay);
		display->setPixmap(map->scaled(700,670));
		display->show();
		QPushButton *save = new QPushButton(imgDisplay);
		save->setText("Save");
		save->move(0,670);
		save->show();
		MapStore *ms = new MapStore(map);
		imgDisplay->connect(save, SIGNAL(clicked()), ms, SLOT(save()));
		imgDisplay->show();

	}
	settings->endGroup();

}
void MainWindow::aboutClick() {
	QWidget *about = new QWidget();
	QLabel *madeBy = new QLabel(about);
	madeBy->setFont(QFont("arial", 16));
	madeBy->setText("Made by Jan Anders Bremer (TZer0)");
	about->resize(350,30);
	about->show();
	madeBy->show();
}
void MainWindow::sourceClick() {
	QDesktopServices::openUrl(QUrl("https://github.com/TZer0/Translate", QUrl::TolerantMode));
}
void MainWindow::storeSize(int size) {
	settings->beginGroup("image");
	settings->setValue("fSize", size);
	settings->endGroup();
}
void MainWindow::openFileManager() {
	QDesktopServices::openUrl(QUrl("file:///" + QDir::toNativeSeparators(settings->fileName().remove("translate.ini"))));
}

Language::Language() {
	name = "none";
}

Language::Language(QString n) {
	name = n;
}
QLWContainer::QLWContainer(QListWidget *qlw, QLineEdit *qle, QSpinBox *xb, QSpinBox *yb, QSpinBox *rotate, QSpinBox *rel, Language *lang) {
	addLine = qle;
	widget = qlw;
	l = lang;
	x = xb;
	y = yb;
	rot = rotate;
	relSize = rel;
}
QLWContainer::QLWContainer() {
	addLine = NULL;
	l = NULL;
	widget = NULL;
	x = y = rot = relSize = NULL;
}
void QLWContainer::addClick() {
	emit passAddClick(addLine->text(), l);
	addLine->setText("");
}
void QLWContainer::removeClick() {
	QList<QListWidgetItem *> sel = widget->selectedItems();
	while ( 0 < sel.size()) {
		emit passRemoveClick((RefWord*)sel[0]);
		sel.removeAt(0);
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
MapStore::MapStore(QPixmap *tmp) {
	map = tmp;
}

void Word::addWord(RefWord *word) {
	trans.push_back(word);
}

void Word::updateColors(int numLangs) {
	QSet<Language*> found;
	for (int i = 0; i < trans.size(); i++) {
			found.insert(trans[i]->l);
	}
	QColor c;
	if (numLangs == found.size()) {
		c = Qt::white;
	} else {
		c = QColor(230, 230, 230);
	}
	for (int i = 0; i < trans.size(); i++) {
		trans[i]->setBackgroundColor(c);
	}
}

void MapStore::save() {
	QString filename = QFileDialog::getSaveFileName(0, tr("Save File"),
							".",
							tr("Images (*.png )"));
	if (filename.size() != 0) {
		map->save(filename, 0, -1);
	}
}
