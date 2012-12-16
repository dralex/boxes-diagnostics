/*****************************************************************************
 * 
 * Configuration file reader.
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

#ifndef CONFIG_HEADER
#define CONFIG_HEADER

#include <QUrl>
#include <QSettings>
#include "book.h"

class Config {
public:
	Config(const QString& cfg);
	
	BooksList books();
	int booksNumber();
	BookDescription searchBook(int stage);
	BookDescription addBook(int stage);
	int targetOperations();

	QUrl postAddress();

private:
	void throwError() {
		throw QString::fromUtf8("Не доступе файл конфигурации %1").arg(cfgfile);
	}
	static bool parseBook(const QString& str, BookDescription& bd);
	BookDescription bookParameter(const QString& key);

	QString cfgfile;
	QSettings settings;
};

#endif
