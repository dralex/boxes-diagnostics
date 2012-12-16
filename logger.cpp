/*****************************************************************************
 * 
 * Test result logger.
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

#include <QDir>
#include "logger.h"

Logger::Logger(const QString& l):
	logfile(l), logpath(QString("%1%2%3").arg(QDir::homePath()).arg(QDir::separator()).arg(l)),
	file(logpath)
{
	if(!file.open(QFile::WriteOnly | QFile::Truncate)) {
		throw QString::fromUtf8("Не могу создать журнал диагностики %1").arg(l);
	}
}
	
void Logger::write(const QString& s)
{
	QString msg = QString("%1\n").arg(s);
	file.write(msg.toUtf8().data()); 
	file.flush();
}

QByteArray Logger::closeAndRead()
{
	QByteArray res;
	file.close();
	if(!file.open(QFile::ReadOnly)) {
		throw QString::fromUtf8("Не могу открыть журнал диагностики %1").arg(logfile);
	}
	res = file.readAll();
	file.close();
	if(!file.open(QFile::Append)) {
		throw QString::fromUtf8("Не могу открыть журнал диагностики %1 для дальнейшей записи").arg(logfile);
	}
	return res;
}
