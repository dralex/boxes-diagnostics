HEADERS = diagn-window.h bookswindow.h bookitem.h bookmodel.h myassert.h newtestdialog.h \
	checktestdialog.h config.h logger.h readonlyview.h webaccess.h
SOURCES = main.cpp diagn-window.cpp bookswindow.cpp bookitem.cpp bookmodel.cpp myassert.cpp newtestdialog.cpp \
	checktestdialog.cpp config.cpp logger.cpp readonlyview.cpp webaccess.cpp
TARGET = diagn
QT += network
CONFIG += qt release
FORMS = diagn-window.ui newtestdialog.ui checktestdialog.ui
RESOURCES = diagn.qrc
win32 {
    RC_FILE = diagn.rc
}
