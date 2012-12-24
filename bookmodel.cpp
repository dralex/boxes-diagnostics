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
#include <QMimeData>

#include "bookmodel.h"
#include "myassert.h"
#include <QDebug>

BookModel::BookModel(Logger& l, QObject *parent):
	QAbstractItemModel(parent), logger(l)
																	
{
	root = new BookItem();	
	emptyBoxIcon = QIcon(":/Icons/images/empty-box.png");
	fullBoxIcon = QIcon(":/Icons/images/full-box.png");
	bookIcon = QIcon(":/Icons/images/book.png");
	diagnMimeType = "application/vnd.diagn.biblitem";
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
	case Qt::EditRole:
		return item->getLabel();
	case Qt::DecorationRole:
		if(index.column() == 0) {
			return getItemIcon(item);
		} 
	default:
		return QVariant();
	}
}

bool BookModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if(index.isValid() && role == Qt::EditRole && index.column() == 0) {
		BookItem *item = static_cast<BookItem*>(index.internalPointer());
		MY_ASSERT(item);
		MY_ASSERT(!item->isRoot());	
		QString newLabel = value.toString();
		if(newLabel.isEmpty()) return false;
		item->rename(newLabel);
		if(item->isBox()) emit boxRenamed(item->constructPath(), newLabel);
		emit dataChanged(index, index);
		return true;
	}
	return false;
}

Qt::ItemFlags BookModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags defaultFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	const BookItem* item = indexToItem(index);
	if(item->isRoot()) return Qt::ItemIsDropEnabled | defaultFlags;
	else if(item->isBox()) return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags | Qt::ItemIsEditable;
	else return Qt::ItemIsDragEnabled | defaultFlags | Qt::ItemIsEditable;
}

bool BookModel::hasIndex(int row, int column, const QModelIndex & parent) const
{
	if(column > 0) return false;
	if(!parent.isValid()) {
		return row >= 0 && row < root->childCount();
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
	BookItem *item = NULL;
	if(!parent.isValid()) {
		item = root;
	} else {
		item = static_cast<BookItem*>(parent.internalPointer());
	}
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
	emit dataChanged(index(start, 0, theindex), index(end, 0, theindex));
	logger.write(QString("Books: %1").arg(list.size()));
	for(int i = 0; i < root->childCount(); i++) {
		BookItem* child = root->child(i);
		MY_ASSERT(child);
		logger.write(QString(" %1{%2}").arg(child->getID()).arg(child->getBookDescr().toString()));
	}
}

void BookModel::appendBooks(const QMap<QString, BookDescription>& books)
{
	QModelIndex theindex = rootIndex();
	int start = rowCount(theindex);
	int end = start + books.size() - 1;
	beginInsertRows(theindex, start, end);
	root->appendBooks(books);
	endInsertRows();
	emit dataChanged(index(start, 0, theindex), index(end, 0, theindex));
	logger.write(QString("Books returned to root: %1").arg(books.size()));
	foreach(QString id, books.keys()) {
		logger.write(" " + id);
	}
}

void BookModel::newBox(const QString& parentdir, const QString& name, int row)
{
	QModelIndex theindex = pathToIndex(parentdir);
	BookItem* parent = indexToItem(theindex);
	MY_ASSERT(parent);
//	int size = parent->childCount();
	beginInsertRows(theindex, row, 0);
	BookItem* boxItem = parent->makeSubBox(name, row);
	endInsertRows();
	logger.write(QString("New box %1{%2} at %3:%4").arg(boxItem->getID()).arg(name).arg(parent->getID()).arg(row));
}

void BookModel::newBook(const QString& parentdir, int row, const BookDescription& book)
{
	QModelIndex theindex = pathToIndex(parentdir);
	BookItem* parent = indexToItem(theindex);
	MY_ASSERT(parent);
	MY_ASSERT(row >= 0 && row <= parent->childCount());
	beginInsertRows(theindex, row, 0);
	BookItem* bookItem = new BookItem(parent, book);
	parent->insertChild(row, bookItem);
	endInsertRows();
	logger.write(QString("New book %1{%2} at %3:%4").arg(bookItem->getID()).arg(book.toString()).arg(parent->getID()).arg(row));
}

void BookModel::rename(const QString& path, const QString& name)
{
	QModelIndex theindex = pathToIndex(path);
	MY_ASSERT(theindex.isValid());
	BookItem* item = indexToItem(theindex);
	MY_ASSERT(item);
	item->rename(name);
	emit dataChanged(theindex, theindex);
	logger.write(QString("Rename %1{%2}").arg(item->getID()).arg(name));
}

void BookModel::move(const QString& srcpath, const QString& destpath, int row)
{
	QModelIndex srcindex = pathToIndex(srcpath);
	QModelIndex parentindex = parent(srcindex);	
	QModelIndex dstindex = pathToIndex(destpath);

	MY_ASSERT(srcindex.isValid());

	int remove_index = srcindex.row();
	int add_index;
	if(row < 0 || row > rowCount(dstindex)) {
		add_index = rowCount(dstindex);
	} else {
		add_index = row;
	}

	BookItem* item = indexToItem(srcindex);
	BookItem* parentitem = indexToItem(parentindex);
	BookItem* dstitem = indexToItem(dstindex);

	beginRemoveRows(parentindex, remove_index, remove_index);
	parentitem->removeChild(remove_index);
	endRemoveRows();

	beginInsertRows(dstindex, add_index, add_index);
	dstitem->insertChild(add_index, item);
	endInsertRows();

	if (item->isBox()) {
		if(dstitem->isRoot()) {
			emit boxMoved(srcpath, destpath + item->getID());
		} else {
			emit boxMoved(srcpath, destpath + BookItem::path_separator + item->getID());
		}
	}
	emit boxInserted(item->constructPath());
	logger.write(QString("Moved %1->%2").arg(item->getID()).arg(dstitem->getID()));
}

void BookModel::insert(const QString& srcpath, int oldrow, int newrow)
{
	QModelIndex srcindex = pathToIndex(srcpath);
	QModelIndex parentindex = parent(srcindex);
	MY_ASSERT(srcindex.isValid());
	MY_ASSERT(srcindex.row() == oldrow);
#if QT_VERSION >= 0x040700
	beginMoveRows(parentindex, oldrow, oldrow, parentindex, newrow);
#else
	beginRemoveRows(parentindex, oldrow, oldrow);
	beginInsertRows(parentindex, newrow, newrow);
#endif
	BookItem* parentitem = indexToItem(parentindex);
	MY_ASSERT(parentitem);
	BookItem* item = indexToItem(srcindex);
	MY_ASSERT(item);
	parentitem->moveChild(oldrow, newrow);
#if QT_VERSION >= 0x040700
	endMoveRows();
#else
	endInsertRows();
	endRemoveRows();
	emit boxInserted(srcpath);
#endif
	if(item->isBox()) {
		emit boxMoved(srcpath, srcpath);
	}
	logger.write(QString("Reodered %1 %2->%3").arg(parentitem->getID()).arg(oldrow).arg(newrow));
}

void BookModel::removeRowsRecursively(QModelIndex parent, QMap<QString, BookDescription>& books)
{
	for(int i = 0; i < rowCount(parent); i++) {
		QModelIndex child = index(i, 0, parent);
		BookItem* item = indexToItem(child);
		logger.write(QString("Remove recursively %1").arg(item->getID()));
		if (item->isBox()) {
			removeRowsRecursively(child, books);
		} else {
			books[item->getID()] = item->getBookDescr();
		}
	}
	if(rowCount(parent) > 0) 
		beginRemoveRows(parent, 0, rowCount(parent) - 1);
}

void BookModel::remove(const QString& path)
{
	QModelIndex srcindex = pathToIndex(path);
	if(!srcindex.isValid()) return ;
	QModelIndex parentindex = parent(srcindex);	
	int remove_index = srcindex.row();
	QMap<QString, BookDescription> books;
	removeRowsRecursively(srcindex, books);
	beginRemoveRows(parentindex, remove_index, remove_index);
	BookItem* item = indexToItem(srcindex);
	BookItem* parentitem = indexToItem(parentindex);
	parentitem->removeChild(remove_index);
	logger.write(QString("Remove %1").arg(item->getID()));
	delete item;
	endRemoveRows();
	appendBooks(books);
}
/*
void BookModel::moveItemsRecursively(BookItem* root)
{
	int i = 0;
	while(i < root->childCount()) {
		BookItem* child = root->child(i);
		MY_ASSERT(child);
		if(child->isBox()) {
			if(child->childCount() > 0) 
				moveItemsRecursively(child);						
		} else {
			move(child->constructPath(), BookItem::path_separator);
//			qDebug() << "moving " + child->constructPath() + " to " + BookItem::path_separator;
		} 
		i++;
	}
}
*/
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

Qt::DropActions BookModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

bool BookModel::dropMimeData(const QMimeData *data,
							Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
//	qDebug() << "action " << action << " row " << row << " column " << column << " parent " << indexToPath(parent);
	if(action == Qt::IgnoreAction) {
		return true;
	}
	if(column > 0) return false;
	BookItem* target_item = indexToItem(parent);
	if(!target_item->isRoot() && !target_item->isBox()) {		
		return false;
	}
	QString target_path = target_item->constructPath();
//	qDebug() << "Drop target: " << target_path;
	if(data->hasFormat(diagnMimeType)) {
		QByteArray encodedData = data->data(diagnMimeType);
		QDataStream stream(&encodedData, QIODevice::ReadOnly);
		QStringList items;
		
		while (!stream.atEnd()) {
			QString path;
			stream >> path;
			items.append(path);
		}

		foreach(QString source_path, items) {
			if(target_path.indexOf(source_path) == 0) {
//				qDebug() << "Denied. source: " << source_path << " target: " << target_path;
				return false;
			}
			BookItem* source_item = pathToItem(source_path);
			BookItem* parent_item = source_item->parent();
			MY_ASSERT(parent_item);		
			QString parent_path = parent_item->constructPath();		
			if(target_path == parent_path) {
				if(row == source_item->row()) {
//					qDebug() << "Denied. parent: " << parent_path << " target: " << target_path << " same row! ";
					return false;
				} if(row == -1) {
					insert(source_path, source_item->row(), parent_item->childCount());
				} else {
					insert(source_path, source_item->row(), row);
				}
				
			} else {
				if(row == -1) {
					move(source_path, target_path, target_item->childCount());
				} else {
					move(source_path, target_path, row);
				}
			}
		}
		return true;
	} else {
		return false;
	}
}

QStringList BookModel::mimeTypes() const
{
	return QStringList(diagnMimeType);
}

QMimeData* BookModel::mimeData(const QModelIndexList &indexes) const
{
	QMimeData *mimeData = new QMimeData;
	QByteArray encodedData;
	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	QString path;
	foreach(QModelIndex index, indexes) {
		if(index.column() != 0) continue;
		const BookItem* item = indexToItem(index);
		path = item->constructPath();
		stream << path;
	}
	mimeData->setData(diagnMimeType, encodedData);	
	return mimeData;	
}

void BookModel::dump()
{
	dumpRecursively(root, "");
}

void BookModel::dumpRecursively(BookItem* root, const QString& indent)
{
	QString type;
	QString descr;
	QString label;
	if(root->isRoot()) {
		type = "Root";
	} else if(root->isBox()) {
		type = "B";
		label = root->getLabel();
	} else {
		type = "F";
		const BookDescription& bd = root->getBookDescr();
		descr = bd.toString();
		if((bd.toTextString()) != root->getLabel()) {
			label = root->getLabel();
		}
	}
	logger.write(QString("%1%2%3{%4}{%5}").arg(indent).arg(type).arg(root->getID()).arg(descr).arg(label));
	QString newindent = indent + "\t";
	for(int i = 0; i < root->childCount(); i++) {
		dumpRecursively(root->child(i), newindent);
	}
}
