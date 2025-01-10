QT       += core gui
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    customtoolbutton.cpp \
    levelsystem.cpp \
    Quiz/quizcard.cpp \
    Quiz/quizcardbasicreversed.cpp \
    Quiz/quizcardbasictypeanswer.cpp \
    Quiz/quizcardmultiplechoice.cpp \
    Quiz/quizdeck.cpp \
    Quiz/quizsession.cpp \
    kalender.cpp \
    kurs.cpp \
    main.cpp \
    homepage.cpp \
    notenliste.cpp \
    notizen.cpp \
    Quiz/quiz.cpp \
    nutzer.cpp \
    notizenelement.cpp \
    notizen_backend.cpp \
    qdynamicbutton.cpp

HEADERS += \
    customtoolbutton.h \
    levelsystem.h \
    Quiz/quizcard.h \
    Quiz/quizcardbasicreversed.h \
    Quiz/quizcardbasictypeanswer.h \
    Quiz/quizcardmultiplechoice.h \
    Quiz/quizdeck.h \
    Quiz/quizsession.h \
    homepage.h \
    kalender.h \
    kurs.h \
    notenliste.h \
    notizen.h \
    Quiz/quiz.h \
    nutzer.h \
    notizenelement.h \
    notizen_backend.h \
    qdynamicbutton.h

FORMS += \
    homepage.ui \
    kalender.ui \
    notenliste.ui \
    notizen.ui \
    Quiz/quiz.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
	.gitignore

RESOURCES += \
    resources.qrc
