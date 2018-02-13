#include "dialog.h"

#include <QApplication>
#include <QTextCodec>




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setWindowIcon(QIcon("MCRClientAppUI.ico"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    QTextCodec *codec = QTextCodec::codecForName("utf8");
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);


    QString aDir = QCoreApplication::applicationDirPath();
    aDir += "/config.ini";

    QSettings *config = new QSettings(aDir,QSettings::IniFormat);

    int fontType = config->value("/system/fontType").toInt();

    if(fontType == 0){
       int fontSize = config->value("/system/fontSize").toInt();
     QFont font("wenquanyi",fontSize,QFont::Normal);
     a.setFont(font);
    }else if(fontType == 1){
       QFont font;
       font.setFamily(("unifont"));
       a.setFont(font);
    }

//    //libffmpeg
//    MyFfPlayer* player;

    Dialog w;

    //w.resize(1200,800);
  //  w.showMaximized();





    int windowX = w.config->value("/system/window_x").toInt();
    int windowY = w.config->value("/system/window_y").toInt();
    int windowWidth = w.config->value("/system/window_width").toInt();
    int windowHeight = w.config->value("/system/window_height").toInt();
  //  int showMaximized = w.config->value("/system/show_maximized").toInt();
    w.setGeometry(windowX,windowY,windowWidth,windowHeight);
//    if(showMaximized==1){
//        w.showMaximized();
//    }else{
//        w.showNormal();
//    }
      w.showMaximized();
///*创建二进制信号量*/
//playSwitchID =  semBCreate(SEM_Q_FIFO, SEM_FULL);

// /*运行任务tmcrClient和任务tffPlayer*/
// mcrClientID = taskSpawn("tSend1", 99, 0, STACK_SIZE, (FUNCPTR)w.player->refreshVideo, 0,0,0,0,0,0,0,0,0,0 );
// ffPlayerID = taskSpawn("tTake1", 99, 0, STACK_SIZE, (FUNCPTR)w.player->asyncDecodePacket, 0,0,0,0,0,0,0,0,0,0 );

    return a.exec();
}
