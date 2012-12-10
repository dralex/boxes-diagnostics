/*****************************************************************************
 * 
 * ReflectDiagn main function.
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

#include <QMessageBox>
#include "diagn-window.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	try {
		DiagnosticsWindow win;
		win.show();
		int res = app.exec();
		return res;
	} catch(const QString& error) {
		QMessageBox::critical(0, QString::fromUtf8("Диагностика работы за компьютером"),
							  QString::fromUtf8("Ошибка при загрузке программы диагностики: %1").arg(error));
	} catch(...) {
		QMessageBox::critical(0, QString::fromUtf8("Диагностика работы за компьютером"),
							  QString::fromUtf8("Неизвестная ошибка при загрузке программы диагностики."));
	}
}
