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

#include <QMap>
#include <algorithm>
#include "bookitem.h"
#include "myassert.h"

QString BookItem::path_separator = "/";

BookItem::BookItem()
{
	box = false;
	parent_item = NULL;
	label = "";
	id = "";
}

BookItem::BookItem(BookItem* parent, const QString& boxLabel)
{
	box = true;
	parent_item = parent;
	MY_ASSERT(parent);
	label = boxLabel;
	id = generateID();
}

BookItem::BookItem(BookItem* parent, const BookDescription& bd, const QString& uid)
{
	box = false;
	parent_item = parent;
	MY_ASSERT(parent);
	label = bd.toTextString();
	if(uid.isEmpty()) {
		id = generateID();
	} else {
		id = uid;
	}
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
	MY_ASSERT(isRoot());
	removeAllChildren();
	foreach(BookDescription bd, list) {
		children.append(new BookItem(this, bd));
	}
}

void BookItem::appendBooks(const QMap<QString, BookDescription>& books)
{
	MY_ASSERT(isRoot());
	foreach(QString uid, books.keys()) {
		const BookDescription& bd = books[uid];
		children.append(new BookItem(this, bd, uid));
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

BookItem* BookItem::makeSubBox(const QString& label, int row)
{
	BookItem* box = new BookItem(this, label);
	children.insert(row, box);
	return box;
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

void BookItem::insertChild(int index, BookItem* child)
{
	child->parent_item = this;
	children.insert(index, child);
}

void BookItem::moveChild(int old_index, int new_index)
{
	BookItem* child = children.at(old_index);	
	children.insert(new_index, child);
	if(old_index < new_index) {				
		children.removeAt(old_index);
	} else {		
		children.removeAt(old_index + 1);
	}
}

QString BookItem::constructPath() const
{
	if(isRoot()) {
		return path_separator;
	} else {
		MY_ASSERT(parent_item);
		if(parent_item->isRoot()) {
			return path_separator + id;
		} else {
			return parent_item->constructPath() + path_separator + id;
		}
	}
}

#include <QUuid>

QString BookItem::generateID()
{
	return QUuid::createUuid().toString();
}

bool listsIntersect(const QList<const BookItem*>& a,
					const QList<const BookItem*>& b)
{
	for(int i = 0; i < std::min(a.size(), b.size()); i++) {
		if(a.at(a.size() - i - 1) != b.at(b.size() - i - 1)) return false;
	}
	return true;
}

QList<const BookItem*> BookItem::pathTo(const BookItem* toitem) const
{
	QList<const BookItem*> result;
	QList<const BookItem*> from_path_to_root = pathToRoot();
	QList<const BookItem*> to_path_to_root = toitem->pathToRoot();
	int find_index = -1;
	for(int i = 0; i < from_path_to_root.size(); i++) {
		const BookItem* current = from_path_to_root.at(i);
		find_index = to_path_to_root.indexOf(current);
		if(find_index != -1) {
			break;
		} else {
			result.append(current);
		}
	}
	if(!listsIntersect(from_path_to_root, to_path_to_root)) {
		find_index--;
	}
	for(int i = find_index; i >= 0; i--) {
		result.append(to_path_to_root.at(i));
	}
	return result;
}

QList<const BookItem*> BookItem::pathToRoot() const
{
	QList<const BookItem*> result;
	result.append(this);
	const BookItem* parent = parent_item;
	while(parent) {
		result.append(parent);
		parent = parent->parent_item;
	} 
	return result;
}
