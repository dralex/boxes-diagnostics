/*****************************************************************************
 * 
 * Base definitions for the Books diagnostics.
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

#ifndef BOOK_HEADER
#define BOOK_HEADER

#include <QString>
#include <QList>

struct BookDescription {
	QString author_name;
	QString author_surname;
	QString title;

	QString toTextString() const {
		return author_name + " " + author_surname + " - " + title;
	}

	QString toString() const {
		return author_name + ":" + author_surname + ":" + title;
	}

	bool operator==(const BookDescription& b) const {
		return (author_name == b.author_name &&
				author_surname == b.author_surname &&
				title == b.title);
	}
};

typedef QList<BookDescription> BooksList;

#endif
