#ifndef TASKLISTBOX_H
#define TASKLISTBOX_H

#include <ZListBox.h>
#include <RES_ICON_Reader.h>
#include <AM_AppLnk.h>
#include <AM_LauncherClient.h>

#include <qvaluelist.h>

#include "ZTaskItem.h"
#include "myDef.h"
#include "TaskThread.h"

#ifdef NEW_JAVA_LIST
typedef QValueList<AM_LauncherClient::AM_ApplicationState> AM_AppLaunchedList;
#endif

class TaskListBox : public ZListBox
{
	Q_OBJECT

public :
    TaskListBox(const QString& type = QString::null, QWidget* parent = 0, WFlags f = 0);
	~TaskListBox();
    void buildTaskList(bool isShowAll = false);
	void RefreshTaskList(bool isShowAll = false);
	void showNativeIcon();
	void kvmToInfo(); //if has kvm in process list >> replace kvm to java name and java icon
	#ifdef NEW_JAVA_LIST
	void buildJavaListNew(); // if define new java list
	#endif
	void updateTaskList();
	void updateNativeIcon();
	void updateKvmToInfo();
	#ifdef NEW_JAVA_LIST
	void updateJavaListNew();  // if define new java list 
	                           // buggy , need to debug
	#endif						   
	//int getListPids();
	#ifdef RAISE_PHONE
	int getPidPhone();
	#else
	bool getKvmStats();	
	#endif
	int getPidProc();	//RokrToolsPro pid


	QString getHlOriName(); //Highlighted item info
	QString getHlUid();
	ITEMTYPE getHlType();
	uint getHlPid();	

	QString getSelOriName(); //Selected item info
	QString getSelUid();
	ITEMTYPE getSelType();
	uint getSelPid();	

private:
 	QString getCmd(int pid);
	bool procFilter(QString &filterStr);	
	//int getListCount();
	void addPid2cmd(int pid);
	void addAllPid2cmd(int pid);
	void updatePid2cmd(int pid);
	//void nativeIconUpdate(int itemIndex, ITEMTYPE t);
	void addNativeIcon(int itemIndex, ITEMTYPE t, QString cmdName);//, int width, int height, QObject *parent = 0);
	void addKvm2Info(int itemIndex, QString cmdName);
	
	int gItemNum;
	
public slots:
    
signals:
    void taskHighlighted(int index);
	void taskSelected(int index);
	void taskSelected();
	
private slots:
    void itemHighlighted(int index);
    void itemSelected(int index);  	
	
	void slotAddPid2cmd(int pid, QString pCmd, QString pName);
	void slotAddAllPid2cmd(int pid, QString pCmd, QString pName);
	void slotUpdatePid2cmd(int pid, QString pCmd, QString pName);
	void slotAddNativeIcon(int itemIndex, QImage iconImg);
	void slotAddKvm2Info(int itemIndex, QString jname, QImage iconImg);

private:

	//bool codeCmp();
	//bool boolCodeCmp;
	//void preChk();
	
	QString hOriName;
	QString hUid;
	ITEMTYPE hType;
	uint hPid;
	
	QString sOriName;
	QString sUid;
	ITEMTYPE sType;
	uint sPid;	
	
	QString mCmdStrList; // global list for each process cmd when refresh the list box
    #ifdef RAISE_PHONE
	int pidPhone;
	#else
	bool kvmStats;
	#endif
	int pidProc;
    RES_ICON_Reader iconReader;
	
	//QList<struct procObj> refreshTmpList;
	QStringList refreshTmpList;
};

#endif // TASKLISTBOX_H

