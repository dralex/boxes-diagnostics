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

void NewCheckDialog::slotOK()
{
	int diff = 0;
	QString title, message, path;
	QModelIndex start_index = model->index(0, 0, model->rootIndex());
	QList<QPair<QString,int> > path_list = calculatePath(start_index, current_index);
	for(int i = 0; i < path_list.size(); i++) {
		QPair<QString,int> pair = path_list.at(i);
		diff += pair.second;
		path += pair.first;
	}

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
		title = trUtf8("Результат добавления");
		message = trUtf8("Книга была добавлена в следующее место:<br/>"
						 "%3<br/>Итого: %1 %2.").arg(operations).arg(operationsNoun(operations, true)).arg(path);
	} else {
		logger.write("Found");	
		title = trUtf8("Результат поиска");
		message = trUtf8("Книга была найдена следующим путем:<br/>"
						 "%3<br/>Итого: %1 %2.").arg(operations).arg(operationsNoun(operations, true)).arg(path);
	}
	QMessageBox mb(QMessageBox::Information,
				   title,
				   message,
				   QMessageBox::NoButton,
				   this);
	mb.setTextFormat(Qt::RichText);
	mb.addButton(trUtf8("Продолжить"), QMessageBox::AcceptRole);	
	mb.exec();
	accept();
}

void NewCheckDialog::slotOK2()
{
	int diff = 0;
	QString title, message, path;
	QModelIndex start_index = model->index(0, 0, model->rootIndex());
	QList<QPair<QString,int> > path_list = calculatePath(start_index, current_index);
	for(int i = 0; i < path_list.size(); i++) {
		QPair<QString,int> pair = path_list.at(i);
		diff += pair.second;
		path += pair.first;
	}

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
	title = trUtf8("Результат добавления");
	message = trUtf8("Книга была добавлена в следующее место:<br/>"
					 "%3<br/>Итого: %1 %2.").arg(operations).arg(operationsNoun(operations, true)).arg(path);
	QMessageBox mb(QMessageBox::Information,
				   title,
				   message,
				   QMessageBox::NoButton,
				   this);
	mb.setTextFormat(Qt::RichText);
	mb.addButton(trUtf8("Продолжить"), QMessageBox::AcceptRole);	
	mb.exec();
	accept();
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
		logger.write("Current nothing selected");
	}

	int diff = calculateDistance(model->index(0, 0, model->rootIndex()), current_index);
	if(do_search) {
		bool book_found = !current_item->isBox() && search_books.contains(current_item->getBookDescr());
		if(book_found) {
			addLabel->setText(trUtf8("<span style=\"font-size:14pt; font-weight:600;\">"
									 "Книга найдена за %1 %2!</span>")
							  .arg(diff)
							  .arg(operationsNoun(diff, true)));
		}
		addLabel->setVisible(book_found);
		okButton->setEnabled(book_found);
	} else {
		addLabel->setText(trUtf8("<span style=\"font-size:14pt; font-weight:600;\">"
								 "Добавить книгу (потребуется %1 %2):</span>")
						  .arg(diff + 1)
						  .arg(operationsNoun(diff + 1, true)));
	}
}

void NewCheckDialog::incrementOperations(int diff)
{
	operations += diff;
	unsigned int time_diff = QDateTime::currentDateTime().toTime_t() - start_time;
	logger.write(QString("O %1 %2").arg(operations).arg(time_diff));
}

QList<QPair<QString,int> > NewCheckDialog::calculatePath(QModelIndex from, QModelIndex to) const
{
	QList<QPair<QString,int> > res;
	if(!from.isValid() || !to.isValid()) {
		return res;
	}

	BookItem* from_item = model->indexToItem(from);
	MY_ASSERT(from_item);
	BookItem* to_item = model->indexToItem(to);
	MY_ASSERT(to_item);

	QList<const BookItem*> path = from_item->pathTo(to_item);
	MY_ASSERT(path.size() >= 0);

	const BookItem* prev = NULL;
	for(int i = 0; i < path.size(); i++) {
		const BookItem* b = path.at(i);
		QString label;
		if(b->isBox()) {
			label = trUtf8("Ящик ") + b->getLabel();
		} else {
			label = b->getLabel();
		}
		QString str;
		int diff = 0;
		if(prev != NULL) {
			if(prev->parent() == b->parent()) {
				diff = abs(b->row() - prev->row());
				str = trUtf8("+%3 %4 за переход<br/><strong>%1) %2</strong><br/>")
					.arg(i + 1)
					.arg(label)
					.arg(diff)
					.arg(operationsNoun(diff, false));
			} else {
				diff = 2 + b->row();
				if(b->row() == 0) {
					str = trUtf8("+1 операция за открытие и +1 за переход<br/><strong>%1) %2</strong><br/>")
						.arg(i + 1)
						.arg(label);
				} else {
					str = trUtf8("+1 операция за открытие и +%3 за переход<br/><strong>%1) %2</strong><br/>")
						.arg(i + 1)
						.arg(label)
						.arg(b->row() + 1);
				}
			}
		} else {
			str = trUtf8("начинаем с<br/><strong>%1) %2</strong><br/>")
				.arg(i + 1)
				.arg(label);
		}
		res.append(QPair<QString,int>(str, diff));
		prev = b;
	}
	return res;
}

int NewCheckDialog::calculateDistance(QModelIndex from, QModelIndex to) const
{
	int diff = 0;
	QList<QPair<QString,int> > path = calculatePath(from, to);
	for(int i = 0; i < path.size(); i++) {
		diff += path.at(i).second;
	}
	return diff;
}

QString NewCheckDialog::operationsNoun(int o, bool rod) const
{
	if ((o % 10 == 1) && o != 11) {
		if (rod) {
			return trUtf8("операция");
		} else {
			return trUtf8("операцию");
		}
	} else if (((o % 10 == 2) || (o % 10 == 3) || (o % 10 == 4)) && (o < 10 || o > 20)) {
		return trUtf8("операции");
	} else {
		return trUtf8("операций");
	}
}
