/*****************************************************************************
 * 
 * New test parameters dialog for the Books diagnostics.
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

#include "newtestdialog.h"

QString TestParticipant::serialize() const
{
	return "";
}

NewTestDialog::NewTestDialog(QWidget* parent):
	QDialog(parent)
{
	setupUi(this);
}

TestParticipant NewTestDialog::result() const
{
	TestParticipant tp;
	tp.school = schoolEdit->text();
	tp.cls = classEdit->text();
	tp.name = nameEdit->text();	
	tp.experience = expEdit->toPlainText();	
	return tp;
}

void NewTestDialog::slotTextChanged(QString)
{
	slotBigTextChanged();
}

void NewTestDialog::slotBigTextChanged()
{
	buttonBox->setEnabled(!nameEdit->text().isEmpty() &&
						  !expEdit->toPlainText().isEmpty());
}
