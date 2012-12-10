/*****************************************************************************
 * 
 * ReflectDiagn main window.
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

#include "diagn-window.h"
#include "book.h"

DiagnosticsWindow::DiagnosticsWindow(QWidget* parent):
	QMainWindow(parent)
{
	setupUi(this);
	model = new BookModel(this);
	BookDescription d1;
	d1.author = QString::fromUtf8("Алексей Федосеев");
	d1.title = QString::fromUtf8("Умная книга");
	BookDescription d2;
	d2.author = QString::fromUtf8("Вася Пупкин");
	d2.title = QString::fromUtf8("Как спасать Россию");
	BooksList bl;
	bl.append(d1);
	bl.append(d2);

	model->setBooks(bl);
	model->newBox("/", "sss");

	rootWindow = new BooksWindow(model, model->rootIndex(), this);
	booksArea->addSubWindow(rootWindow);

	BooksWindow* aw = new BooksWindow(model, model->rootIndex(), this);
	booksArea->addSubWindow(aw);
}
