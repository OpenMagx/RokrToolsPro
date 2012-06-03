
#include "UtilsTaskBase.h"


void sendMes(int pid, QString mes)
{
#ifdef DEBUG_TASK_MESSAGE
    qDebug("pid is %d\n", pid);
#endif

    //#ifdef ES_EVENT


    // TODO for VE66/EM35:
    // Exemple event for show app:
    // Data size: 4
    // Data: link to " 00;00;00;04;.... "
    // Map:
    // Qt-Message = raise()
    // Source-Domains = 12
    // Source-PID = 360

    //char data[4] = {0,0,0,4};

    //#ifdef DEBUG_TASK_MESSAGE
    //qDebug("=== es event to show apps ===\n");
    //#endif

    //ES_Event evt( QCString("EZX/Application/"+QString::number(pid)) );//, 0/*&data*/, 4 );

    //evt.GetPropMap()["Qt-Message"] = mes;
    //evt.GetPropMap()["Source-Domains"] = "12";
    //evt.GetPropMap()["Source-PID"] = QString::number( getpid() );
    //evt.GetPropMap()["Source-PID "] = QString::number(pidPhone);

    //int res = ES_EventPublisher::publish( evt );

    //if (res != ES_RESULT_SUCCESS)
    //	qDebug("Error send message! \n");
    //#else

    //#ifdef DEBUG_TASK_MESSAGE
    //qDebug("=== normal to show apps ===\n");
    //#endif

#ifdef ES_EVENT
    ESCopChannel::send( QCString("EZX/Application/"+QString::number(pid)), QCString ( mes ), QByteArray(), false );
#else
    QCopChannel::send( QCString("EZX/Application/"+QString::number(pid)), QCString ( mes ), QByteArray() );
#endif
}
/*void qtSend(int pid, QString mes)
{
    qDebug("pid is %d\n", pid);
	#ifdef DEBUG_TASK_MESSAGE
	qDebug("=== normal to show apps ===\n");
	#endif
	QCopChannel::send( QCString("EZX/Application/"+QString::number(pid)), QCString ( mes ), QByteArray() );
}*/

void sendReturnToIdle()
{
#ifdef DEBUG_TASK_MESSAGE
    qDebug("=== for minimized java , sendReturnToIdle===\n");
#endif

    QByteArray data;
    QDataStream stream( data, IO_WriteOnly );
#ifdef VE66
    stream << (int)RETURN_IDLE_BY_SLIDER;
#else
    stream << (int)RETURN_IDLE_BY_FLIP;
#endif
    QCopChannel::send("EZX/System", QCString ( "ReturnToIdle(int)" ), data );
}
#ifdef NEW_JAVA_LIST
void javaToTop( QString uid )
{
#ifdef DEBUG_TASK_MESSAGE
    qDebug("=== javaToTop () ===\n");
#endif
    if ( uid.isEmpty() ) return;

    AM_LauncherClient::launchAppObject( uid );
}
#endif

void launchAppObject( QString uid )
{
#ifdef DEBUG_TASK_MESSAGE
    qDebug(" launch app obj form uid \n");
#endif
    if ( uid.isEmpty() ) return;

    AM_LauncherClient::launchAppObject( uid );
}
/*
void launchAppObject( QString &uid )
{
    #ifdef DEBUG_TASK_MESSAGE
	qDebug(" launch app obj form uid \n");
	#endif
	AM_LauncherClient::launchAppObject( uid );
}
*/
bool killProc(int pid)
{
#ifdef DEBUG_TASK_MESSAGE
    qDebug("kill proc %d\n", pid);
#endif
    if( ( ::kill(pid, SIGKILL ) < 0 ) )   return false;

    return true;
}

bool processIsRuning(int pid)
{
    QString fn;
    QString s = readFileLine(fn.sprintf("/proc/%d/cmdline", pid));
    if(s.isEmpty())
        return false;
    else
        return true;
}

void pidQuit(int pid)
{
    sendMes( pid, "quickQuit()" );
}

void pidToTop(int pid)
{
    sendMes( pid, "raise()" );
}

/*void qtPidToTop(int pid)
{
    qtSend( pid, "raise()" );
}*/

void goToIdle(int pidPhone, bool isForShowJava)
{
    if( isForShowJava )
        sendReturnToIdle();

    sendMes( pidPhone, "raise()" );
}

/*
void chkJavaLoaded(const QString &javaExecID, bool &isJavaLoaded, int &returnedJavaPid)
{
	FILE *f;
	if( (f = popen("ps", "r")) == NULL ) { //system调用
		printf("popen() error!\n");
		isJavaLoaded = false;
	}

	char buf[512];

	while(fgets(buf, sizeof buf, f))//读文件
	{
		QString line(buf);
		if( line.find(javaExecID) > -1 && line.find("/usr/SYSjava/kvm") > -1 ) {
			//cout<<"cur line :  "<<line<<endl;
			line = line.stripWhiteSpace();
			printf("cur line : %s\n", (const char*)line.local8Bit() );
			isJavaLoaded = true;
			QStringList lineList = QStringList::split("", line.stripWhiteSpace() );
			//returnedJavaPid = lineList[0].toInt();

			printf("===== %s", (const char*)lineList[0].local8Bit() );
			printf(" ======  java pid %d \n", returnedJavaPid);
			break;
		}
	}
	pclose(f);
}
void loadJava(const QString &gname, bool isSD, bool isMult, appListBox *lb)
{
	#ifdef DEBUG_TASK_MESSAGE
	cout<<"load java  "<<gname<<endl;
	#endif

	#ifndef NEW_JAVA_LIST
	QString exec;
	if(isSD == true) {
		ZConfig cfg(SD_INSTALL_DB);
		#ifdef ZN5
		exec = cfg.readEntry(gname, "JavaId", "");
		#else
		exec = cfg.readEntry(gname, "ExecId", "");
		#endif
	} else {
		ZConfig cfg(PHONE_INSTALL_DB);
		#ifdef ZN5
		exec = cfg.readEntry(gname, "JavaId", "");
		#else
		exec = cfg.readEntry(gname, "ExecId", "");
		#endif
	}
	if ( !isMult ) {
		bool isLoaded = false;
		int pid = -1;
		ZTaskItem* pItem = NULL;
		QString line = "";
		for(int i=0; i<=lb->count(); i++ ) {
			pItem= (ZTaskItem*)lb->item(i);
			line = pItem->getOriName();
			if( line != "" ) {
				if( line.find(javaExecID) > -1 && line.find("/usr/SYSjava/kvm") > -1 ) {
					isLoaded = true;
					pid = pItem->getPid();
					break;
				}
			}
		}

		if( isLoaded && pid > -1 )
			pidToTop(pid);
		else
			system( QString("exec /usr/SYSjava/kvm -launch %1 &").arg(exec) );
	} else {
		system( QString("exec /usr/SYSjava/kvm -launch %1 &").arg(exec) );
	#endif
	#ifdef NEW_JAVA_LIST
		launchAppObject( gname );
	#endif
	#ifndef NEW_JAVA_LIST
	}
	#endif

}
void loadObj(const QString &gname, ITEMTYPE t, bool isMult, appListBox *lb)//, QString &existAppInProcListStr)
{
	#ifdef DEBUG_TASK_MESSAGE
	cout<<"loadObj  "<<gname<<endl;
	#endif

	if(t == ITEM_FAV_OBJ)
	{
	    ZConfig cfg(FAV_OBJ_PATH);
		int type = cfg.readNumEntry(gname, "AppType", -1);
		if(type == -1) return;
		#if defined(E8) || defined(ZN5)
		if(type == 1)
		{
			QString exec = cfg.readEntry(gname, "ExecId", "");
			printf("\n\n\n\n\n ================ load java \n");
			printf("exec is %s \n", (const char*)exec.local8Bit() );
			if ( isMult ) {
				system( QString("exec /usr/SYSjava/kvm -launch %1 &").arg(exec) );
			} else {
				bool isLoaded = false;
				int pid = -1;
				ZTaskItem* pItem = NULL;
				QString line = "";
				for(int i=0; i<=lb->count(); i++ ) {
					pItem= (ZTaskItem*)lb->item(i);
					line = pItem->getOriName();
					if( line != "" ) {
						if( line.find(javaExecID) > -1 && line.find("/usr/SYSjava/kvm") > -1 ) {
							isLoaded = true;
							pid = pItem->getPid();
							break;
						}
					}
				}
				if( isLoaded && pid > -1 )
					pidToTop(pid);
				else
					system( QString("exec /usr/SYSjava/kvm -launch %1 &").arg(exec) );
			}
		}
		else
		#endif
		launchAppObject(gname);
	}
	else if(t == ITEM_MGX_OBJ)
	{
	    ZConfig cfg(MGX_OBJ_PATH);
		//char buf[1024];
		QString dir = cfg.readEntry(gname, "Directory", "");
		QString exec = cfg.readEntry(gname, "ExecId", "");
		//sprintf(buf, "exec '%s/%s' &", (const char *)dir.local8Bit(), (const char *)exec.local8Bit() );
		//system(buf);
		system(QString("exec '%1/%2' &").arg(dir).arg(exec) );
	}
	else if(t == ITEM_MPKG_OBJ)
	    launchAppObject(gname);

	else return;
}
*/
