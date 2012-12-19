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
#include "config.h"

class DiagnosticsWindow: public QMainWindow, public Ui::DiagnosticsWindow {
Q_OBJECT
public:
	DiagnosticsWindow(Logger& l, QWidget* parent = 0);
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
	void slotSaveResults();

signals:
	void startNewTest();

protected:
	void closeEvent(QCloseEvent* event);

private:
	bool finishTest();
	void secondPart();
	void createWindow(QModelIndex root);
	bool yesNoDialog(const QString& title, const QString& text);
	QString helpText();
	void showSearchResults(unsigned int seares, unsigned int addres, unsigned int seares2 = 0);
	bool sendResults();

	static const char* configFile;
	Config config;
	BookModel* model;	
	int testnumber;
	TestParticipant testpart;
	Logger& logger;
	QString firstbook;

	struct TestTiming {
		unsigned int start;
		unsigned int edit1end;
		unsigned int search1end;
		unsigned int search2end;
		unsigned int add1end;
		unsigned int edit2end;
		unsigned int search3end;
		unsigned int add2end;

		TestTiming()
			{
				start = edit1end = search1end = search2end = add1end = edit2end = search3end = add2end = 0;
			}
	} timing;
};

#endif
