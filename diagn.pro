HEADERS = diagn-window.h bookswindow.h bookitem.h bookmodel.h myassert.h newtestdialog.h \
	checktestdialog.h config.h logger.h readonlyview.h webaccess.h addboxdialog.h helpdialog.h
SOURCES = main.cpp diagn-window.cpp bookswindow.cpp bookitem.cpp bookmodel.cpp myassert.cpp newtestdialog.cpp \
	checktestdialog.cpp config.cpp logger.cpp readonlyview.cpp webaccess.cpp addboxdialog.cpp \
	helpdialog.cpp
TARGET = diagn
QT += network
CONFIG += qt release
FORMS = newtestdialog.ui checktestdialog.ui addboxdialog.ui helpdialog.ui
contains(QT_VERSION, ^4\\.3\\..*) {
    FORMS += qt-4.3/diagn-window.ui
} else {
    FORMS += diagn-window.ui
}
RESOURCES = diagn.qrc
win32 {
    TARGET = diagn
    RC_FILE = diagn.rc
} else {
    TARGET = reflect-diagn
}
