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

CheckTestDialog::CheckTestDialog(BookModel* m,
								 const BookDescription& book, 
								 bool search,
								 QWidget* parent):
	QDialog(parent),
	model(m), do_search(search), search_book(book), operations(0)
{
	setupUi(this);
	slotRestart();
}

void CheckTestDialog::slotRestart()
{
	operations = 0;
	current_index = model->index(0, 0, model->rootIndex());
	bookLabel->setText(search_book.author_name + " " +
					   search_book.author_surname + " - " +
					   search_book.title);
	updateControls();
}

void CheckTestDialog::slotOpen()
{
	MY_ASSERT(current_index.isValid());
	current_index = model->parent(current_index);
	operations++;
	updateControls();
}

void CheckTestDialog::slotClose()
{
	MY_ASSERT(current_index.isValid());
	current_index = model->parent(current_index);
	updateControls();
}

void CheckTestDialog::slotBefore()
{
}

void CheckTestDialog::slotAfter()
{
}

void CheckTestDialog::slotFirst()
{
}

void CheckTestDialog::slotLast()
{
}
	
void CheckTestDialog::updateControls()
{
	QModelIndex parent_index = model->parent(current_index);
	BookItem* parent_item = model->indexToItem(parent_index);
	MY_ASSERT(parent_item);
	currentBoxLabel->setPixmap(model->getItemIcon(parent_item).pixmap(48, 48));
	if(!parent_index.isValid()) {
		currentBoxLabel->setText(QString::fromUtf8("Библиотека"));
		closeButton->setEnabled(false);
	} else {
		currentBoxLabel->setText(QString::fromUtf8("Ящик ") + parent_item->getLabel());
		closeButton->setEnabled(true);
	}	
	
}
