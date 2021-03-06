/*****************************************************************************
 * 
 * Book/box model item for the Books diagnostics.
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

#ifndef BOOK_ITEM_HEADER
#define BOOK_ITEM_HEADER

#include <QStringList>
#include <QList>
#include "book.h"

class BookItem {
public:
	BookItem();
	BookItem(BookItem* parent, const QString& boxLabel);
	BookItem(BookItem* parent, const BookDescription& bd, const QString& uid = QString(""));
	~BookItem();

	bool isRoot() const { return parent_item == NULL; }
	bool isBox() const { return box; }
	int childCount() const { return children.size(); }
	int subboxesCount() const;
	BookItem* child(int index);
	BookItem* parent() const { return parent_item; }
	QString constructPath() const;
	int row() const;
	int findChildID(const QString& id) const;

	void setBooks(const BooksList& list);
	void appendBooks(const QMap<QString, BookDescription>& books);
	BookItem* makeSubBox(const QString& label, int row);
	void rename(const QString& n) { label = n; }
	void removeChild(int row);
	void addChild(BookItem* child);
	void moveChild(int old_index, int new_index);
	void insertChild(int index, BookItem* child);

	const QString& getLabel() const { return label; }
	const QString& getID() const { return id; }
	const BookDescription& getBookDescr() const { return book_descr; }

	static QString		path_separator;

	QList<const BookItem*> pathTo(const BookItem*) const;
	QList<const BookItem*> pathToRoot() const;

private:

	void removeChildren(const QStringList& list);
	void removeAllChildren();
	QString generateID();

	bool 				root, box;
	QString 			label, id;
	BookDescription 	book_descr;
	BookItem*			parent_item;
	QList<BookItem*>	children;	
};

#endif
