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

#include "checktestdialog.h"
#include "myassert.h"

CheckTestDialog::CheckTestDialog(BookModel* m,
								 const BookDescription& book, 
								 bool search,
								 QWidget* parent):
	QDialog(parent),
	model(m), do_search(search), search_book(book), operations(0)
{
	setupUi(this);
	if(search) {
		operationLabel->setText(QString::fromUtf8("Найдите следующую книгу, затратив меньшее число операций:"));
	} else {
		operationLabel->setText(QString::fromUtf8("Добавьте в библиотеку следующую книгу, затратив меньшее число операций:"));
	}
	addBoxButton->setVisible(!search);
	bookLabel->setText(book.author_name + " " + book.author_surname + " <strong>" + book.title + "</strong>");
	bookView->setModel(m);
	slotRestart();
	resize(sizeHint());
}

void CheckTestDialog::slotRestart()
{
	operations = 0;
	parent_index = model->rootIndex();
	current_index = model->index(0, 0, parent_index);
	bookLabel->setText(search_book.author_name + " " +
					   search_book.author_surname + " - " +
					   search_book.title);
	updateControls();
}

void CheckTestDialog::slotOpen()
{
	MY_ASSERT(current_index.isValid());
	parent_index = current_index;
	current_index = model->index(0, 0, current_index);
	operations++;
	updateControls();
}

void CheckTestDialog::slotClose()
{
	MY_ASSERT(parent_index.isValid());
	current_index = parent_index;
	parent_index = model->parent(parent_index);
	updateControls();
}

void CheckTestDialog::slotBefore()
{
	int row = current_index.row();
	current_index = model->index(row - 1, 0, parent_index);
	operations++;
	updateControls();
}

void CheckTestDialog::slotAfter()
{
	int row = current_index.row();
	current_index = model->index(row + 1, 0, parent_index);
	operations++;
	updateControls();
}

void CheckTestDialog::slotFirst()
{
	current_index = model->index(0, 0, parent_index);
	updateControls();
}

void CheckTestDialog::slotLast()
{
	current_index = model->index(model->rowCount(parent_index) - 1, 0, parent_index);
	updateControls();
}

void CheckTestDialog::slotAddBox()
{
	// вызвать диалог по вводу имени и выбору - до или после вставить
	operations++;
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
		currentIcon->setPixmap(model->getItemIcon(current_item).pixmap(48, 48));
		const BookDescription& bd = current_item->getBookDescr();
		currentLabel->setText(bd.author_name + " " + bd.author_surname + " <strong>" + bd.title + "</strong>");
	} else {
		currentIcon->setPixmap(QPixmap());
		currentLabel->setText(QString::fromUtf8("Ящик пуст"));
	}

	operationsCountLabel->setText(QString("<strong>%1</strong>").arg(operations));

	closeButton->setEnabled(parent_index.isValid());
	openButton->setEnabled(current_index.isValid() && current_item->isBox());
	firstButton->setEnabled(current_index.isValid() && current_index.row() > 0);
	lastButton->setEnabled(current_index.isValid() && current_index.row() < model->rowCount(parent_index) - 1);
	beforeButton->setEnabled(current_index.isValid() && current_index.row() > 0);
	afterButton->setEnabled(current_index.isValid() && current_index.row() < model->rowCount(parent_index) - 1);

	okButton->setEnabled(!current_item->isBox() && search_book == current_item->getBookDescr());
}
