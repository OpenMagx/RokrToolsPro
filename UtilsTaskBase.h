#ifndef _UTILSTASKBASE_H
#define _UTILSTASKBASE_H

#include <qcopchannel_qws.h>

#include <qstring.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

#include <AM_LauncherClient.h>

#include "myDef.h"
#include "UtilsEnum.h"
#include "UtilsTools.h"
//#include "source.h"

#ifdef ES_EVENT
#include <ESCopChannel.h>
#include <ES_SubscriberAPI.h>
#endif


void sendMes(int pid, QString mes);


//void qtSend(int pid, QString mes);
//void qtPidToTop(int pid);

void sendReturnToIdle();  // send a qcopchannel message : "ReturnToIdle(int)" 
#ifdef NEW_JAVA_LIST
void javaToTop( QString uid );
#endif

void pidToTop(int pid);
void launchAppObject( QString uid );
//void launchAppObject( QString &uid );

void goToIdle(int pidPhone, bool isForShowJava = true); // just raise the "phone" app itself

void pidQuit(int pid);

//void chkJavaLoaded(const QString &javaExecID, bool &isJavaLoaded, int &returnedJavaPid);
//void loadObj(const QString &gname, ITEMTYPE t, bool isMult = true);//, TaskListBox *lb=0);//, QString &existAppInProcListStr = QString::null);
//void loadJava(const QString &gname, bool isSD, bool isMult = true);//, TaskListBox *lb=0);//, QString &existAppInProcListStr = QString::null);

bool killProc(int pid);
bool processIsRuning(int pid);

///////////////////////////

class AM_MainMenuInterface
{
  public:
    AM_MainMenuInterface();
    virtual ~AM_MainMenuInterface();

    int ShowMainMenu();
};


#endif //UTILSTASKBASE_H

