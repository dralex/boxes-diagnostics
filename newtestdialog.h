/*****************************************************************************
 * 
 * ReflectDiagn book window for the Books diagnostics.
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

#ifndef NEW_TEST_DIALOG_HEADER
#define NEW_TEST_DIALOG_HEADER

#include <QDialog>
#include "ui_newtestdialog.h"

struct TestParticipant {
	QString school;
	QString cls;
	QString name;
	QString experience;

	QString serialize() const;
};

class NewTestDialog: public QDialog, public Ui::NewTestDialog {
Q_OBJECT
public:
	NewTestDialog(QWidget* parent = 0);

	TestParticipant result() const; 

public slots:
	void slotTextChanged(QString);
	void slotBigTextChanged();
};

#endif
