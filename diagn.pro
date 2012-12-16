HEADERS = diagn-window.h bookswindow.h bookitem.h bookmodel.h myassert.h newtestdialog.h \
	checktestdialog.h
SOURCES = main.cpp diagn-window.cpp bookswindow.cpp bookitem.cpp bookmodel.cpp myassert.cpp newtestdialog.cpp \
	checktestdialog.cpp
TARGET = diagn
CONFIG += qt release
FORMS = diagn-window.ui newtestdialog.ui checktestdialog.ui
RESOURCES = diagn.qrc
