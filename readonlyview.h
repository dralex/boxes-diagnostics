/*****************************************************************************
 * 
 * Read only QListView implementation.
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

#ifndef READ_ONLY_VIEW_HEADER
#define READ_ONLY_VIEW_HEADER

#include <QListView>

class ReadOnlyView: public QListView {
Q_OBJECT
public:
	ReadOnlyView(QWidget* parent);

	void setReallyCurrentIndex(QModelIndex ci);

protected slots:
	void selectionChanged(const QItemSelection & selected, const QItemSelection& deselected);

private:
	QModelIndex current_index;
};

#endif
