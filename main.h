/*****************************************************************************
 * 
 * ReflectDiagn application.
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

#ifndef DIAGN_APPLICATION
#define DIAGN_APPLICATION

#include <QApplication>
#include <QMessageBox>

class DiagnApplication: public QApplication {
Q_OBJECT
public:
	DiagnApplication(int & argc, char ** argv):
		QApplication(argc, argv) 
		{}
	QString language() const {
		return trUtf8("Русский");
	}

	virtual bool notify(QObject * receiver, QEvent * e)
		{
			try {
				return QApplication::notify(receiver, e);
			} catch(const QString& error) {
				printMessage(error);
				quit();
			} catch(...) {
				printMessage();
				quit();
			}
			return false;
		}
	void printMessage(const QString& msg = "")
		{
			if(!msg.isEmpty()) {
				QMessageBox::critical(0, trUtf8("Диагностика работы за компьютером", "Message title"),
									  trUtf8("Ошибка при загрузке программы диагностики: %1", "The message").arg(msg));	
			} else {
				QMessageBox::critical(0, trUtf8("Диагностика работы за компьютером", "Message title"),
									  trUtf8("Неизвестная ошибка при загрузке программы диагностики.", "The message"));
			}
		}
};

#endif
