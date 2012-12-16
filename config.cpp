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

#include <QFile>
#include <QStringList>
#include "config.h"

Config::Config(const QString& cfg):
	cfgfile(cfg), settings(cfg, QSettings::IniFormat)
{
}

bool Config::parseBook(const QString& str, BookDescription& bd)
{
	QStringList sl = str.split(":");
	if(sl.size() != 3) return false;
	bd.author_name = sl.at(0);
	bd.author_surname = sl.at(1);
	bd.title = sl.at(2);
	return true;
}
	
BooksList Config::books()
{
	if(!settings.contains("Test/books")) throwError();
	QString booksFile = settings.value("Test/books").toString();
	int maxSize = -1;
	if(settings.contains("Test/size")) {
		maxSize = settings.value("Test/size").toInt();
	}
	BooksList bl;
	QFile file(booksFile);
	if(file.open(QFile::ReadOnly)) {
		while (!file.atEnd()) {
			QString line = QString::fromUtf8(file.readLine()).trimmed();
			if(line.isEmpty()) continue;
			BookDescription bd;
			if(!parseBook(line, bd)) continue;
			bl.append(bd);
			if(maxSize > 0 && bl.size() == maxSize) break;
		}
		file.close();
	}
	return bl;
}

int Config::booksNumber()
{
	if(!settings.contains("Test/size")) throw QString("Не задан размер списка книг");
	return settings.value("Test/size").toInt();
}

BookDescription Config::bookParameter(const QString& key)
{
	if(!settings.contains(key)) throwError();
	QString bookStr = QString::fromUtf8(settings.value(key).toByteArray());
	BookDescription bd;
	if(!parseBook(bookStr, bd)) throwError();
	return bd;	
}

BookDescription Config::searchBook(int stage)
{
	QString key = QString("Test/search%1").arg(stage);
	return bookParameter(key);
}

BookDescription Config::addBook(int stage)
{
	QString key = QString("Test/add%1").arg(stage);
	return bookParameter(key);
}

QUrl Config::postAddress()
{
	if(!settings.contains("Post/address")) throwError();
	return QUrl(settings.value("Post/address").toString());	
}

#include <math.h>

int Config::targetOperations()
{
	return int(floor(log(double(booksNumber()))/log(2))) * 2;
}
