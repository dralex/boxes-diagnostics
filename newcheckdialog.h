/*****************************************************************************
 * 
 * Checking dialog window for the Books diagnostics (new version).
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

#ifndef NEW_CHECK_DIALOG_HEADER
#define NEW_CHECK_DIALOG_HEADER

#include <QDialog>
#include "ui_newcheckdialog.h"
#include "bookmodel.h"

class NewCheckDialog: public QDialog, public Ui::NewCheckDialog {
Q_OBJECT
public:
	NewCheckDialog(Logger& logger,
					BookModel* model,
					const QString& question,
					const BooksList& books,
					bool search,
					QWidget* parent = 0);

	unsigned int operationsResult() const { return operations; }

public slots:
	void slotOK();
	void slotOK2();
	void slotRestart();
	void slotAddBox();
	void slotBookClicked(QModelIndex index);
	void slotBoxExpanded(QModelIndex index);
	void slotBoxCollapsed(QModelIndex index);

private:
	void incrementOperations(int diff = 1);
	void updateControls();
	int  calculateDistance(QModelIndex from,
						   QModelIndex to);

	Logger&			logger;
	BookModel*		model;
	bool			do_search;
	BooksList		search_books;
	unsigned int	operations;
	QModelIndex		current_index;
	QModelIndex		parent_index;
	unsigned int	start_time;
};

#endif
