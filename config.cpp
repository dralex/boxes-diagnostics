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
	if(!settings.contains("Test/size")) throw trUtf8("Не задан размер списка книг");
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

BooksList Config::searchBooks(int stage)
{
	QString key = QString("Test/search%1").arg(stage);
	if(!settings.contains(key)) throwError();
	QString bookStr = QString::fromUtf8(settings.value(key).toByteArray());
	BooksList res;
	foreach(QString bs, bookStr.split("|")) {
		BookDescription bd;
		if(!parseBook(bs, bd)) throwError();
		res.append(bd);
	}
	return res;
}

QString Config::searchBookQuestion(int stage)
{
	QString key = QString("Test/search%1question").arg(stage);
	if (!settings.contains(key)) return "";
	return QString::fromUtf8(settings.value(key).toByteArray());
}

BooksList Config::addBook(int stage)
{
	QString key = QString("Test/add%1").arg(stage);
	BooksList bl;
	bl.append(bookParameter(key));
	return bl;
}

QUrl Config::postAddress()
{
	if(!settings.contains("Post/address")) throwError();
	return QUrl(settings.value("Post/address").toString());	
}

#include <math.h>

int Config::targetOperations()
{
	if(booksNumber() == 50) {
		return 7;
	} else if(booksNumber() == 75) {
		return 8;
	} else if(booksNumber() == 100) {
		return 9;
	} else {
		double logres = log(double(booksNumber())) / log(2.0);
		return int(floor(logres)) * 2;
	}
}
