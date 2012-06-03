

#include <qcopchannel_qws.h>
#include <qdir.h>

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "lng.h"
#include "TaskListBox.h"
#include "cfg.h"
#include "TaskThread.h"
#include "myDef.h"
#include "megabox.h"
#include "source.h"
#include "codeComp.h"

extern Settings *cfg;
extern megabox *gui;
//extern MyApplication *app;
extern Lng *lng;
extern CDCP *codeComp;

TaskListBox::TaskListBox(const QString& type, QWidget* parent, WFlags f)
    : ZListBox(type, parent, 0)
{
    //ListBox = new ZListBox
#ifdef RAISE_PHONE
    pidPhone = 0;
#endif
    pidProc = 0;
    gItemNum = 0;

    connect ( this, SIGNAL ( highlighted ( int ) ), this, SLOT ( itemHighlighted ( int ) ) );
    connect ( this, SIGNAL ( selected ( int ) ), this, SLOT ( itemSelected ( int ) ) );
}

TaskListBox::~TaskListBox()
{
#ifdef DEBUG
    qDebug("===== delete task list box \n");
#endif
}

void TaskListBox::buildTaskList(bool isShowAll)
{
    //clear();
#ifdef DEBUG_APP_LIST
    qDebug("add task list \n");
#endif

    static DIR *dir;// = NULL;
    struct dirent *entry;// = NULL;
    char *name;
    //int n;
    //char buf[1024];
    //FILE *fp;
    int pid;
    char *cmd;

    //QString procCmd;

    dir = opendir("/proc");
    if(!dir)
        qDebug("Can't open /proc \n");

    for(;;)
    {
        cmd = 0;

        if((entry = readdir(dir)) == NULL)
        {
            closedir(dir);
            dir = 0;
#ifdef DEBUG
            qDebug("Get runing proc: End\n");
#endif
            break;
        }
        name = entry->d_name;
        if (!(*name >= '0' && *name <= '9'))
            continue;

        pid = atoi(name);

        if(isShowAll == true)
        {
            addAllPid2cmd(pid);
        }
        else
        {
#ifdef ES_EVENT
            if ( ESCopChannel::isRegistered( QCString("EZX/Application/" + QString::number(pid)) ) )
#else
            if ( QCopChannel::isRegistered( QCString("EZX/Application/" + QString::number(pid)) ) )
#endif
            {
                addPid2cmd(pid);
            }
        }
    }
}

void TaskListBox::RefreshTaskList(bool isShowAll)
{
    clear();
    if(isShowAll == true) buildTaskList(true);
    else buildTaskList(false);
}

void TaskListBox::showNativeIcon()
{
#ifdef DEBUG_APP_LIST
    qDebug("=== nativ replace icon ===\n");
#endif
    int i = 0;
    ITEMTYPE type;
    QString cmd = QString::null;

    //QPixmap icon;
    for(i=0; i<(int)count(); i++)
    {
        ZTaskItem* pItem = (ZTaskItem*)item ( i );
        type = pItem->getType();
        if(type == ITEM_NATIV || type == ITEM_SPECIAL)
        {
            cmd = pItem->getOriName();
            addNativeIcon(i, type, cmd);
        }
    }
#ifdef DEBUG_APP_LIST
    qDebug("=== nativ icon replace ok ===\n");
#endif
}

QString TaskListBox::getCmd(int pid)
{
    QString scmd = QString::null;
    char *name;
    char buf[1024];
    char *cmd;
    int n;
    FILE *fp;
    cmd = 0;

    if((fp = fopen(QCString("/proc/"+QString::number(pid)+"/cmdline"), "r")) != NULL)
    {
        if((n=fread(buf, 1, sizeof(buf)-1, fp)) > 0)
        {
            //cmd = 0;
            if(buf[n-1]=='\n')
                buf[--n] = 0;
            name = buf;
            while(n)
            {
                if(((unsigned char)*name) < ' ')
                    *name = ' ';
                name++;
                n--;
            }
            *name = 0;
            if(buf[0])
                cmd = strdup(buf);
        }
        fclose(fp);

        if (cmd != 0 && cmd[0] != 0 )
        {
            scmd = QString(cmd);
            scmd.stripWhiteSpace();
        }
        else
            scmd = "ERROR";
    }
    else
        scmd = "ERROR";

    return scmd;
}

void TaskListBox::kvmToInfo()
{
#ifdef DEBUG_APP_LIST
    qDebug("===== kvm to info =====\n");
#endif
    int i = 0;
    ITEMTYPE type;
    QString cmd = QString::null;

    for (i=0; i<(int)count(); i++)
    {
        ZTaskItem* pItem= (ZTaskItem*)item ( i );
        type = pItem->getType();
        if(type == ITEM_JAVA)
        {
            cmd = pItem->getOriName();
            addKvm2Info(i, cmd);
        }
    }
    //setCurrentItem(0);
#ifdef DEBUG_APP_LIST
    qDebug("===== java name: end repleace =======\n");
#endif
}
#ifdef NEW_JAVA_LIST
void TaskListBox::buildJavaListNew()
{
#ifdef DEBUG_APP_LIST
    qDebug("==== buildJavaListNew ======\n");
#endif
#ifndef RAISE_PHONE
    kvmStats=0;
#endif

    AM_AppLaunchedList runJavaList;

    AM_RESULT_CODE_T result = AM_LauncherClient::getAppStates(AM_AppLnk::JavaApp, runJavaList);
    if ( result == AM_RESULT_SUCCESS )
    {
#ifdef DEBUG_APP_LIST
        qDebug("==== Java list geted! ====\n");
        std::cout<<"===== Java in list"<<QString::number(runJavaList.count())<<std::endl;
#endif

        ZConfig cardCfg("/mmc/mmca1/.system/java/CardRegistry", false);
        ZConfig phCfg("/ezxlocal/download/appwrite/am/InstalledDB", false);

        QString name;
        QString icon;

        AM_AppLaunchedList::Iterator p;
        for (p = runJavaList.begin(); p != runJavaList.end(); p++)//++p)
        {
#ifndef RAISE_PHONE
            kvmStats=kvmStats||((*p).state&RUNNING_ON_FOREGROUND);
#endif
            name = cardCfg.readEntry ((*p).uid, "Name", "");
            if(name != "")
            {
                icon = cardCfg.readEntry((*p).uid, "Icon", "");
                if(icon = "")
                    icon = cardCfg.readEntry((*p).uid, "BigIcon", "");

                icon = cardCfg.readEntry((*p).uid, "Directory", "") + "/" +icon;
            }
            else
            {
                name = phCfg.readEntry ((*p).uid, "Name", "");
                icon = phCfg.readEntry((*p).uid, "Icon", "");
                if(icon = "")
                    icon = phCfg.readEntry((*p).uid, "BigIcon", "");

                icon = phCfg.readEntry((*p).uid, "Directory", "") + "/" +icon;
            }

            //QImage img = pm.convertToImage();
            //img = img.smoothScale(24, 24);
            //pm.convertFromImage(img);

            ZTaskItem* pItem = new ZTaskItem ( this, QString ( "%I%M" ) );
            if(cfg->cfg_showJavaIcon)
            {
                QPixmap pm;
                if(QFile::exists(icon)) pm.load( icon );
                else pm = iconReader.getIcon("java_midlet_pop.bmp");
                QImage img = pm.convertToImage();
                img = img.smoothScale(ICON_SMALL);
                pm.convertFromImage(img);
                pItem->setPixmap ( 0, pm );
            }
            pItem->appendSubItem ( 1, QString( name ), true );
            pItem->setType(ITEM_JAVA);
            pItem->setGname((*p).uid);
            insertItem ( pItem, 1, true );
        }
    }
#ifdef DEBUG_APP_LIST
    qDebug("=== buildJavaListNew: end ===\n");
#endif
}
#endif
#ifdef NEW_JAVA_LIST
void TaskListBox::updateJavaListNew()
{
#ifdef DEBUG_APP_LIST
    qDebug("==== updateJavaListNew ======\n");
#endif

    int i, j = this->count();
    QStringList itemList;
    QStringList::ConstIterator p;

    for (i=0; i<j; i++)
    {
#ifdef DEBUG_APP_LIST
        qDebug("cur item number is %d", i);
        qDebug("cur item total is %d", j);
        //#endif

        //printf("cur item number is %d\n", i);
        //printf("cur item total is %d\n", j);
#endif

        ZTaskItem* pItem= (ZTaskItem*)item ( i );
        if(pItem->getType() == ITEM_JAVA)
        {
#ifdef DEBUG_APP_LIST
            qDebug("cur item %d need to remove", i );
#endif
            itemList += QString::number(i);
        }
    }
    //qDebug("*** start to remove items ****");
    //printf("*** start to remove items ****\n");
    for (p = itemList.begin(); p != itemList.end(); p++ ) //++p
    {
        qDebug("delet items cur p is %d", QString(*p).toInt() );
        this->removeItem(atoi(*p));
    }

    buildJavaListNew();

#ifdef DEBUG_APP_LIST
    qDebug("==== updateJavaListNew  end ======\n");
#endif
}
#endif

void TaskListBox::updateTaskList()
{
#ifdef DEBUG_APP_LIST
    qDebug("=================================================");
    qDebug("============= updateTaskList start ===============");
#endif
    static DIR *dir;// = NULL;
    struct dirent *entry;// = NULL;
    char *name;
    int pid;
    char *cmd;
    int i, j = this->count();
    mCmdStrList = QString::null;
    refreshTmpList = QString::null;
    QStringList itemList;
    QStringList::ConstIterator p;

    //ResTool tool;
    for (i=0; i<j; i++)
    {
#ifdef DEBUG_APP_LIST
        qDebug("cur item number is %d", i);
        qDebug("cur item total is %d", j);
#endif
        ZTaskItem* pItem= (ZTaskItem*)item ( i );
#ifdef NEW_JAVA_LIST
        if(pItem->getType() == ITEM_JAVA)
            continue;
#endif
        pid = pItem->getPid();
        //QString path = "/proc/"+QString::number(pid);
        //QDir dir(path);
        //if(dir.exists() == false)
        if(!processIsRuning(pid))
        {
#ifdef DEBUG_APP_LIST
            qDebug("cur item %d need to remove", i );
#endif
            itemList += QString::number(i);
        }
        else
        {
            QString tmp = QString::number( pItem->getPid() )+pItem->getOriName()+";";
            mCmdStrList += tmp;
        }
    }
    qDebug("*** start to remove items ****");
    for (p = itemList.begin(); p != itemList.end(); p++) // ++p )
    {
        qDebug("delet items cur p is %d", QString(*p).toInt() );
        this->removeItem(atoi(*p));
    }
#ifdef DEBUG_APP_LIST
    qDebug("mCmdStrList is %s", (const char *)mCmdStrList.local8Bit() );
#endif

    dir = opendir("/proc");
    if(!dir) {
        qDebug("Can't open /proc \n");
        return;
    }

    for(;;)
    {
        cmd = 0;

        if((entry = readdir(dir)) == NULL)
        {
            closedir(dir);
            dir = 0;
#ifdef DEBUG
            qDebug("Get runing proc: End\n");
#endif
            break;
        }
        name = entry->d_name;
        if (!(*name >= '0' && *name <= '9'))
            continue;

        pid = atoi(name);
#ifdef ES_EVENT
        if ( ESCopChannel::isRegistered( QCString("EZX/Application/" + QString::number(pid)) ) )
#else
        if ( QCopChannel::isRegistered( QCString("EZX/Application/" + QString::number(pid)) ) )
#endif
        {
            updatePid2cmd(pid);
        }
    }

#ifdef DEBUG_APP_LIST
    qDebug("=========== updateTaskList end ===============\n");
#endif
}

void TaskListBox::updateNativeIcon()
{
#ifdef DEBUG_APP_LIST
    qDebug("============= updateNativeIcon() start ===============\n");
#endif
    int i;
    QPixmap pm;
    ITEMTYPE type;
    QString cmd = QString::null;
    for (i=0; i<(int)count(); i++)
    {
        ZTaskItem* pItem= (ZTaskItem*)item ( i );
        pm = pItem->getPixmap(0);
        if( pm.isNull() == true)
        {
#ifdef DEBUG_APP_LIST
            qDebug("cur item number %d need to update", i);
#endif
            type = pItem->getType();
            cmd = pItem->getOriName();
            addNativeIcon(i, type, cmd);
        }
    }
#ifdef DEBUG_APP_LIST
    qDebug("============= updateNativeIcon() end ===============\n");
#endif
}

void TaskListBox::updateKvmToInfo()
{
#ifdef DEBUG_APP_LIST
    qDebug("============= updateKvmToInfo() start ===============\n");
#endif
    int i;
    QPixmap pm;
    ITEMTYPE type;
    QString cmd = QString::null;
    for (i=0; i<(int)count(); i++)
    {
        ZTaskItem* pItem= (ZTaskItem*)item ( i );
        type = pItem->getType();
        if(type == ITEM_JAVA)
        {
            pm = pItem->getPixmap(0);
            cmd = pItem->getOriName();
            if( pm.isNull() == true || cmd.find("kvm") > -1)
            {
#ifdef DEBUG_APP_LIST
                qDebug("cur item number %d need to update", i);
#endif
                cmd = pItem->getOriName();
#ifdef DEBUG_APP_LIST
                qDebug("cmd is %s", (const char*)cmd.local8Bit() );
#endif
                addKvm2Info(i, cmd);
            }
        }
    }
#ifdef DEBUG_APP_LIST
    qDebug("============= updateKvmToInfo() end ===============\n");
#endif
}

void TaskListBox::addPid2cmd(int pid)
{
    pid2cmdThread *pid2cmd = new pid2cmdThread(pid);
    connect(pid2cmd, SIGNAL(pid2cmdObj(int, QString, QString)), this, SLOT(slotAddPid2cmd(int, QString, QString)) );
    pid2cmd->start();
    if( pid2cmd->wait() )
        delete pid2cmd;
    else
        qDebug("======== try to delete pid to cmd thread %d, delete failed ==========", pid);
}

void TaskListBox::addAllPid2cmd(int pid)
{
    pid2cmdThread *pid2cmd = new pid2cmdThread(pid);
    connect(pid2cmd, SIGNAL(pid2cmdObj(int, QString, QString)), this, SLOT(slotAddAllPid2cmd(int, QString, QString)) );
    pid2cmd->start();
    if( pid2cmd->wait() )
        delete pid2cmd;
    else
        qDebug("======== try to delete add all pid to cmd thread %d, delete failed ==========", pid);
}

void TaskListBox::updatePid2cmd(int pid)
{
    pid2cmdThread *pid2cmd = new pid2cmdThread(pid);
    connect(pid2cmd, SIGNAL(pid2cmdObj(int, QString, QString)), this, SLOT(slotUpdatePid2cmd(int, QString, QString)) );
    pid2cmd->start();
    if( pid2cmd->wait() )
        delete pid2cmd;
    else
        qDebug("======== try to delete update pid to cmd thread %d, delete failed ==========", pid);
}

void TaskListBox::addNativeIcon(int itemIndex, ITEMTYPE t, QString cmdName)
{
    nativeIconThread *pThread = new nativeIconThread(itemIndex, t, cmdName);
    connect(pThread, SIGNAL(nativeIconObj(int, QImage)), this, SLOT( slotAddNativeIcon(int, QImage)) );
    pThread->start();
    if( pThread->wait() )
        delete pThread;
    else
        qDebug("======== try to delete nativeIconThread itemIndex %d, delete failed ==========", itemIndex);
}
void TaskListBox::addKvm2Info(int itemIndex, QString cmdName)
{
    kvmToInfoThread *pThread = new kvmToInfoThread(itemIndex, cmdName);
    connect(pThread, SIGNAL( kvmToInfoObj(int, QString, QImage) ), this, SLOT( slotAddKvm2Info(int, QString, QImage) ) );
    pThread->start();
    if( pThread->wait() )
        delete pThread;
    else
        qDebug("======== try to delete kvmToInfoThread itemIndex %d, delete failed ==========", itemIndex);
}

void TaskListBox::slotAddPid2cmd(int pid, QString pCmd, QString pName)
{
#ifdef DEBUG_APP_LIST
    qDebug("========= slot add app 2 cmd %d =========", pid);
#endif
    if(!procFilter(pCmd) )
    {
#ifdef SCREEN_V
        ZTaskItem* pItem = new ZTaskItem ( this, QString ( "%I%M130%M" ) );
#else
        ZTaskItem* pItem = new ZTaskItem ( this, QString ( "%I%M190%M" ) );
#endif
        pItem->setOriName(pCmd);
        QString text;
        if(pName.find("phone") > -1)
        {
#ifdef RAISE_PHONE
            pidPhone = pid;
#endif
            pItem->setType( ITEM_SPECIAL );
#ifndef VE66
            text = lng->tr("Minimized All Apps");
#else
            text = pName;
#endif
        }
        else if( (pName.find("flowplayer") > -1) || (pName.find("xp") > -1) )
        {
            pItem->setType( ITEM_SPECIAL );
#ifndef VE66
            text = lng->tr("Menu");
#else
            text = pName;
#endif
        }
        else if(pName.find("messaging") > -1)
        {
            pItem->setType( ITEM_SPECIAL );
            text = lng->tr("messaging");
        }
        else if(pName.find("RokrToolsPro") > -1)
        {
            pidProc = pid;
            pItem->setType( ITEM_SPECIAL );
            text = pName;
        }
        else if (pName.find("kvm") > -1)
        {
            pItem->setType(ITEM_JAVA);
#ifdef NEW_JAVA_LIST
            text = lng->tr("Java Machine");
            pItem->setType( ITEM_SPECIAL );
#else
            text = pName;
#endif
        }
        else
        {
            pItem->setType( ITEM_NATIV );
            text = pName;
        }
        pItem->setPid( (uint)pid );
        pItem->appendSubItem(1, text, false, SETTINGITEM_REGION_A);
        pItem->appendSubItem(2, QString::number(pid), false, SETTINGITEM_REGION_A);
#ifdef DEBUG_APP_LIST
        std::cout<<"app text is "<<text<<std::endl;
        std::cout<<"app name is "<<pName<<std::endl;
#endif
        insertItem(pItem);
    }
    else
        qDebug("cur pid : %d is in filter, no add¡£", pid);
#ifdef DEBUG_APP_LIST
    qDebug("========= slot add app 2 cmd %d  end =========", pid);
#endif
}
void TaskListBox::slotAddAllPid2cmd(int pid, QString pCmd, QString pName)
{
#ifdef DEBUG_APP_LIST
    qDebug("========= slot add all app 2 cmd %d =========", pid);
#endif

#ifdef SCREEN_V
    ZTaskItem* pItem = new ZTaskItem ( this, QString ( "%M50%M" ) );
#else
    ZTaskItem* pItem = new ZTaskItem ( this, QString ( "%M70%M" ) );
#endif
    pItem->setOriName(pCmd);
    QString text = pCmd;//pName;
    pItem->setPid( (uint)pid );
    pItem->appendSubItem(1, text, false, SETTINGITEM_REGION_A);
    pItem->appendSubItem(0, QString::number(pid), false, SETTINGITEM_REGION_A);
#ifdef DEBUG_APP_LIST
    std::cout<<"app text is "<<text<<std::endl;
    std::cout<<"app name is "<<pName<<std::endl;
#endif
    insertItem(pItem);
#ifdef DEBUG_APP_LIST
    qDebug("========= slot add all app 2 cmd %d  end =========", pid);
#endif
}
void TaskListBox::slotUpdatePid2cmd(int pid, QString pCmd, QString pName)
{
    if(!procFilter(pCmd) )
    {
        QString tmp = QString::number(pid)+pCmd+";";
        if(mCmdStrList.find( tmp ) <= -1 )
        {
#ifdef DEBUG_APP_LIST
            qDebug("slot update app 2 cmd, add app,  pid %d", pid);
#endif
            //refreshTmpList += QString::number(pid);
            addPid2cmd(pid);
        }
    }
#ifdef DEBUG_APP_LIST
    else
    {
        qDebug("cur pid %d is in filter ", pid);
    }
#endif
#ifdef DEBUG_APP_LIST
    qDebug("========= slot update app 2 cmd %d  end =========", pid);
#endif
}

void TaskListBox::slotAddNativeIcon(int itemIndex, QImage iconImg)
{
#ifdef DEBUG_APP_LIST
    qDebug("===== slot to add native icon ,item index %d ======", itemIndex);
#endif
    QPixmap pm;
    ZTaskItem* pItem = (ZTaskItem*)item(itemIndex);
    pm.convertFromImage(iconImg);
    pItem->setPixmap(0, pm);
#ifdef DEBUG_APP_LIST
    qDebug("===== slot to add native icon ,item index %d end ======", itemIndex);
#endif
}

void TaskListBox::slotAddKvm2Info(int itemIndex, QString jname, QImage iconImg)
{
#ifdef DEBUG_APP_LIST
    qDebug("===== slot to add kvm to info ,item index %d ======", itemIndex);
#endif
    ZTaskItem* pItem = (ZTaskItem*)item(itemIndex);

    if(cfg->cfg_showJavaName)
        pItem->setSubItem ( 1, 0, jname);

    if(cfg->cfg_showJavaIcon)
    {
        QPixmap pm;
        pm.convertFromImage(iconImg);
        pItem->setPixmap(0, pm);
    }
#ifdef DEBUG_APP_LIST
    qDebug("===== slot to add kvm to info ,item index %d  end ======", itemIndex);
#endif
}
bool TaskListBox::procFilter(QString &filterStr)
{
    if(/*codeCmp()*/codeComp->cpcp() == false)
        return false;
    else
    {
        if(cfg->cfg_procFilter)
        {
            if( cfg->procFilterStr.find(filterStr) > -1 )
                return true;
        }
        return false;
    }
}

void TaskListBox::itemHighlighted(int index)
{
    //this->setItemType("%I%M");

    //printf("cur g item num %d\n", gItemNum);
    //printf("cur index %d\n", index);
    //if(gItemNum != index)
    //{
    //	ZTaskItem* pItem= (ZTaskItem*)item (gItemNum);
    //	pItem->deleteSubItem(1, 1);
    //}
    ZTaskItem* pItem= (ZTaskItem*)item (index);
    hOriName = pItem->getOriName();
    hUid = pItem->getGname();
    hType = pItem->getType();
    hPid = pItem->getPid();

    //pItem->appendSubItem(1, hOriName, false, SETTINGITEM_REGION_B);
    //gItemNum = index;

    emit taskHighlighted(index);
}

void TaskListBox::itemSelected(int index)
{
    ZTaskItem* pItem= (ZTaskItem*)item (index);
    sOriName = pItem->getOriName();
    sUid = pItem->getGname();
    sType = pItem->getType();
    sPid = pItem->getPid();

    emit taskSelected(index);
    emit taskSelected();
}

QString TaskListBox::getHlOriName()
{
    return hOriName;
}

QString TaskListBox::getHlUid()
{
    return hUid;
}

ITEMTYPE TaskListBox::getHlType()
{
    return hType;
}

uint TaskListBox::getHlPid()
{
    return hPid;
}

QString TaskListBox::getSelOriName()
{
    return sOriName;
}

QString TaskListBox::getSelUid()
{
    return sUid;
}

ITEMTYPE TaskListBox::getSelType()
{
    return sType;
}

uint TaskListBox::getSelPid()
{
    return sPid;
}
#ifdef RAISE_PHONE
int TaskListBox::getPidPhone()
{
    return pidPhone;
}
#else
bool TaskListBox::getKvmStats()
{
    return kvmStats;
}
#endif
int TaskListBox::getPidProc()
{
    return pidProc;
}

/*int TaskListBox::getListCount()
{
    return this->count();
}
*/
