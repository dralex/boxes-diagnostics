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
#include <QFile>
#include "diagn-window.h"
#include "book.h"
#include "myassert.h"
#include "checktestdialog.h"

const char* DiagnosticsWindow::booksFile = "books.txt";

DiagnosticsWindow::DiagnosticsWindow(QWidget* parent):
	QMainWindow(parent), firsttest(true)
{
	setupUi(this);
	model = new BookModel(this);
	connect(this, SIGNAL(startNewTest()), this, SLOT(slotNewTest()), Qt::QueuedConnection);
	connect(model, SIGNAL(boxMoved(const QString&, const QString&)), this, SLOT(boxMoved(const QString&, const QString&)));
	connect(model, SIGNAL(boxRenamed(const QString&, const QString&)), this, SLOT(boxRenamed(const QString&, const QString&)));
}

void DiagnosticsWindow::showMaximized()
{
	QMainWindow::showMaximized();
	if(firsttest) {
		emit startNewTest();
	}
}

void DiagnosticsWindow::secondPart()
{
	BookDescription book;
	book.author_name = QString::fromUtf8("Марк");
	book.author_surname = QString::fromUtf8("Твен");
	book.title = QString::fromUtf8("Приключения Гекльберри Финна");
	CheckTestDialog dialog(model, book, true, this);
	dialog.exec();

	book.author_name = QString::fromUtf8("Marx");
	book.author_surname = QString::fromUtf8("Carl");
	book.title = QString::fromUtf8("Capital");
	CheckTestDialog dialog2(model, book, false, this);
	dialog2.exec();
}

void DiagnosticsWindow::slotNewTest()
{
	if(!firsttest && !yesNoDialog(QString::fromUtf8("Предупреждение"),
								  QString::fromUtf8("Вы точно хотите начать новую диагностику и потерять все, что было сделано?"))) 
		return ;

	NewTestDialog dialog(this);
/*	if(dialog.exec() != QDialog::Accepted) {
		if (firsttest) QApplication::quit();
		return ;
		}*/

	booksArea->closeAllSubWindows();
	
	testpart = dialog.result();
	firsttest = false;

//	slotHelp();

	BooksList bl = loadBooks();
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
				   QString::fromUtf8("Текст условия задачи"),
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
				   QString::fromUtf8("Перейти ко второй части диагностики?"))) {
		secondPart();		
	}
}

void DiagnosticsWindow::slotRootClosed()
{	
	if(yesNoDialog(QString::fromUtf8("Предупреждение"),
				   QString::fromUtf8("Закончить первую часть диагностики?"))) {
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

BooksList DiagnosticsWindow::loadBooks()
{
	BooksList bl;
	QFile file(booksFile);
	if(file.open(QFile::ReadOnly)) {
		while (!file.atEnd()) {
			QString line = QString::fromUtf8(file.readLine()).trimmed();
			if(line.isEmpty()) continue;
			QStringList sl = line.split(":");
			if(sl.size() != 3) continue;
			BookDescription bd;
			bd.author_name = sl.at(0);
			bd.author_surname = sl.at(1);
			bd.title = sl.at(2);
			bl.append(bd);
			if(bl.size() == 50) break;
		}
		file.close();
	}
	return bl;
}
