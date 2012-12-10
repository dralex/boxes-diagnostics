/*****************************************************************************
 * 
 * Book/box model for the Books diagnostics.
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

#include <QIcon>
#include <QList>

#include "bookmodel.h"
#include "myassert.h"

BookModel::BookModel(QObject *parent):
	QAbstractItemModel(parent)
																	
{
	root = new BookItem();	
	emptyBoxIcon = QIcon(":/Icons/images/empty-box.png");
	fullBoxIcon = QIcon(":/Icons/images/full-box.png");
	bookIcon = QIcon(":/Icons/images/book.png");
}

BookModel::~BookModel()
{
	delete root;
}

void BookModel::reset()
{
	emit modelAboutToBeReset();
	MY_ASSERT(root);
	delete root;
	root = new BookItem();
	emit modelReset();	
}

QVariant BookModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();
	
	BookItem *item = static_cast<BookItem*>(index.internalPointer());
	MY_ASSERT(item);
	MY_ASSERT(!item->isRoot());

	switch(role) {
	case Qt::DisplayRole:
	case Qt::ToolTipRole:		
		return item->getLabel();
	case Qt::DecorationRole:
		if(index.column() == 0) {
			return getItemIcon(item);
		} 
	default:
		return QVariant();
	}
}

Qt::ItemFlags BookModel::flags(const QModelIndex &index) const
{
	if (!index.isValid()) return 0;
	Qt::ItemFlags defaultFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	const BookItem* item = indexToItem(index);
	if(item->isRoot()) return defaultFlags;
	else if(item->isBox()) return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
	else return Qt::ItemIsDragEnabled | defaultFlags;
}

bool BookModel::hasIndex(int row, int column, const QModelIndex & parent) const
{
	if(column > 0) return false;
	if(!parent.isValid()) {
		return root->childCount() > 0;
	} else {
		BookItem *parentItem = static_cast<BookItem*>(parent.internalPointer());
		MY_ASSERT(parentItem);
		if(parentItem->childCount() > 0) {
			return row >= 0 && row < parentItem->childCount();
		} else {
			return false;
		}
	}	
}

QModelIndex BookModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent)) return QModelIndex();

	BookItem *parentItem;
	if (!parent.isValid())
		parentItem = root;
	else
		parentItem = static_cast<BookItem*>(parent.internalPointer());
	MY_ASSERT(parentItem);

	BookItem *childItem = parentItem->child(row);
	MY_ASSERT(childItem);
	return createIndex(row, column, childItem);
}

QModelIndex BookModel::parent(const QModelIndex &index) const
{
	if (!index.isValid()) {
		return QModelIndex();
	}
	BookItem *childItem = static_cast<BookItem*>(index.internalPointer());
	MY_ASSERT(childItem);
	BookItem *parentItem = childItem->parent();
	
	if (parentItem == root) {
		return QModelIndex();
	}

	MY_ASSERT(parentItem);

	return createIndex(parentItem->row(), 0, parentItem);
}

int BookModel::rowCount(const QModelIndex &parent) const
{
	if (parent.column() > 0) return 0;
	
	BookItem *item = NULL;
	if (!parent.isValid())
		item = root;
	else
		item = static_cast<BookItem*>(parent.internalPointer());
	MY_ASSERT(item);

	return item->childCount();
}

int BookModel::columnCount(const QModelIndex &) const
{
	return 1;
}

bool BookModel::hasChildren(const QModelIndex & parent) const
{
	return rowCount(parent) > 0;
}

bool BookModel::hasSubboxes(const QModelIndex & parent) const
{
	if (!parent.isValid()) {
		return true;
	}

	const BookItem *item = NULL;
	if (parent.isValid())
		item = static_cast<BookItem*>(parent.internalPointer());
	else
		item = root;
	MY_ASSERT(item);

	return item->subboxesCount() > 0;
}

QModelIndex BookModel::rootIndex() const
{
	return QModelIndex();
}

QModelIndex BookModel::childIndex(const QModelIndex &parent, const QString& id) const
{
	MY_ASSERT(parent.isValid());
	BookItem *item = static_cast<BookItem*>(parent.internalPointer());
	MY_ASSERT(item);
	int idx = item->findChildID(id);
	if(idx == -1)
		return QModelIndex();
	else
		return index(idx, 0, parent);
}

QModelIndex BookModel::pathToIndex(const QString& path) const
{
	QModelIndex rootindex = rootIndex();
	if(path == BookItem::path_separator) return rootindex;
	QStringList sl = path.split(BookItem::path_separator);
	sl.pop_front();
	MY_ASSERT(sl.size() >= 1);
	foreach(QString part, sl) {
		QModelIndex childindex = childIndex(rootindex, part);
		if(!childindex.isValid()) 
			return QModelIndex();
		rootindex = childindex;
	}
	return rootindex;
}

const BookItem* BookModel::indexToItem(const QModelIndex& index) const
{
	if(index.isValid()) {
		BookItem *item = static_cast<BookItem*>(index.internalPointer());	
		MY_ASSERT(item);
		return item;
	} else {
		return root;
	}
}

BookItem* BookModel::indexToItem(const QModelIndex& index)
{
	if(index.isValid()) {
		BookItem *item = static_cast<BookItem*>(index.internalPointer());	
		MY_ASSERT(item);
		return item;
	} else {
		return root;
	}
}

const BookItem* BookModel::pathToItem(const QString& path) const
{
	return indexToItem(pathToIndex(path));
}

BookItem* BookModel::pathToItem(const QString& path)
{
	return indexToItem(pathToIndex(path));
}

QString BookModel::indexToPath(const QModelIndex& index) const
{
	return indexToItem(index)->constructPath();
}

void BookModel::setBooks(const BooksList& list)
{
	reset();
	int start = 0, end = list.size() - 1;
	QModelIndex theindex = rootIndex();
	beginInsertRows(theindex, start, end);
	root->setBooks(list);
	endInsertRows();
	emit dataChanged(index(0, 0, theindex), index(end, 0, theindex));
}

void BookModel::newBox(const QString& parentdir, const QString& name)
{
	QModelIndex theindex = pathToIndex(parentdir);
	BookItem* parent = indexToItem(theindex);
	MY_ASSERT(parent);
	int size = parent->childCount();
	beginInsertRows(theindex, size, size);
	parent->makeSubBox(name);
	endInsertRows();
}

void BookModel::rename(const QString& path, const QString& name)
{
	QModelIndex theindex = pathToIndex(path);
	MY_ASSERT(theindex.isValid());
	BookItem* item = indexToItem(theindex);
	MY_ASSERT(item);
	item->rename(name);
	emit dataChanged(theindex, theindex);
}

void BookModel::move(const QString& srcpath, const QString& destpath)
{
	QModelIndex srcindex = pathToIndex(srcpath);
	QModelIndex parentindex = parent(srcindex);	
	QModelIndex dstindex = pathToIndex(destpath);

	MY_ASSERT(srcindex.isValid());
	MY_ASSERT(parentindex.isValid());
	MY_ASSERT(dstindex.isValid());

	int remove_index = srcindex.row(),
		add_index = rowCount(dstindex);

	BookItem* item = indexToItem(srcindex);
	BookItem* parentitem = indexToItem(parentindex);
	BookItem* dstitem = indexToItem(dstindex);

	beginRemoveRows(parentindex, remove_index, remove_index);
	parentitem->removeChild(remove_index);
	endRemoveRows();

	beginInsertRows(dstindex, add_index, add_index);
	dstitem->addChild(item);
	endInsertRows();
}

Qt::DropActions BookModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

const QIcon& BookModel::getItemIcon(const BookItem* item) const 
{
	MY_ASSERT(item);
	if(item->isRoot()) {
		return fullBoxIcon;
	} else if(item->isBox()) {
		if(item->childCount() > 0) {
			return fullBoxIcon;
		} else {
			return emptyBoxIcon;
		}
	} else {
		return bookIcon;
	}	
}
