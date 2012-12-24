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

#include "webaccess.h"

WebAccess::WebAccess(QObject* parent):
	QObject(parent)
{
#if QT_VERSION >= 0x040700
	connect(&network, SIGNAL(finished(QNetworkReply*)),
			this, SLOT(requestFinished(QNetworkReply*)));
#else 
	connect(&http, SIGNAL(requestFinished(int, bool)),
			this, SLOT(requestFinished(int, bool)));
#endif
}

bool WebAccess::post(const QUrl& url, Logger& logger)
{
	error = false;
	QByteArray postdata = logger.closeAndRead();
#if QT_VERSION >= 0x040700
	QNetworkRequest req;
	req.setUrl(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
	req.setHeader(QNetworkRequest::ContentLengthHeader, postdata.length());
	network.post(req, postdata);
#else
	if(http.state() != QHttp::Unconnected) http.close();
	httpRequestID = -1;
	QHttpRequestHeader  header;
	header.setRequest("POST", url.path(), 1, 1);
	header.setValue("Host", url.host());
	header.setContentType("application/octet-stream");
	header.setContentLength(postdata.length());
	httpRequestID = http.request(header, postdata);
#endif
	loop.exec();
	if(error) {
		logger.write(error_string);
	}
	return !error;
}

#if QT_VERSION >= 0x040700										

void WebAccess::requestFinished(QNetworkReply* reply)
{
	QNetworkReply::NetworkError net_error = reply->error();
	if (net_error != QNetworkReply::NoError) {
		error_string = QString("NetworkError: %1").arg(net_error);
		error = true;			
	} else {
		QByteArray res = reply->readAll();
		QString result = QString::fromUtf8(res).trimmed();
		error = result != "OK";
		error_string = QString("WrongNetworkResult: '%1'").arg(result);
	}
	loop.exit();
}

#else

void WebAccess::requestFinished(int id, bool e)
{
	if(id != httpRequestID) return ;
	httpRequestID = -1;
	if(e) {
		error_string = QString("NetworkError: %1").arg(http->error());
		error = true;			
	} else {
		QByteArray res = http.readAll();
		QString result = QString::fromUtf8(res).trimmed();
		error = result != "OK";
		error_string = QString("WrongNetworkResult: '%1'").arg(result);
	}
	loop.exit();
}

#endif
