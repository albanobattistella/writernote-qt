QT       += core
QT       += gui
QT       += widgets
QT       += multimedia
QT       += printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DESTDIR = build
RESOURCES     = writernote.qrc
CONFIG += c++11

#for glibconfig.h
INCLUDEPATH += /usr/include/glib-2.0/ /usr/lib/x86_64-linux-gnu/glib-2.0/include/

DEFINES += "STAMPA"
DEFINES += "NUMEROPIXELPAGINA=1200"
DEFINES += "NUMEROPIXELORIZZONALI=700"

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/areyousure.cpp \
    src/audioplay/aggiornotastiriascolto.cpp \
    src/audioplay/audiovolume.cpp \
    src/audioplay/cambiostatoplayer.cpp \
    src/audioplay/enableriascolto.cpp \
    src/audioplay/playbottonfunction.cpp \
    src/audioplay/riascoltoaudioprogressivo.cpp \
    src/audioplay/slidertimechange.cpp \
    src/audioplay/updatestatusbar_riascolto.cpp \
    src/audioplay/valuechangeslidervolume.cpp \
    src/audiosetting/audioqualityoption.cpp \
    src/audiorecord/displayErrorMessage.cpp \
    src/audiorecord/getbufferlevels.cpp \
    src/audiosetting/loadqualita.cpp \
    src/audiosetting/savequalita.cpp \
    src/chartoint.cpp \
    src/closeevent_file.cpp \
    src/currenttitle/audiototext.cpp \
    src/currenttitle/checksimilecopybook.cpp \
    src/currenttitle/currenttitle_class.cpp \
    src/currenttitle/deleteaudio.cpp \
    src/currenttitle/deletecopybook.cpp \
    src/currenttitle/newcopybook.cpp \
    src/currenttitle/redolist.cpp \
    src/currenttitle/renamefile.cpp \
    src/currenttitle/rightclicklistcopybook.cpp \
    src/datamanage/check1.cpp \
    src/datamanage/checkinserimento.cpp \
    src/datamanage/eliminazioneNFrasi.cpp \
    src/datamanage/spacchettamento.cpp \
    src/datamanage/stoprecording.cpp \
    src/datawrite/colortoint.cpp \
    src/datawrite/compressfile.cpp \
    src/datawrite/loadbinario.cpp \
    src/datawrite/qfilechoose.cpp \
    src/datawrite/renamefile_f_zip.cpp \
    src/datawrite/salvabinario.cpp \
    src/datawrite/savefile.cpp \
    src/dialog_critic.cpp \
    src/indice_class.cpp \
    src/main.cpp \
    src/datawrite/xmlstruct.cpp \
    src/mainwindow.cpp \
    src/newcopybook_.cpp \
    src/savecopybook.cpp \
    src/self_class.cpp \
    src/setting_ui.cpp \
    src/stringgenerator.cpp \
    src/audiorecord/audiolevel.cpp \
    src/style/abilitazioneinput.cpp \
    src/style/bulletfile.cpp \
    src/style/insertimage.cpp \
    src/style/inserttable.cpp \
    src/style/inserttable_class.cpp \
    src/style/main_style.cpp \
    src/audioplay/pausariascolto.cpp \
    src/audioplay/stopriascolto.cpp \
    src/audioplay/aggiornamentostatusbar.cpp \
    src/style/bold.cpp \
    src/audioplay/main_audioplay.cpp \
    src/style/print_action.cpp \
    src/touch/datastruct/datastruct.cpp \
    src/touch/datastruct/loadpixel.cpp \
    src/touch/datastruct/needtocreatenew.cpp \
    src/touch/event/eventmethod.cpp \
    src/touch/event/itspossibletoscroll.cpp \
    src/touch/event/mouseevent.cpp \
    src/touch/event/moving_f.cpp \
    src/touch/eventmainwindow.cpp \
    src/touch/initpixmap.cpp \
    src/touch/paintevent/disegnofoglio.cpp \
    src/touch/paintevent/gomma.cpp \
    src/touch/paintevent/paintevent.cpp \
    src/touch/paintevent/zoomin.cpp \
    src/touch/paintevent/zoomon.cpp \
    src/touch/riascolto/riascolto.cpp \
    src/touch/settingmethod.cpp \
    src/touch/square/move.cpp \
    src/touch/square/square.cpp \
    src/touch/tabletapplication.cpp \
    src/touch/tabletcanvas.cpp \
    src/touch/tabletevent.cpp \
    src/touch/updatelist.cpp \
    src/update_list_copybook.cpp \
    src/videocompress/compressivideo.cpp \
    src/videocompress/enable_videocompress.cpp \
    src/videocompress/importvideo_functionmainwindow.cpp \
    src/videocompress/videocompress_ui.cpp


HEADERS += \
    src/areyousure.h \
    src/audioplay/aggiornotastiriascolto.h \
    src/audioplay/cambioiconariascolto.h \
    src/audiosetting/audioqualityoption.h \
    src/audiorecord/getbufferlevels.h \
    src/audiosetting/definition.h \
    src/audiosetting/loadqualita.h \
    src/audiosetting/savequalita.h \
    src/currenttitle/audiototext.h \
    src/currenttitle/checksimilecopybook.h \
    src/currenttitle/deleteaudio.h \
    src/currenttitle/deletecopybook.h \
    src/currenttitle/redolist.h \
    src/currenttitle/renamefile.h \
    src/datamanage/check1.h \
    src/datamanage/checkinserimento.h \
    src/datamanage/spacchettamento.h \
    src/datawrite/colortoint.h \
    src/datawrite/renamefile_f_zip.h \
    src/datawrite/sizedata.h \
    src/dialog_critic.h \
    src/chartoint.h \
    src/newcopybook_.h \
    src/setting_ui.h \
    src/style/abilitazioneinput.h \
    src/touch/datastruct/datastruct.h \
    src/touch/event/itspossibletoscroll.h \
    src/touch/method/methoddefinition.h \
    src/touch/square/square.h \
    src/touch/tabletapplication.h \
    src//touch/tabletcanvas.h \
    src/update_list_copybook.h \
    src/currenttitle/currenttitle_class.h \
    src/datawrite/qfilechoose.h \
    src/datawrite/savefile.h \
    src/indice_class.h \
    src/mainwindow.h \
    src/savecopybook.h \
    src/self_class.h \
    src/audiorecord/audiolevel.h\
    src/datawrite/xmlstruct.h \
    src/style/inserttable_class.h \
    src/videocompress/compressivideo.h \
    src/videocompress/enable_videocompress.h \
    src/videocompress/videocompress_ui.h

FORMS += \
    src/audiosetting/audioqualityoption.ui \
    src/mainwindow.ui \
    src/style/inserttable_class.ui \
    src/videocompress/videocompress_ui.ui \

# for the filesystem
LIBS += -lstdc++fs -lglib-2.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += libzip

# testing for windows support and compile
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../mingw64/lib/ -lzip
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../mingw64/lib/ -lzip

win32:{
    INCLUDEPATH += $$PWD/../../../mingw64/include
    DEPENDPATH += $$PWD/../../../mingw64/include
}
