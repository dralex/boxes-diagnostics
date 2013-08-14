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

#include <QDateTime>
#include <QUuid>
#include <QTranslator>
#include "diagn-window.h"
#include "main.h"

const char* version = "1.6";

int main(int argc, char *argv[])
{
	DiagnApplication app(argc, argv);
#ifdef DIAGN_ENGLISH
	QTranslator translator;
	if(translator.load("diagn_en")) {
		app.installTranslator(&translator);
	}
#endif
	qsrand(QDateTime::currentDateTime().toTime_t());	
	try {
		QString logfile = QString("diagn-%1%2.log").arg(QDateTime::currentDateTime().toTime_t()).arg(qrand() % 10000);
		Logger logger(logfile);
		logger.write(QString("AppVersion: %1").arg(version));
		logger.write(QString("Language: %1").arg(app.language()));
		logger.write(QString("DiagnID: %1 %2").arg(QUuid::createUuid().toString()).arg(logfile));
#ifdef Q_WS_WIN
		logger.write("SysInfo: MS Windows");
#elif defined(Q_WS_MAC)
		logger.write("SysInfo: Mac OS X");
#else
		logger.write("SysInfo: Linux");
#endif
		DiagnosticsWindow win(logger);
		win.showMaximized();
		int res = app.exec();
		return res;
	} catch(const QString& error) {
		app.printMessage(error);
	} catch(...) {
		app.printMessage();
	}
	return 1;
}
