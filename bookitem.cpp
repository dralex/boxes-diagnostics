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

#include "bookitem.h"
#include "myassert.h"

QString BookItem::path_separator = "/";

BookItem::BookItem()
{
	box = false;
	parent_item = NULL;
	label = "";
}

BookItem::BookItem(BookItem* parent, const QString& boxLabel)
{
	box = true;
	parent_item = parent;
	MY_ASSERT(parent);
	label = boxLabel;
	id = generateID();
}

BookItem::BookItem(BookItem* parent, const BookDescription& bd)
{
	box = false;
	parent_item = parent;
	MY_ASSERT(parent);
	label = bd.author + " - " + bd.title;
	id = generateID();
	book_descr = bd;
}

BookItem::~BookItem()
{	
	removeAllChildren();
}

void BookItem::removeChildren(const QStringList& remove)
{
	foreach(QString child_to_remove, remove) {
		int idx = findChildID(child_to_remove);
		if(idx == -1) continue;		
		BookItem* item = child(idx);
		MY_ASSERT(item);
		delete item;
		children.removeAt(idx);
	}
}

void BookItem::removeAllChildren()
{
	foreach(BookItem* item, children) {
		MY_ASSERT(item);
		delete item;
	}
	children.clear();
}

int BookItem::subboxesCount() const
{
	int count = 0;
	foreach(BookItem* item, children) {
		if(item->isBox()) count++;
	}
	return count;
}

int BookItem::row() const
{
	if(isRoot()) return 0;
	MY_ASSERT(parent_item);
	MY_ASSERT(parent_item->childCount() > 0);
	return parent_item->children.indexOf(const_cast<BookItem*>(this));
}

BookItem* BookItem::child(int index)
{
	if(index < 0 || index >= children.size()) return NULL;
	if(children.size() == 0) return NULL;
	return children.value(index);
}

void BookItem::setBooks(const BooksList& list)
{
	removeAllChildren();

	foreach(BookDescription bd, list) {
		children.append(new BookItem(this, bd));
	}
}

int BookItem::findChildID(const QString& id) const
{
	int idx = 0;
	foreach(BookItem* child, children) {
		if(child->id == id) {
			return idx;
		}		
		idx++;
	}
	return -1;
}

void BookItem::makeSubBox(const QString& label)
{
	children.append(new BookItem(this, label));
}

void BookItem::removeChild(int row)
{
	children.removeAt(row); 
} 

void BookItem::addChild(BookItem* child)
{
	child->parent_item = this;
	children.append(child);
}

QString BookItem::constructPath() const
{
	if(parent_item == NULL) {
		return path_separator;
	} else {
		MY_ASSERT(parent_item);
		return parent_item->constructPath() + path_separator + id;
	}
}

#include <QUuid>

QString BookItem::generateID()
{
	return QUuid::createUuid().toString();
}
