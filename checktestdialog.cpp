/*****************************************************************************
 * 
 * Checking dialog window for the Books diagnostics.
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

#include <QDateTime>
#include "checktestdialog.h"
#include "myassert.h"
#include "addboxdialog.h"

CheckTestDialog::CheckTestDialog(Logger& l,
								 BookModel* m,
								 const QString& question,
								 const BooksList& books, 
								 bool search,
								 QWidget* parent):
	QDialog(parent),
	logger(l), model(m), do_search(search), search_books(books), operations(0), start_time(0)
{
	setupUi(this);
	MY_ASSERT((search && books.size() > 0) || books.size() == 1);
	if(search) {
		if (question.isEmpty()) {
			operationLabel->setText(QString::fromUtf8("Найдите следующую книгу, затратив меньшее число операций:"));
		}
		addLabel->setVisible(false);
		ok2Button->setVisible(false);
		addBoxButton->setVisible(false);
	} else {
		addLabel->setText(QString::fromUtf8("<span style=\"font-size:14pt; font-weight:600;\">Добавить книгу:</span>"));
		typeLabel->setText(QString::fromUtf8("<span style=\"font-size:20pt; font-weight:600;\">Добавление новой книги</span>"));
		actionsLabel->setText(QString::fromUtf8("<span style=\"font-size:14pt; font-weight:600;\">"
												"Для поиска подходящего места под новую книгу используйте следующие действия:</span>"));
		operationLabel->setText(QString::fromUtf8("Добавьте в библиотеку следующую книгу, затратив меньшее число операций:"));
		okButton->setText(QString::fromUtf8("до выбранной"));
	}	
	if (question.isEmpty()) {
		QStringList bl;
		foreach(const BookDescription& book, books) {
			bl.append(book.author_name + " " + book.author_surname + " <strong>" + book.title + "</strong>");			
		}
		bookLabel->setText(bl.join("<br/>"));
	} else {
		bookLabel->setText(question);
	}
	bookView->setModel(m);
	slotRestart();
	resize(sizeHint());
}

void CheckTestDialog::slotOK()
{
	if(!do_search) {
		int row;
		if(current_index.isValid()) {
			row = current_index.row();
		} else {
			row = 0;
		}		
		model->newBook(model->indexToPath(parent_index), row, search_books.at(0));
		logger.write("Inserted before");
		incrementOperations();
	} else {
		logger.write("Found");
	}
	accept();
}

void CheckTestDialog::slotOK2()
{
	MY_ASSERT(!do_search);
	int row;
	if(current_index.isValid()) {
		row = current_index.row() + 1;
	} else {
		row = 0;
	}
	model->newBook(model->indexToPath(parent_index), row, search_books.at(0));
	logger.write("Inserted after");
	incrementOperations();
	accept();
}

void CheckTestDialog::slotRestart()
{
	operations = 0;
	parent_index = model->rootIndex();
	current_index = model->index(0, 0, parent_index);
	if(do_search) {
		logger.write("Start search") ;
	} else {
		logger.write("Start add");
	}
	logger.write("O 0");
	start_time = QDateTime::currentDateTime().toTime_t();
	updateControls();
}

void CheckTestDialog::slotOpen()
{
	MY_ASSERT(current_index.isValid());
	parent_index = current_index;
	current_index = model->index(0, 0, current_index);
	logger.write("Open");
	incrementOperations();
	if(current_index.isValid()) incrementOperations();
	updateControls();
}

void CheckTestDialog::slotClose()
{
	MY_ASSERT(parent_index.isValid());
	current_index = parent_index;
	parent_index = model->parent(parent_index);
	logger.write("Close");
	updateControls();
}

void CheckTestDialog::slotBefore()
{
	int row = current_index.row();
	current_index = model->index(row - 1, 0, parent_index);
	logger.write("<");
	incrementOperations();
	updateControls();
}

void CheckTestDialog::slotAfter()
{
	int row = current_index.row();
	current_index = model->index(row + 1, 0, parent_index);
	logger.write(">");
	incrementOperations();
	updateControls();
}

void CheckTestDialog::slotFirst()
{
	current_index = model->index(0, 0, parent_index);
	logger.write("|<");
	updateControls();
}

void CheckTestDialog::slotLast()
{
	current_index = model->index(model->rowCount(parent_index) - 1, 0, parent_index);
	logger.write(">|");
	updateControls();
}

void CheckTestDialog::slotAddBox()
{
	AddBoxDialog dialog(this);
	if(dialog.exec() == QDialog::Accepted) {
		if(!current_index.isValid()) {
			model->newBox(model->indexToPath(parent_index), dialog.boxName());	
			current_index = model->index(0, 0, parent_index);
		} else {
			int target_row = -1;
			if(dialog.addBefore()) {
				target_row = current_index.row();
			} else {
				target_row = current_index.row() + 1;				
			}
			model->newBox(model->indexToPath(parent_index), dialog.boxName(), target_row);
			current_index = model->index(target_row, 0, parent_index);
		}
		logger.write("Add box");
		incrementOperations();
		updateControls();
	}
}

void CheckTestDialog::slotBookClicked(QModelIndex index)
{
	if(!index.isValid() || current_index == index) {
		return ;
	}
	BookItem* parent_item = model->indexToItem(parent_index);
	MY_ASSERT(parent_item);
	BookItem* current_item = model->indexToItem(current_index);
	MY_ASSERT(current_item);
	int current_row = current_item->row();
	BookItem* new_item = model->indexToItem(index);
	MY_ASSERT(new_item);
	int new_row = new_item->row();
	int diff = new_row - current_row;
	if (diff < 0) diff = -diff;
	logger.write(QString("Clicked %1{%2}").arg(new_item->getID()).arg(new_item->getLabel()));
	if (diff < parent_item->childCount() - 1) {
		incrementOperations(diff);
	}
	current_index = index;
	updateControls();
}
	
void CheckTestDialog::updateControls()
{
	BookItem* parent_item = model->indexToItem(parent_index);
	MY_ASSERT(parent_item);
	currentBoxIcon->setPixmap(model->getItemIcon(parent_item).pixmap(48, 48));
	if(!parent_index.isValid()) {
		currentBoxLabel->setText(QString::fromUtf8("библиотекой"));
		closeButton->setEnabled(false);
	} else {
		currentBoxLabel->setText(parent_item->getLabel());
		closeButton->setEnabled(true);
	}	

	bookView->setRootIndex(parent_index);
	bookView->setCurrentIndex(current_index);

	BookItem* current_item = model->indexToItem(current_index);
	MY_ASSERT(current_item);
	if(current_index.isValid()) {
		logger.write(QString("Current %1{%2}").arg(current_item->getID()).arg(current_item->getLabel()));
		currentIcon->setPixmap(model->getItemIcon(current_item).pixmap(48, 48));
		currentLabel->setText(current_item->getLabel());
	} else {
		logger.write("Current empty box");
		currentIcon->setPixmap(QPixmap());
		currentLabel->setText(QString::fromUtf8("Ящик пуст"));
	}
	operationsCountLabel->setText(QString("<span style=\"font-size:20pt; font-weight:600;\">%1</span>").arg(operations));

	closeButton->setEnabled(parent_index.isValid());
	openButton->setEnabled(current_index.isValid() && current_item->isBox());
	firstButton->setEnabled(current_index.isValid() && current_index.row() > 0);
	lastButton->setEnabled(current_index.isValid() && current_index.row() < model->rowCount(parent_index) - 1);
	beforeButton->setEnabled(current_index.isValid() && current_index.row() > 0);
	afterButton->setEnabled(current_index.isValid() && current_index.row() < model->rowCount(parent_index) - 1);

	if(do_search) {
		bool book_found = !current_item->isBox() && search_books.contains(current_item->getBookDescr());
		addLabel->setVisible(book_found);
		okButton->setEnabled(book_found);
	}
}

void CheckTestDialog::incrementOperations(int diff)
{
	operations += diff;
	unsigned int time_diff = QDateTime::currentDateTime().toTime_t() - start_time;
	logger.write(QString("O %1 %2").arg(operations).arg(time_diff));
}
