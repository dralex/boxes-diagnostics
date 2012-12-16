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

#ifndef BOOK_MODEL_HEADER
#define BOOK_MODEL_HEADER

#include <QAbstractListModel>
#include <QIcon>
#include "bookitem.h"

class BookModel: public QAbstractItemModel {
Q_OBJECT
public:
	BookModel(QObject *parent);
	~BookModel();

	// CORE FUNCTIONALITY
	void reset();
	void setBooks(const BooksList& list);
	void appendBooks(const QMap<QString, BookDescription>& books);

	void newBox(const QString& path, const QString& label);	
	void rename(const QString& path, const QString& newlabel);
	void move(const QString& srcpath, const QString& destpath);
	void insert(const QString& srcpath, int oldrow, int newrow);
	void remove(const QString& box);

	// DATA REPRESENTATION
	QVariant data(const QModelIndex &index, int role) const;	
	Qt::ItemFlags flags(const QModelIndex &index) const;
	bool hasIndex(int row, int column, const QModelIndex & parent = QModelIndex()) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	bool hasChildren(const QModelIndex & parent = QModelIndex()) const;
	bool hasSubboxes(const QModelIndex & parent = QModelIndex()) const;

	const QIcon& getItemIcon(const BookItem* item) const;

	// EDITING
	bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

	// DRAG & DROP
	Qt::DropActions supportedDropActions() const;
	bool dropMimeData(const QMimeData *data,
					  Qt::DropAction action, int row, int column, const QModelIndex &parent);
	QMimeData* mimeData(const QModelIndexList &indexes) const;
	QStringList mimeTypes() const;

	// INDEXES
	QModelIndex rootIndex() const;
	QModelIndex childIndex(const QModelIndex &parent, const QString& id) const;
	QModelIndex pathToIndex(const QString& path) const;
	BookItem* indexToItem(const QModelIndex& index);
	BookItem* pathToItem(const QString& path);
	BookItem* nextItem(const BookItem* item) const;
	const BookItem* indexToItem(const QModelIndex& index) const;
	const BookItem* pathToItem(const QString& path) const;
	QString indexToPath(const QModelIndex& index) const;

signals:
	void dataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight);
	void modelAboutToBeReset();
	void modelReset();
	void boxMoved(const QString& from_path, const QString& to_path);
	void boxRenamed(const QString& path, const QString& label);

private:
	void removeRowsRecursively(QModelIndex parent, QMap<QString, BookDescription>& books);
	
	BookItem*	root;
	QString		diagnMimeType;
	QIcon		emptyBoxIcon, fullBoxIcon, bookIcon;
};

#endif

