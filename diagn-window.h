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

#ifndef DIAGNOSTICS_WINDOW
#define DIAGNOSTICS_WINDOW

#include <QMainWindow>
#include "ui_diagn-window.h"
#include "bookmodel.h"
#include "bookswindow.h"
#include "newtestdialog.h"

class DiagnosticsWindow: public QMainWindow, public Ui::DiagnosticsWindow {
Q_OBJECT
public:
	DiagnosticsWindow(QWidget* parent = 0);
	virtual void showMaximized();

public slots:
	void slotNewTest();
	void slotNewBox();
	void slotFinishEditing();
	void slotHelp();
	void slotDeleteBox();
	void slotRename();

protected slots:
	void slotRootClosed();
	void boxClicked(QModelIndex box);
	void boxMoved(const QString& from, const QString& to);
	void boxRenamed(const QString& path, const QString& label);

signals:
	void startNewTest();

private:
	void createWindow(QModelIndex root);
	void secondPart();
	bool yesNoDialog(const QString& title, const QString& text);
	BooksList loadBooks();

	static const char* booksFile;
	BookModel* model;	
	bool firsttest;
	TestParticipant testpart;
};

#endif
