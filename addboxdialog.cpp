/*****************************************************************************
 * 
 * Add box input dialog
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

#include "addboxdialog.h"

AddBoxDialog::AddBoxDialog(QWidget* parent):
	QDialog(parent)
{
	setupUi(this);
	iconLabel->setPixmap(QIcon(":/Icons/images/empty-box.png").pixmap(48, 48));
}

QString AddBoxDialog::boxName() const
{
	return nameEdit->text();
}

bool AddBoxDialog::addBefore() const
{
	return beforeRB->isChecked();
}

void AddBoxDialog::slotNameChanged(QString)
{
	addButton->setEnabled(!boxName().isEmpty());
}
