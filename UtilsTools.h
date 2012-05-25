#ifndef UTILSTOOLS_H
#define UTILSTOOLS_H

#include <AM_LauncherClient.h>
#include <TAPI_ACCE_Client.h>

#include <ZMessageDlg.h>
#include <ZNoticeDlg.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <iostream>

#include "myDef.h"
//#include "ZTaskItem.h"


QString readFileLine(const QString & fileName); 

bool readLineConfig(QString &file, QString &key, QString &type, QString &dest);
bool setLineConfig(QString &file, QString &key, QString &type, QString &value);
bool deleteKeyLine(QString &file, QString &deleteMark);

//QString ctr(const char*ChineseString);
QString getAppDir();
QString getIMEI();
QString getIMSI(); 
QString getGname(int step = 255); //make a uid 
								 //need set differet step each time

unsigned int getDirSize(const char* path);
QString size2string(unsigned long size);

int showQ(const QString &title, const QString &text, int type = 0);
void showNotify(const QString &title, const QString &text, int type = 0, int time = 1);

bool isValidFileName(QString &fileName);

//void initMgxConf( QString &file);



#endif //UTILSTOOLS_H

