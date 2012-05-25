#ifndef TASKTHREAD_H
#define TASKTHREAD_H

#define QT_THREAD_SUPPORT
#include <ZApplication.h>

#include <qstring.h>
#include <qthread.h>
#include <qimage.h>

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

#include "source.h"
#include "myDef.h"

class pid2cmdThread : public QObject, public QThread
{
    Q_OBJECT	
public:
	pid2cmdThread(int pid, QObject *parent = 0);
	~pid2cmdThread();
protected:
    void run();
	QString extractExecName(QString name);//, QString &destName);
signals:	
    void pid2cmdObj(int pid , QString pCmd, QString pName);
	void pid2cmdFailed(int pid);
	void threadFinished();
private:
    int mPid;
};
/*************************************************************/
/*************************************************************/
class nativeIconThread : public QObject, public QThread
{
    Q_OBJECT
public:
    nativeIconThread(int itemIndex = 0, ITEMTYPE t = ITEM_NATIV, QString cmdName = QString::null, 
	                 int width = 24, int height = 24, QObject *parent = 0);
	~nativeIconThread();				 
protected:
    void run();
signals:	
    void nativeIconObj(int itemIndex, ITEMTYPE type, QImage iconImg);
	void nativeIconObj(int itemIndex, QImage iconImg);
	void nativeIconObjFailed(int itemIndex);
	void threadFinished();	
private:
	QString getAppDir() {
		QString tmp = QString(qApp->argv()[0]);
		int i = tmp.findRev("/");
		tmp.remove(i+1,tmp.length()-1);
		return tmp;	
	};	
	int mIndex;
	int mWidth;
	int mHeight;
	QString mCmd;
	ITEMTYPE mType;
	QImage mImg;
	QString imgPath;
};
/*************************************************************/
/*************************************************************/
class kvmToInfoThread : public QObject, public QThread
{
    Q_OBJECT
public:	
	kvmToInfoThread(int itemIndex = 0, QString cmdName = QString::null, 
	                int width = 24, int height = 24, QObject *parent = 0);
	~kvmToInfoThread();
protected:
    void run();
	QString getJavaFolderInSD(); 	
	QString getJavaName(QString name);//, QString &destName);
signals:
	void kvmToInfoObj(int itemIndex, QString jname, QImage iconImg);
	void kvmToInfoObjFailed(int itemIndex);
	void threadFinished();		
private:
	QString getAppDir() {
		QString tmp = QString(qApp->argv()[0]);
		int i = tmp.findRev("/");
		tmp.remove(i+1,tmp.length()-1);
		return tmp;	
	};
	int mIndex;	
	int mWidth;
	int mHeight;	
	QString mCmd;
	QString mJname;
	QString imgPath;
	QImage mImg;
	
};



#endif //TASKTHREAD_H

