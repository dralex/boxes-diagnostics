/*****************************************************************************
 * 
 * ReflectDiagn scroll area for the Books diagnostics.
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

#include "bookswindow.h"
#include "myassert.h"

BooksWindow::BooksWindow(BookModel* model, 
						 QModelIndex index,
						 QWidget* parent):
	QMdiSubWindow(parent)
{
	booksView = new QListView(this);
	booksView->setViewMode(QListView::IconMode);
	booksView->setWrapping(true);
	booksView->setMovement(QListView::Free);

	setWidget(booksView);
	
	BookItem* item = model->indexToItem(index);
	MY_ASSERT(item);
	setWindowIcon(model->getItemIcon(item));
	if(item->isRoot()) {
		setWindowTitle(QString::fromUtf8("Библиотека"));
	} else {
		MY_ASSERT(item->isBox());
		setWindowTitle(QString::fromUtf8("Коробка: ") + item->getLabel());
	}

	booksView->setModel(model);
	booksView->setRootIndex(index);
}
