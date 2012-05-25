#ifndef MEGABOX_H
#define MEGABOX_H

#include <ZKbMainWidget.h>
#include <ZSoftKey.h>
#include <ZOptionsMenu.h>
#include <ZNavTabWidget.h>
#include <RES_ICON_Reader.h>

#ifndef RAISE_PHONE
	#include <SETUP_UTIL.h>
#endif

#include <qdatetime.h>
#include <qtimer.h>
#include <qscrollview.h>
#include <iostream>

#include "appListBox.h"
#include "TaskListBox.h"
#include "FileBrowser.h"
#include "myDef.h"
#include "daemon.h"
#include "searchUI.h"

class megabox : public ZKbMainWidget
{
	Q_OBJECT

public:
	enum ITEM_ORDER_TYPE_E{ 
		TYPE_START_ORDER	= 0, 	//开始选择item
		TYPE_ITME_SELECTED	= 1, 	//已经选中某个item
		TYPE_ORDER_OVER				//普通状态
	};	
	megabox(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
	~megabox();
    void createWindow(QWidget* parent);
	void preInitConf();
	void switchTab(int index);//, ITEMMODE mode = NORMAL);
	void timerEvent(QTimerEvent *e);
    void initTabs(); // need mod
	void regNotify();
	
	void fileModCore(QString title = QString::null, QString text = QString::null, 
					 QString cmdStr1 = QString::null, QString cmdStr2 = QString::null,
					 FILE_MOD_TYPE type = FILE_OTHER, bool isShowProcess = true);
	
	#ifndef RAISE_PHONE
	void fixKvmStats();
	#endif
	void fixFmWidget();
	
	void loadObj(const QString &gname, ITEMTYPE t, bool isMult = true, TaskListBox *lb = NULL, int tabIndex = 1);
	void loadJava(const QString &gname, bool isSD, bool isMult = true, TaskListBox *lb = NULL, int tabIndex = 1);
	
	FileBrowser *getFileBrowser();
	
protected:
    bool  eventFilter(QObject *, QEvent *);	

private :
	//bool tabItemError(int tabIndex);
	
private slots: //Ef >> eventFilter
	void slotEfProcKill(); 
	void slotEfRefreshProc();
	void slotEfHide();
	void slotEfFmDel();
	void slotEfRefreshMpkg();
	void slotEfRefreshMgx();
	void slotEfRefreshFav();
	void slotEfRefreshFm();
	void slotEfChgFmGeometry();
	void slotEfOrder(int tabIndex, bool isUp);
	
public slots:
	void obj_click(int index);
	void slotUpdateTime();
    void appSet();
	void slotQuit();
	void slotHide();	
	void slotOrder();
	void slotOrderBack();

	void updateTask();	
	void proc_click(int index);	
	void initReadWindowsProc();
	void initReadAllProc();
	void proc_quit();
	void proc_kill(); 
	void proc_setFilter();
	void proc_resetFilter();
    void proc_info();	
	void proc_state();
	void proc_reboot();
	void proc_poweroff();
	void showMgxInfoInProc();
	void proc_pid2top(); //普通切换java进程
	void proc_idle2pidToTop(); //精确切换java
	//void proc_back2menu();
	
	void fav_refresh();	
	void fav_add();
	void fav_del();
	void fav_rename();
	
	void app_props();	
	void mgx_move();
	void mgx_m_move();
	void mgx_del();
	void mgx_m_del();
	void mgx_refresh();
	//void mgx_back_refresh();
	void mgx_force_refresh();
	void mgx_save();
	void mgx_m_save();
	void mgx_create_menu();
	void mgx_delete_menu();
	void mgx_add_2_menu();
	void mgx_m_add_2_menu();
	void mgx_move_2_main();
	void mgx_m_move_2_main();
	void mgx_mod_menu();
	void mgx_rename();
	void mgx_setMult();
	void mgx_sel_all();
    void mgx_sel_none();
    void mgx_sel_back();
	
	//void app_set_permission();
	void mpkg_rename();
	void mpkg_refresh();	
	#ifdef TAB3_JAVA
	void java_back_up();
	#endif
	void java_set_profile();

	void mpkg_remove();	
	void mpkg_set_permission();
	void mpkg_run_type();

	void initReadFm();	
	void fm_create();
	void fm_del();
	void fm_cut();
	void fm_copy();
	void fm_paste();
	void fm_turn2root();
	void fm_turn2phone();
	void fm_turn2mmc();
	void fm_mult();
	void fm_sel_all();
    void fm_sel_none();
    void fm_sel_back();
	void fm_clear();
	void fm_rename();
	void fm_search();
	void fm_pack();
	void fm_openWith();
	void fm_fileInfo();
	void fm_sendBy_bt();
	void fm_openBookMark();
	void fm_editBookMark();
	void fm_addBookMark();
	
private slots:
	void slotCHNL(const QCString &,const QByteArray &);
	void slotMenuClick();
	void slotTabChg(QWidget* AWidget);

private :
	ZNavTabWidget *tabWidget;
	
	TaskListBox *procListbox;
	appListBox *favObjListbox;
	appListBox *mpkgListbox;
	appListBox *mgxListbox;
	FileBrowser *fmListbox;
	FileBrowser *fmListbox2;
	
	ZSoftKey *softKey;
	ZOptionsMenu *menu_left;
	ZOptionsMenu *menu_turn;
	ZOptionsMenu *menu_adv;	
	ZOptionsMenu *menu_mgr;	
	
	QScrollView * sv;
	
	QTimer *tTimer;

	QString m_strCurDir;

    QString fm_function_str; // 单文件复制粘贴时剪贴板路径
	
	RES_ICON_Reader iconReader;
	
	int curTab;
    #ifdef RAISE_PHONE
	int pidPhone;
	#else
	bool kvmStats;
	#endif
	int pidProcTmp; //当不使用单个java多开的时候，用来存储目标java的临时pid
	//int m_nItemOrderIndex;
	
	FM_MOD m_eMod;  // fm tab // chk the function mod => current mod is cut/copy/normal ?
	ITEMMODE m_eFmFunctionMod; // fm tab // chk if is multi-mod in cut/copy
	ITEM_ORDER_TYPE_E m_eMgxListOrderType;

	QStringList fmTmpCutList; // 多文件时复制粘贴数据
	QStringList fmTmpCopyList;
	
	bool m_bFmState; //双标签的flag
	//bool m_1st_adjust_fm_state;
	//bool codeCmp();
	//Daemon *mDaemon;
};
#endif
