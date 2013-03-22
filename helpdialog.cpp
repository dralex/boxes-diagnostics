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

#include "helpdialog.h"

HelpDialog::HelpDialog(int testnumber,
					   int booknumber,
					   const QString& abook,
					   const QString& operstr,
					   QWidget* parent):
	QDialog(parent)
{
	setupUi(this);
	infoLabel->setText(infoLabel->text().arg(booknumber).arg(abook));
	actionLabel->setText(actionLabel->text() + " " + operstr);
	if(testnumber > 0) {
		okButton->setText(trUtf8("Продолжить"));
	}
	resize(sizeHint());
}
