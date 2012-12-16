/*****************************************************************************
 * 
 * ReflectDiagn book window for the Books diagnostics.
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

#ifndef BOOKS_WINDOW
#define BOOKS_WINDOW

#include <QMdiSubWindow>
#include <QGridLayout>
#include <QListView>
#include <QDrag>
#include <QCursor>

#include "bookitem.h"
#include "bookmodel.h"

class BooksView: public QListView {
Q_OBJECT
public:
	BooksView(QWidget* parent): QListView(parent) {
		setViewMode(QListView::ListMode);
		setFlow(QListView::TopToBottom);
		setResizeMode(QListView::Adjust);
		setWrapping(true);
		setDragEnabled(true);
		setAcceptDrops(true);
		setDropIndicatorShown(true);
		setDragDropMode(QAbstractItemView::DragDrop);
		setDefaultDropAction(Qt::MoveAction);
		setEditTriggers(QAbstractItemView::SelectedClicked);
//	setSelectionMode(QAbstractItemView::MultiSelection);
//	setMovement(QListView::Free);		
	}
	
protected:

	void startDrag(Qt::DropActions) {
		QDrag* drag = new QDrag(this);
		QModelIndexList indexes;
		QModelIndex current = currentIndex();
		indexes.append(current);
		BookModel* m = reinterpret_cast<BookModel*>(model());
		drag->setMimeData(m->mimeData(indexes));
		BookItem* item = m->indexToItem(current);
		drag->setPixmap(m->getItemIcon(item).pixmap(32, 32));
		drag->setDragCursor(QCursor(Qt::ClosedHandCursor).pixmap(), Qt::MoveAction);
		drag->exec(Qt::MoveAction);
	}
};

class BooksWindow: public QMdiSubWindow {
Q_OBJECT
public:
	BooksWindow(BookModel* model, 
				QModelIndex index,
				QWidget* parent = 0);

	QModelIndex selectedIndex() const;
	QModelIndex rootIndex() const { return booksView->rootIndex(); }
	QString rootPath() const { return root; }
	void modifyRoot(const QString& newRoot);
	void modifyLabel(const QString& label);
	void editSelected();

signals:
	void rootWindowClosed();
	void boxClicked(QModelIndex box);

protected slots:
	void slotDoubleClicked(const QModelIndex& index);

protected:
	void closeEvent(QCloseEvent *event);

private:
	BookModel* model;
	QListView* booksView;
	QString root;
};

#endif
