/*****************************************************************************
 * 
 * Web server access interface.
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

#ifndef WEB_ACCESS_HEADER
#define WEB_ACCESS_HEADER

#include <QtGlobal>
#if QT_VERSION >= 0x040700
  #include <QNetworkAccessManager>
  #include <QNetworkRequest>
  #include <QNetworkReply>
#else
  #include <QHttp>
#endif
#include <QEventLoop>
#include <QUrl>
#include "logger.h"

class WebAccess: public QObject {
Q_OBJECT
public:
	WebAccess(QObject* parent = 0);
	bool post(const QUrl& url, Logger& logger);
										
protected slots:
#if QT_VERSION >= 0x040700
	void requestFinished(QNetworkReply*);
#else
	void requestFinished(int id, bool error);
#endif

private:
#if QT_VERSION >= 0x040700
	QNetworkAccessManager 		network;
#else
	int							httpRequestID;
	QHttp						http;
#endif
	QEventLoop					loop;
	bool						error;
	QString						error_string;
};

#endif
