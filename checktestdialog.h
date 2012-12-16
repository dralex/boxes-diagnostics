/*****************************************************************************
 * 
 * Checking dialog window for the Books diagnostics.
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

#ifndef CHECK_TEST_DIALOG_HEADER
#define CHECK_TEST_DIALOG_HEADER

#include <QDialog>
#include "ui_checktestdialog.h"
#include "bookmodel.h"

class CheckTestDialog: public QDialog, public Ui::CheckTestDialog {
Q_OBJECT
public:
	CheckTestDialog(BookModel* model,
					const BookDescription& book, 
					bool search,
					QWidget* parent = 0);

	unsigned int operationsResult() const { return operations; }

public slots:
	void slotRestart();
	void slotOpen();
	void slotClose();
	void slotBefore();
	void slotAfter();
	void slotFirst();
	void slotLast();
	void slotAddBox();

private:
	void updateControls();

	BookModel*		model;
	bool			do_search;
	BookDescription search_book;
	unsigned int	operations;
	QModelIndex		current_index;
	QModelIndex		parent_index;
};

#endif
