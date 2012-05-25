
#include <qcopchannel_qws.h>
#include "TaskThread.h"
#include "source.h"


pid2cmdThread::pid2cmdThread(int pid, QObject *parent)
    : QObject(parent)
{
    mPid = pid;
}
pid2cmdThread::~pid2cmdThread()
{
    #ifdef DEBUG_THREAD
	qDebug("pid2cmd thread  pid %d end\n", mPid);
	#endif
}
void pid2cmdThread::run()
{
  	char *name;
	int n;
	char buf[1024];
	FILE *fp;
	char *cmd = 0;
	QString procName;
	QString procCmd;  

	#ifdef SLOW_TEST
	sleep(1);
	#endif
	#ifdef DEBUG_THREAD
	qDebug("\n=======================================================");
	qDebug("pid2cmdThread thread start, cur pid is %d", mPid);
	#endif
	if((fp = fopen(QCString("/proc/"+QString::number(mPid)+"/cmdline"), "r")) == NULL) 
	{	
        emit pid2cmdFailed(mPid);
	    #ifdef DEBUG_THREAD
		qDebug("pid2cmd failed");
		#endif
	}
	else
	{
		if((n=fread(buf, 1, sizeof(buf)-1, fp)) > 0) 
		{
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
			procCmd = QString(cmd);
			procCmd.stripWhiteSpace();
            procName = extractExecName(procCmd);			
			#ifdef DEBUG_THREAD
			qDebug("procCmd %s", (const char*)procCmd.local8Bit() );
			qDebug("procName %s", (const char*)procName.local8Bit() );
			#endif
			emit pid2cmdObj(mPid , procCmd, procName);
		}	
		else
			emit pid2cmdFailed(mPid);
	}
	#ifdef DEBUG_THREAD
	qDebug("thread main end");
	#endif
	emit threadFinished();
}
QString pid2cmdThread::extractExecName(QString name)
{
	name.remove(name.find(" "), name.length() );
	if (name.find(" ")==-1 || name.find(" ") > name.find("/"))
	{
		while ( name.find("/") > -1 ) name.remove(0,name.find("/")+1 );
	}		
	return name;
}

/*************************************************************/

nativeIconThread::nativeIconThread(int itemIndex, ITEMTYPE t, QString cmdName, int width, int height, QObject *parent)
    : QObject(parent)
{
    mIndex = itemIndex;
    mType = t;
	mWidth = width;
	mHeight = height;
	mCmd = cmdName; 
	imgPath = getAppDir()+"img/";
	//#ifdef DEBUG_THREAD
	//qDebug("img path is %s", (const char*)imgPath.local8Bit() );
	//#endif
}
nativeIconThread::~nativeIconThread()
{
    #ifdef DEBUG_THREAD
	qDebug("nativeIconThread  itemindex %d end\n", mIndex);
	#endif
}
void nativeIconThread::run()
{
	#ifdef DEBUG_THREAD
	qDebug("===================================");
	qDebug("nativeIconThread thread start, cur index is %d", mIndex);
	#endif
	if(mType != ITEM_NATIV && mType != ITEM_SPECIAL)
	{
		emit nativeIconObjFailed(mIndex);
		#ifdef DEBUG_THREAD
		qDebug("nativeIconThread , item type not match");
		#endif			
	}
	else
	{
		if(mType == ITEM_NATIV)
		{
			mImg = QImage(imgPath+"native.png");
			mImg = mImg.smoothScale(mWidth, mHeight);
			emit nativeIconObj(mIndex, mType, mImg);
		}
		else if(mType == ITEM_SPECIAL)
		{
			if( mCmd.find("phone") > -1 ) mImg = QImage(imgPath+"phone.png"); 
			else if( mCmd.find("messaging") > -1) mImg = QImage(imgPath+"message.png"); 
			else if( mCmd.find("RokrToolsPro") > -1) mImg = QImage(imgPath+"rokrtool.png"); 
			else if( (mCmd.find("flowplayer") > -1) || (mCmd.find("xp") > -1) ) mImg = QImage(imgPath+"menu.png"); 
			#ifdef NEW_JAVA_LIST
			else if( mCmd.find("kvm") > -1) mImg = QImage(imgPath+"java.png"); 
			#endif
			mImg = mImg.smoothScale(mWidth, mHeight);
		    emit nativeIconObj(mIndex, mType, mImg);		
		}
		emit nativeIconObj(mIndex, mImg);
	}
    emit threadFinished();
	#ifdef DEBUG_THREAD
	qDebug("nativeIconThread main end");
	#endif	
}

/*************************************************************/

kvmToInfoThread::kvmToInfoThread(int itemIndex, QString cmdName, int width, int height, QObject *parent)
    : QObject(parent)
{
    mIndex = itemIndex;
	mCmd = cmdName;
	mWidth = width;
	mHeight = height;
	imgPath = getAppDir()+"/img/";
}
kvmToInfoThread::~kvmToInfoThread()
{
    #ifdef DEBUG_THREAD
	qDebug("kvmToInfoThread  itemindex %d end\n", mIndex);
	#endif
}
QString kvmToInfoThread::getJavaFolderInSD()
{
    #ifdef DEBUG_THREAD
	qDebug("==== getJavaFolderInSD() ====\n");
	#endif
	
	static DIR *dir;
	struct dirent *entry;
	char *name;
	QString qname;		

	dir = opendir("/mmc/mmca1/.system");
	if(!dir)
		qDebug("Can't open SD Card \n");
	else	
	{
		for(;;) 
		{	
			if((entry = readdir(dir)) == NULL) 
			{
				closedir(dir);
				dir = 0;
				break;
			}	
			name = entry->d_name;
			qname = QString(name);
			if (qname.find("java_")>-1)  return qname;
		}
	}
	#ifdef DEBUG_THREAD
	qDebug("=== JavaFolderInSD: not found ====\n");
	#endif
	return NULL;
}
QString kvmToInfoThread::getJavaName(QString name)
{
	QString sRes = QString::null;
    QString destName = QString::null;
	QString id;
	QString JavaFolderInSD = getJavaFolderInSD();
	int n;
	bool isFind = false;
	QFile file;
	#ifdef DEBUG_THREAD
    qDebug("name is %s", (const char*)name.local8Bit() );
	#endif
	
	QString path[] = {"/mmc/mmca1/.system/"+ JavaFolderInSD +"/JAMInfo/index",
	                  "/ezxlocal/download/mystuff/.system/java/JAMInfo/index"};
	
	for (int i = 0; i < 2; ++i)
	{				       
	    if(isFind) break;		
		if (QFile::exists(path[i]))
		{
		    file.setName(path[i]);
            file.open( IO_ReadOnly );
			while (!file.atEnd()) 
			{
				file.readLine(sRes,256);
				n = sRes.find("||");
				id = sRes.left(n);
				if(id == name)
				{
					destName = sRes.mid(n+2,sRes.find("||",n+2)-(n+2) );
					isFind = true;
					break;
				}	
			}
			file.close();
		} 
		else
		{
			qDebug("java name: can't open path or path is not exist\n");
		}
	}
	return destName;
}
void kvmToInfoThread::run()
{
	#ifdef DEBUG_THREAD
	qDebug("===================================");
	qDebug("kvmToInfoThread thread start, cur index is %d", mIndex);
	#endif
	int n;
	QString JavaFolderInSD = getJavaFolderInSD();
	
	n = mCmd.find("-launch");
	if ( n > -1 )
	{
		//Get java id from run line
		mCmd.remove(0,n+7);
		while ( mCmd[0]==' ' )
			mCmd.remove(0,1);
		n = mCmd.find(" ");
		if ( n > -1 )
			mCmd.remove(n,mCmd.length());		
		//Fix java id, if need		
		while ( mCmd.length() < 5 )
			mCmd = "0"+mCmd;
		//Set pixmep and name
		//QPixmap icon;
		//mJname = "";
		#ifdef DEBUG_THREAD
		qDebug("cur name proc is %s", (const char*)mCmd.local8Bit() );
		#endif

		mJname = "/mmc/mmca1/.system/"+ JavaFolderInSD +"/DownloadApps/MIDlet"+mCmd+"/Files/MIDlet-1.png" ;
		if (!QFile::exists(mJname))
		{
			mJname = "/ezxlocal/download/mystuff/.system/java/DownloadApps/MIDlet"+mCmd+"/Files/MIDlet-1.png";
			if (!QFile::exists(mJname))
				mJname = "/ezxlocal/download/mystuff/.system/java/SystemApps/MIDlet"+mCmd+"/Files/MIDlet-1.png";
		}
		if ( QFile::exists(mJname) )  mImg = QImage(mJname); 
		else mImg = QImage(imgPath+"javaIcon.png"); 
		mImg = mImg.smoothScale(mWidth, mHeight);
        
		mJname = getJavaName(mCmd);
		if ( mJname.length() == 0 )
			mJname = "Java ID:'"+mCmd+"'";
		mJname = QString::fromLocal8Bit(mJname);
		
		emit kvmToInfoObj(mIndex, mJname, mImg);
	}
	else
	    emit kvmToInfoObjFailed(mIndex);
	
	emit threadFinished();
	#ifdef DEBUG_THREAD
	qDebug("kvmToInfoThread main end");
	#endif
}


	




