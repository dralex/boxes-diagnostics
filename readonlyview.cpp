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

#include <QDebug>
#include "readonlyview.h"

ReadOnlyView::ReadOnlyView(QWidget* parent): QListView(parent)
{
}

void ReadOnlyView::setReallyCurrentIndex(QModelIndex ci)
{
	current_index = ci;
	setCurrentIndex(current_index);
}

void ReadOnlyView::selectionChanged(const QItemSelection & selected, const QItemSelection& deselected)
{
	if(selected.contains(current_index)) {
		QListView::selectionChanged(selected, deselected);
	} else {
		setCurrentIndex(current_index);
	}
}
