HEADERS = diagn-window.h bookswindow.h bookitem.h bookmodel.h myassert.h newtestdialog.h \
	newcheckdialog.h config.h logger.h webaccess.h addboxdialog.h helpdialog.h main.h
SOURCES = main.cpp diagn-window.cpp bookswindow.cpp bookitem.cpp bookmodel.cpp myassert.cpp newtestdialog.cpp \
	newcheckdialog.cpp config.cpp logger.cpp webaccess.cpp addboxdialog.cpp \
	helpdialog.cpp
TARGET = diagn
QT += network
CONFIG += qt release
FORMS = newtestdialog.ui newcheckdialog.ui addboxdialog.ui helpdialog.ui
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
TRANSLATIONS = diagn_en.ts

# uncomment to compile Engilsh version:
# DEFINES += DIAGN_ENGLISH
