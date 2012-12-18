/*****************************************************************************
 * 
 * ReflectDiagn main window.
 * 
 * Author: Alexey Fedoseev <aleksey@fedoseev.net>
 * 
 * This file is part of ReflectDiagn.
 *
 * ReflectDiagn is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ReflectDiagn is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ReflectDiagn. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>
#include <QDateTime>
#include <QCloseEvent>
#include <QFileDialog>
#include "diagn-window.h"
#include "book.h"
#include "myassert.h"
#include "checktestdialog.h"
#include "webaccess.h"

const char* DiagnosticsWindow::configFile = "config.ini";

DiagnosticsWindow::DiagnosticsWindow(Logger& l, QWidget* parent):
	QMainWindow(parent), config(configFile), testnumber(0), logger(l)
{
	setupUi(this);
	model = new BookModel(logger, this);
	connect(this, SIGNAL(startNewTest()), this, SLOT(slotNewTest()), Qt::QueuedConnection);
	connect(model, SIGNAL(boxMoved(const QString&, const QString&)), this, SLOT(boxMoved(const QString&, const QString&)));
	connect(model, SIGNAL(boxRenamed(const QString&, const QString&)), this, SLOT(boxRenamed(const QString&, const QString&)));
}

void DiagnosticsWindow::showMaximized()
{
	QMainWindow::showMaximized();
	if(testnumber == 0) {
		emit startNewTest();
	}
}

void DiagnosticsWindow::secondPart()
{
	if(testnumber == 1) {

		timing.edit1end = QDateTime::currentDateTime().toTime_t();
		logger.write(QString("EditDuration: %1").arg(timing.edit1end - timing.start));
		logger.write("EditDump:");
		model->dump();

		logger.write("SearchLog:");
		CheckTestDialog dialog(logger, model, config.searchBook(1), true, this);
		if(dialog.exec() != QDialog::Accepted) {
			logger.write("Search cancelled");
			if(finishTest()) return ;
		}
		timing.search1end = QDateTime::currentDateTime().toTime_t();
		logger.write(QString("SearchDuration: %1").arg(timing.search1end - timing.edit1end));
		unsigned int search1oper = dialog.operationsResult();
		logger.write(QString("SearchOperations: %1").arg(search1oper));

		logger.write("Search2Log:");
		CheckTestDialog dialog2(logger, model, config.searchBook(2), true, this);
		if(dialog2.exec() != QDialog::Accepted) {
			logger.write("Search cancelled");
			if(finishTest()) return ;
		}
		timing.search2end = QDateTime::currentDateTime().toTime_t();
		logger.write(QString("Search2Duration: %1").arg(timing.search2end - timing.search1end));
		unsigned int search2oper = dialog2.operationsResult();
		logger.write(QString("Search2Operations: %1").arg(search2oper));

		logger.write("AddLog:");
		CheckTestDialog dialog3(logger, model, config.addBook(1), false, this);
		if(dialog3.exec() != QDialog::Accepted) {
			logger.write("Addition cancelled");
			if(finishTest()) return ;
		}
		timing.add1end = QDateTime::currentDateTime().toTime_t();
		logger.write(QString("AddDuration: %1").arg(timing.add1end - timing.search2end));
		unsigned int add1oper = dialog2.operationsResult();
		logger.write(QString("AddOperations: %1").arg(add1oper));

		testnumber = 2;

		showSearchResults(search1oper, add1oper, search2oper);

		logger.write("Edit2Log:");

	} else if(testnumber == 2) {

		timing.edit2end = QDateTime::currentDateTime().toTime_t();
		logger.write(QString("Edit2Duration: %1").arg(timing.edit2end - timing.add1end));
		logger.write("Edit2Dump:");
		model->dump();

		logger.write("Search3Log:");
		CheckTestDialog dialog(logger, model, config.searchBook(3), true, this);
		if(dialog.exec() != QDialog::Accepted) {
			logger.write("Search cancelled");
			if(finishTest()) return ;
		}
		timing.search3end = QDateTime::currentDateTime().toTime_t();
		logger.write(QString("Search3Duration: %1").arg(timing.search3end - timing.edit2end));
		unsigned int search3oper = dialog.operationsResult();
		logger.write(QString("Search3Operations: %1").arg(search3oper));

		logger.write("Add2Log:");
		CheckTestDialog dialog2(logger, model, config.addBook(2), false, this);
		if(dialog2.exec() != QDialog::Accepted) {
			logger.write("Addition cancelled");
			if(finishTest()) return ;
		}
		timing.add2end = QDateTime::currentDateTime().toTime_t();
		logger.write(QString("Add2Duration: %1").arg(timing.add2end - timing.search3end));
		unsigned int add2oper = dialog2.operationsResult();
		logger.write(QString("Add2Operations: %1").arg(add2oper));

		testnumber = 3;

		showSearchResults(search3oper, add2oper);

		finishTest();
		
	} else {
		MY_ASSERT(false);
	}
}

void DiagnosticsWindow::slotNewTest()
{
/*	if(testnumber > 0 && !yesNoDialog(QString::fromUtf8("Предупреждение"),
									  QString::fromUtf8("Вы точно хотите начать новую диагностику и потерять все, что было сделано?"))) 
									  return ;*/

	NewTestDialog dialog(this);
	if(dialog.exec() != QDialog::Accepted) {
		if (testnumber == 0) QApplication::quit();
		return ;
	}

	booksArea->closeAllSubWindows();
	
	testpart = dialog.result();
	logger.write(QString("School: %1").arg(testpart.school));
	logger.write(QString("Class: %1").arg(testpart.cls));
	logger.write(QString("Name: %1").arg(testpart.name));
	logger.write(QString("Background: %1").arg(testpart.experience));

	slotHelp();

	testnumber = 1;
	timing.start = QDateTime::currentDateTime().toTime_t();	
	logger.write(QString("LocalTime: %1").arg(timing.start));
	logger.write("EditLog:");

	BooksList bl = config.books();
	if(bl.size() == 0) {
		throw QString::fromUtf8("Не удалось найти список книг.");
	}
	model->setBooks(bl);

	createWindow(model->rootIndex());
}

void DiagnosticsWindow::slotHelp()
{
	QMessageBox mb(QMessageBox::NoIcon,
				   QString::fromUtf8("Условие задачи"),
				   helpText(),
				   QMessageBox::NoButton,
				   this);
	mb.addButton(QString::fromUtf8("Хорошо"), QMessageBox::AcceptRole);
	mb.exec();	
}

void DiagnosticsWindow::slotNewBox()
{
	QMdiSubWindow* sw = booksArea->activeSubWindow();
	if(sw == NULL) return ;
	bool ok = false;
	QString name = QInputDialog::getText(this,
										 QString::fromUtf8("Новый ящик"),
										 QString::fromUtf8("Надпись на ящике"),
										 QLineEdit::Normal,
										 QString::fromUtf8(""),
										 &ok);
	if(!ok || name.isEmpty()) return ;
	QModelIndex insert_index = reinterpret_cast<BooksWindow*>(sw)->rootIndex();
	model->newBox(model->indexToPath(insert_index), name);
	QModelIndex newindex = model->index(0, 0, insert_index);
	createWindow(newindex);
}

void DiagnosticsWindow::slotDeleteBox()
{
	QMdiSubWindow* sw = booksArea->activeSubWindow();
	if(sw == NULL) return ;
	QModelIndex index = reinterpret_cast<BooksWindow*>(sw)->selectedIndex();
	if(!index.isValid()) return ;
	BookItem* item = model->indexToItem(index);
	if(!item->isBox()) return ;
	QString removePath = item->constructPath();
	model->remove(removePath);
	QList<QMdiSubWindow*> win_list = booksArea->subWindowList();
	foreach(QMdiSubWindow* sw, win_list) {
		BooksWindow* bw = reinterpret_cast<BooksWindow*>(sw);
		if(bw->rootPath().indexOf(removePath) == 0) {
			booksArea->removeSubWindow(bw);
		}
	}
}

void DiagnosticsWindow::slotRename()
{
	QMdiSubWindow* sw = booksArea->activeSubWindow();
	if(sw == NULL) return ;
	reinterpret_cast<BooksWindow*>(sw)->editSelected();
}

void DiagnosticsWindow::slotFinishEditing()
{
	if(yesNoDialog(QString::fromUtf8("Предупреждение"),
				   QString::fromUtf8("Перейти к следующей части диагностики?"))) {
		secondPart();		
	}
}

void DiagnosticsWindow::slotSaveResults()
{
	QFileDialog dialog(this);
	dialog.setLabelText(QFileDialog::LookIn, QString::fromUtf8("Выберите папку для сохранения результатов диагностики"));
	dialog.setFileMode(QFileDialog::Directory);
	dialog.setViewMode(QFileDialog::List);
	dialog.setWindowFlags(dialog.windowFlags() & (~Qt::WindowContextHelpButtonHint));
	int res = dialog.exec();
	if(!res) return ;	
	QStringList fileNames = dialog.selectedFiles();	
	MY_ASSERT(fileNames.size() == 1);
	QString directory = fileNames.at(0);
	if(QFile(logger.logPath()).copy(directory + QDir::separator() + logger.logFile())) {
		QMessageBox::information(this,
								 QString::fromUtf8("Завешение диагностики"),
								 QString::fromUtf8("Результаты диагностики успешно сохранены в папке %1.").arg(directory));
	} else {
		QMessageBox::information(this,
								 QString::fromUtf8("Завешение диагностики"),
								 QString::fromUtf8("Не удалось сохранить результаты диагностики в папке %1.").arg(directory));
	}
}

bool DiagnosticsWindow::finishTest()
{
	if(testnumber == 0) {
		QApplication::quit();
		return true;
	} else if(testnumber <= 2 &&
	   !yesNoDialog(QString::fromUtf8("Предупреждение"),
				   QString::fromUtf8("Диагностика еще не завершена. Вы уверены, что хотите закрыть приложение?"))) {
		return false;
	} else {
		QString text;
		bool save = !sendResults();
		QMessageBox::Icon icon;
		if(save) {
			text = QString::fromUtf8("Произошла ошибка отправления результатов диагностики.<br/><br/>"
									 "Пожалуйста, сохраните файл <strong>%1</strong> и отправьте его организаторам диагностики "
									 "по адресу <a href='email:diagn@eduscen.ru'>diagn@eduscen.ru</a>.").arg(logger.logPath());
			icon = QMessageBox::Warning;
		} else {
			text = QString::fromUtf8("Данные вашей диагностики успешно отправлены организаторам. Большое спасибо за участие!");
			icon = QMessageBox::Information;
		}
		QMessageBox mb(icon,
					   QString::fromUtf8("Завешение диагностики"),
					   text,
					   QMessageBox::NoButton,
					   this);
		mb.addButton(QString::fromUtf8("Конец"), QMessageBox::AcceptRole);
		if(save) {
			QPushButton* button = mb.addButton(QString::fromUtf8("Скорпировать результаты"), QMessageBox::HelpRole);
			connect(button, SIGNAL(clicked()), this, SLOT(slotSaveResults()));
		}
		mb.exec();
		QApplication::quit();
		return true;
	}
}

void DiagnosticsWindow::slotRootClosed()
{	
	if(yesNoDialog(QString::fromUtf8("Предупреждение"),
				   QString::fromUtf8("Закончить редактирование библиотеки?"))) {
		secondPart();		
	} else {
		createWindow(model->rootIndex());
	}
}

void DiagnosticsWindow::boxClicked(QModelIndex box)
{
	MY_ASSERT(box.isValid());
	QList<QMdiSubWindow*> win_list = booksArea->subWindowList();
	foreach(QMdiSubWindow* sw, win_list) {
		BooksWindow* bw = reinterpret_cast<BooksWindow*>(sw);
		if(bw->rootIndex() == box) {
			booksArea->setActiveSubWindow(bw);
			return ;
		}
	}	
	createWindow(box);
}

void DiagnosticsWindow::boxMoved(const QString& from, const QString& to)
{
	QList<QMdiSubWindow*> win_list = booksArea->subWindowList();
	foreach(QMdiSubWindow* sw, win_list) {
		BooksWindow* bw = reinterpret_cast<BooksWindow*>(sw);
		QString root = bw->rootPath();
		if (root.indexOf(from) == 0) {
			if(root == from) {
				bw->modifyRoot(to);
			} else {
				QString newRoot = to + BookItem::path_separator + root.mid(from.length() + BookItem::path_separator.length());
				bw->modifyRoot(newRoot);
			}
		}
	}
}

void DiagnosticsWindow::boxRenamed(const QString& path, const QString& label)
{
	QList<QMdiSubWindow*> win_list = booksArea->subWindowList();
	foreach(QMdiSubWindow* sw, win_list) {
		BooksWindow* bw = reinterpret_cast<BooksWindow*>(sw);
		QString root = bw->rootPath();
		if (root == path) {
			bw->modifyLabel(label);
			return ;
		}
	}
}

void DiagnosticsWindow::showSearchResults(unsigned int seares, unsigned int addres, unsigned int seares2)
{
	QString text, s_postfix, a_postfix;
	if(addres != 11 && addres % 10 == 1) {
		a_postfix = QString::fromUtf8("ю");
	} else if((addres < 10 || addres > 20) && ((addres % 10) > 1) && (addres % 10) < 5) {
	a_postfix = QString::fromUtf8("и");
	} else {
		a_postfix = QString::fromUtf8("й");
	}
	if(seares != 11 && seares % 10 == 1) {
		s_postfix = QString::fromUtf8("ю");
	} else if((seares < 10 || seares > 20) && (seares % 10 > 1) && (seares % 10) < 5) {
		s_postfix = QString::fromUtf8("и");
	} else {
		s_postfix = QString::fromUtf8("й");
	}
	if(seares2 != 0) {
		text = QString::fromUtf8("Вы смогли отыскать книги за <strong>%1</strong> и <strong>%2</strong> операций,"
								 " а добавить за <strong>%3</strong> операци%4.<br/><br/>").arg(seares).arg(seares2).arg(addres).arg(a_postfix);
	} else {
		text = QString::fromUtf8("Вы смогли отыскать книгу за <strong>%1</strong> операци%2,"
								 " а добавить за <strong>%3</strong> операци%4.<br/><br/>").arg(seares).arg(s_postfix).arg(addres).arg(a_postfix);
	}
	text += helpText();

	QMessageBox mb(QMessageBox::Information,
				   QString::fromUtf8("Результат поиска и добавления книг"),
				   text,
				   QMessageBox::NoButton,
				   this);
	mb.addButton(QString::fromUtf8("Продолжить"), QMessageBox::AcceptRole);	
	mb.exec();
}

void DiagnosticsWindow::closeEvent(QCloseEvent* event)
{
	if(testnumber > 0) {
		if(!finishTest()) {
			event->ignore();
			return ;
		}
	}
	event->accept();
}

void DiagnosticsWindow::createWindow(QModelIndex root)
{
	BooksWindow* window = new BooksWindow(model, root, this);
	booksArea->addSubWindow(window);
	window->show();
	if(root == model->rootIndex()) {
		connect(window, SIGNAL(rootWindowClosed()), this, SLOT(slotRootClosed()));
		window->resize(QSize(booksArea->width() / 2, booksArea->height()));
	}
	connect(window, SIGNAL(boxClicked(QModelIndex)), this, SLOT(boxClicked(QModelIndex)));
}

bool DiagnosticsWindow::yesNoDialog(const QString& title, const QString& text)
{
	QMessageBox mb(QMessageBox::Question,
				   title, text, 
				   QMessageBox::NoButton,
				   this);
	QPushButton* yesButton = mb.addButton(QString::fromUtf8("Да"), QMessageBox::YesRole);
	mb.addButton(QString::fromUtf8("Нет"), QMessageBox::NoRole);
	mb.exec();
	return mb.clickedButton() == (QAbstractButton*)yesButton;
}

QString DiagnosticsWindow::helpText()
{
	QString text;
	if(testnumber <= 1) {
		text = QString::fromUtf8("Необходимо расположить %1 книг по ящикам так,"
								 " чтобы искомую книгу можно было найти быстрее всего,"
								 " а добавление еще одной книги не было бы слишком долгим.\n\n"
								 "Программа позволяет располагать книги в ящиках. Число"
								 " ящиков не ограниченно.\n\n"
								 "После того, как вы как-то разложите книги, вы сможете проверить,"
								 " сколько операций вам потребуется на поиск или добавление новой книги.").arg(config.booksNumber());
	} else if(testnumber == 2) {
		int t_op = config.targetOperations();
		text = QString::fromUtf8("Известно, что можно расположить книги в ящиках таким образом, что поиск любой книги будет "
								 "занимать не более <strong>%1</strong> операций, а добавление - не более <strong>%2</strong> операций.<br/><br/>").arg(t_op).arg(t_op + 1);
		text += QString::fromUtf8("Предлагаем вам усовершенствовать принцип расположения книг в ящиках так, чтобы улучшить "
								  "полученные результаты поиска и добавления книг.");
	} else if(testnumber == 3) {
		text = QString::fromUtf8("Диагностика завершена. Нажмите кнопку, чтобы оформить результаты диагностики.");
	}
	return text;
}

bool DiagnosticsWindow::sendResults()
{
	WebAccess wa;
	return wa.post(config.postAddress(), logger);	
}
