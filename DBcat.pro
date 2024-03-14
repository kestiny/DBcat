QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    appconfig.cpp \
    operation/syntaxparser.cpp \
    ui/commenthighlighter.cpp \
    ui/hosttreewidget.cpp \
    main.cpp \
    ui/dbcat.cpp \
    operation/mysqloperator.cpp \
    regexpconstexpr.cpp \
    ui/messagebox.cpp \
    ui/newhost.cpp \
    ui/sqleditor.cpp \
    ui/sqlresultcontroll.cpp \
    ui/codeeditor.cpp

HEADERS += \
    appconfig.h \
    operation/syntaxparser.h \
    ui/commenthighlighter.h \
    ui/dbcat.h \
    dbtype.h \
    ui/hosttreewidget.h \
    operation/idboperator.h \
    operation/mysqloperator.h \
    regexpconstexpr.h \
    ui/messagebox.h \
    ui/newhost.h \
    ui/sqleditor.h \
    ui/sqlresultcontroll.h \
    ui/codeeditor.h

FORMS += \
    ui/dbcat.ui \
    ui/messagebox.ui \
    ui/newhost.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_FILE += res.rc

DISTFILES += \
    res.rc

RESOURCES += \
    resource.qrc

TRANSLATIONS = languages/lang_zh.ts \
                 languages/lang_en.ts
