/*****************************************************************************
 * 
 * Checking dialog window for the Books diagnostics (new version).
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
#include <QMessageBox>
#include "newcheckdialog.h"
#include "myassert.h"
#include "addboxdialog.h"

NewCheckDialog::NewCheckDialog(Logger& l,
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
			operationLabel->setText(trUtf8("Найдите следующую книгу, затратив меньшее число операций:"));
		}
		addLabel->setVisible(false);
		ok2Button->setVisible(false);
		addBoxButton->setVisible(false);
	} else {
		addLabel->setText(trUtf8("<span style=\"font-size:14pt; font-weight:600;\">Добавить книгу:</span>"));
		typeLabel->setText(trUtf8("<span style=\"font-size:20pt; font-weight:600;\">Добавление новой книги</span>"));
		operationLabel->setText(trUtf8("Добавьте в библиотеку следующую книгу, затратив меньшее число операций:"));
		okButton->setText(trUtf8("до выбранной"));
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
}

void NewCheckDialog::slotOK()
{
	int diff = calculateDistance(model->index(0, 0, model->rootIndex()), current_index);
	incrementOperations(diff);	
	if(!do_search) {
		QModelIndex index;
		int row;
		BookItem* current_item = model->indexToItem(current_index);
		MY_ASSERT(current_item);
		if(current_item->isBox()) {
			row = 0;
			index = current_index;
		} else {
			index = current_index.parent();
			row = current_index.row();
		}		
		model->newBook(model->indexToPath(index), row, search_books.at(0));
		logger.write("Inserted before");
		incrementOperations();
	} else {
		logger.write("Found");
	}
	accept();
}

void NewCheckDialog::slotOK2()
{
	int diff = calculateDistance(model->index(0, 0, model->rootIndex()), current_index);
	incrementOperations(diff);	
	MY_ASSERT(!do_search);
	QModelIndex index;
	int row;
	BookItem* current_item = model->indexToItem(current_index);
	MY_ASSERT(current_item);
	if(current_item->isBox()) {
		row = 0;
		index = current_index;
	} else {
		index = current_index.parent();
		row = current_index.row() + 1;
	}		
	model->newBook(model->indexToPath(index), row, search_books.at(0));
	logger.write("Inserted after");
	incrementOperations();
	accept();
}

void NewCheckDialog::slotRestart()
{
	operations = 0;
	current_index = model->index(0, 0, model->rootIndex());
	if(do_search) {
		logger.write("Start search") ;
	} else {
		logger.write("Start add");
	}
	logger.write("O 0");
	start_time = QDateTime::currentDateTime().toTime_t();
	updateControls();
}


void NewCheckDialog::slotAddBox()
{
	AddBoxDialog dialog(this);
	if(dialog.exec() == QDialog::Accepted) {
		if(!current_index.isValid()) {
			model->newBox(model->indexToPath(model->rootIndex()), dialog.boxName());	
			current_index = model->index(0, 0, model->rootIndex());
		} else {
			int target_row = -1;
			if(dialog.addBefore()) {
				target_row = current_index.row();
			} else {
				target_row = current_index.row() + 1;				
			}
			model->newBox(model->indexToPath(current_index.parent()), dialog.boxName(), target_row);
			current_index = model->index(target_row, 0, current_index.parent());
		}
		logger.write("Add box");
		incrementOperations();
		updateControls();
	}
}

void NewCheckDialog::slotBookClicked(QModelIndex index)
{
	if(!index.isValid() || current_index == index) {
		return ;
	}
	BookItem* current_item = model->indexToItem(index);
	MY_ASSERT(current_item);
	current_index = index;
	updateControls();
}

void NewCheckDialog::slotBoxExpanded(QModelIndex index)
{
	if(!index.isValid()) {
		return ;
	}
	BookItem* current_item = model->indexToItem(index);
	MY_ASSERT(current_item);
	logger.write(QString("Expanded %1{%2}").arg(current_item->getID()).arg(current_item->getLabel()));
}

void NewCheckDialog::slotBoxCollapsed(QModelIndex index)
{
	if(!index.isValid()) {
		return ;
	}
	BookItem* current_item = model->indexToItem(current_index);
	MY_ASSERT(current_item);
	logger.write(QString("Collapsed %1{%2}").arg(current_item->getID()).arg(current_item->getLabel()));
}
	
void NewCheckDialog::updateControls()
{
	bookView->setCurrentIndex(current_index);

	BookItem* current_item = model->indexToItem(current_index);
	MY_ASSERT(current_item);
	if(current_index.isValid()) {
		logger.write(QString("Current %1{%2}").arg(current_item->getID()).arg(current_item->getLabel()));
	} else {
		logger.write("Current empty box");
	}

	int diff = calculateDistance(model->index(0, 0, model->rootIndex()), current_index);
	if(do_search) {
		bool book_found = !current_item->isBox() && search_books.contains(current_item->getBookDescr());
		if(book_found) {
			addLabel->setText(trUtf8("<span style=\"font-size:14pt; font-weight:600;\">"
									 "Книга найдена за %1 опер.!</span>").arg(diff));
		}
		addLabel->setVisible(book_found);
		okButton->setEnabled(book_found);
	} else {
		addLabel->setText(trUtf8("<span style=\"font-size:14pt; font-weight:600;\">"
								 "Добавить книгу (потребуется %1 опер.):</span>").arg(diff + 1));
	}
}

void NewCheckDialog::incrementOperations(int diff)
{
	operations += diff;
	unsigned int time_diff = QDateTime::currentDateTime().toTime_t() - start_time;
	logger.write(QString("O %1 %2").arg(operations).arg(time_diff));
}

int  NewCheckDialog::calculateDistance(QModelIndex from, QModelIndex to)
{
	if(!from.isValid() || !to.isValid() || from == to) {
		return 0;
	}

	BookItem* from_item = model->indexToItem(from);
	MY_ASSERT(from_item);
	BookItem* to_item = model->indexToItem(to);
	MY_ASSERT(to_item);

	QList<const BookItem*> path = from_item->pathTo(to_item);
	MY_ASSERT(path.size() >= 0);

	int diff = 0;
	const BookItem* prev = NULL;
	for(int i = 0; i < path.size(); i++) {
		const BookItem* b = path.at(i);
		if(prev != NULL) {
			if(prev->parent() == b->parent()) {
				diff += abs(b->row() - prev->row());
			} else {
				diff += 2 + b->row();
			}
		}
		prev = b;
	}
	return diff;
}
