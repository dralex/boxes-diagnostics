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

#ifndef LOGGER_HEADER
#define LOGGER_HEADER

#include <QFile>

class Logger: public QObject {
Q_OBJECT
public:
	Logger(const QString& logfile);
	
	QString logFile() const { return logfile; }
	QString logPath() const { return logpath; }
	void write(const QString& s);
	QByteArray closeAndRead();

private:
	QString logfile;
	QString logpath;
	QFile	file;
};

#endif
