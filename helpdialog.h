/*****************************************************************************
 * 
 * Test instruction dialog
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

#ifndef HELP_DIALOG_HEADER
#define HELP_DIALOG_HEADER

#include <QDialog>
#include "ui_helpdialog.h"

class HelpDialog: public QDialog, public Ui::HelpDialog {
Q_OBJECT
public:
	HelpDialog(int testnumber,
			   int booknumber,
			   const QString& abook,
			   const QString& operstr,
			   QWidget* parent = 0);
};

#endif
