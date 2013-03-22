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

BooksWindow::BooksWindow(BookModel* m, 
						 QModelIndex index,
						 QWidget* parent):
	QMdiSubWindow(parent), model(m)
{
	setAttribute(Qt::WA_DeleteOnClose);
	booksView = new BooksView(this);
	setWidget(booksView);
	
	BookItem* item = model->indexToItem(index);
	MY_ASSERT(item);
	setWindowIcon(model->getItemIcon(item));
	if(item->isRoot()) {
		setWindowTitle(trUtf8("Библиотека"));
	} else {
		MY_ASSERT(item->isBox());
		setWindowTitle(trUtf8("Ящик: %1").arg(item->getLabel()));
	}
	root = model->indexToPath(index);
	booksView->setModel(model);
	booksView->setRootIndex(index);
	connect(booksView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(slotDoubleClicked(const QModelIndex&)));
	booksView->resize(sizeHint());
	connect(model, SIGNAL(boxInserted(const QString&)),
			this, SLOT(slotBoxInserted(const QString&)));
}

void BooksWindow::closeEvent(QCloseEvent* )
{
	if (root == BookItem::path_separator) {
		emit rootWindowClosed();
	}
}

void BooksWindow::slotDoubleClicked(const QModelIndex& index)
{
	if(!index.isValid()) return ;
	BookItem* item = model->indexToItem(index);
	MY_ASSERT(item);
	if(item->isBox()) {
		emit boxClicked(index);
	}
}

void BooksWindow::modifyRoot(const QString& newRoot)
{
	root = newRoot;
	QModelIndex index = model->pathToIndex(root);
	MY_ASSERT(index.isValid());
	booksView->setRootIndex(index);
}

void BooksWindow::modifyLabel(const QString& label)
{
	setWindowTitle(trUtf8("Ящик: %1").arg(label));
}

QModelIndex BooksWindow::selectedIndex() const
{
	return booksView->currentIndex();
}

void BooksWindow::editSelected()
{
	booksView->edit(booksView->currentIndex());
}

void BooksWindow::slotBoxInserted(const QString& path)
{
	QModelIndex index = model->pathToIndex(path);
	MY_ASSERT(index.isValid());
	if (model->indexToPath(model->parent(index)) == root) {
		booksView->setCurrentIndex(index);
	}
}
