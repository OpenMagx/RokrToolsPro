
#include <ZMessageDlg.h>
#include <ZNoticeDlg.h>
#include <ZListBox.h>
#include <ZLabel.h>
#include <ZPopup.h>
#include <ZGlobal.h>
#include <ZSingleCaptureDlg.h>
#include <ZSingleSelectDlg.h>
#include <ZConfig.h>
#include <AM_UidDef.h>

#include <qfile.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qtextcodec.h>
#include <qcopchannel_qws.h>
#include <qtextstream.h>

#include <sys/reboot.h>

#include "ZTaskItem.h"
#include "source.h"
#include "megabox.h"
#include "cfg.h"
#include "info.h"
#include "appAddDlg.h"
#include "deathDlg.h"
#include "myapplication.h"
#include "settingDlg.h"
#include "lng.h"
#include "codeComp.h"
#include "SHARE_Tool.h"
#include "saveDlg.h"
#include "UtilsDBOperation.h"

extern Settings *cfg;
extern MyApplication *app;
extern Lng *lng;
extern CDCP *codeComp;
extern UtilsDBOperation *mgxDB;
extern UtilsDBOperation *favDB;

megabox :: megabox( QWidget* parent, const char*, WFlags ) 
         : ZKbMainWidget(ZHeader::MAINDISPLAY_HEADER, 0, "megabox", WType_TopLevel/* | WStyle_StaysOnTop*/)
{
	preInitConf();
	
	/******  fix for sdl libs ******/
	#ifdef DEBUG
	qDebug("=== Check run SDL app ===\n");
	#endif
	
	if (QCopChannel::isRegistered("PE/SDLLib/")) {
		QCopChannel::send("PE/SDLLib/", "hide()");
		qDebug("Send hide SDL lib\n");
	}		
	createWindow(this);	
	initTabs();
	
	//if( cfg->cfg_fullScreenStatus)
	//	this->setFullScreenMode(true);
	if( cfg->cfg_timeHeader ) {
		tTimer->start(1000);
	}
	startTimer(300);
}
void megabox ::preInitConf()
{
	curTab = 0;
	m_strCurDir = "/mmc";
	pidProcTmp = -1;
 	m_eMod = FM_MOD_NORMAL;
	m_eFmFunctionMod = NORMAL;
	m_eMgxListOrderType = TYPE_ORDER_OVER;
	fm_function_str = QString::null;
	fmTmpCutList = NULL;
    fmTmpCopyList = NULL;	
	tTimer = NULL;
	m_bFmState = true;
}

void megabox ::createWindow(QWidget* parent)
{
	qApp->installEventFilter( parent );
	
	#ifdef DEBUG
	qDebug("=== create main window ===\n");
	#endif
    setAppTitle( QString(APP_TITLE) );

    procListbox = new TaskListBox("%I%M", parent, 0);
	procListbox->enableMarquee(ZListBox::LISTITEM_REGION_A, true);
    procListbox->enableMarquee(ZListBox::LISTITEM_REGION_B, true);

    favObjListbox = new appListBox(appListBox::ALB_FAV, "%I%M", parent, 0);
	favObjListbox->enableMarquee(ZListBox::LISTITEM_REGION_A, true);
    favObjListbox->enableMarquee(ZListBox::LISTITEM_REGION_B, true);	
	
	#ifdef TAB3_JAVA
	mpkgListbox = new appListBox(appListBox::ALB_JAVA, "%I%M", parent, 0);
	#else
	mpkgListbox = new appListBox(appListBox::ALB_MPKG, "%I%M", parent, 0);
	#endif
	mpkgListbox->enableMarquee(ZListBox::LISTITEM_REGION_A, true);
    mpkgListbox->enableMarquee(ZListBox::LISTITEM_REGION_B, true);
	
	mgxListbox = new appListBox(appListBox::ALB_MGX, "%I%M", parent, 0);
	mgxListbox->enableMarquee(ZListBox::LISTITEM_REGION_A, true);
    mgxListbox->enableMarquee(ZListBox::LISTITEM_REGION_B, true);	
	
	/******** sv**********/
	sv = new QScrollView(this );//, 0, 0);
	sv->setFixedWidth ( SCREEN_WHIDTH );
	sv->setMargin(-2);
	
	fmListbox = new FileBrowser( QString("/mmc/mmca1"), "%I%M", parent, 0);
	fmListbox->enableMimeType(cfg->cfg_showMimeType);
	fmListbox->enableMarquee(ZListBox::LISTITEM_REGION_A, true);
    fmListbox->enableMarquee(ZListBox::LISTITEM_REGION_B, true);

	sv->addChild(fmListbox, 0, 0);

	fmListbox2 = new FileBrowser( QString("/ezxlocal/download/mystuff"), "%I%M", parent, 0);
	fmListbox2->enableMimeType(cfg->cfg_showMimeType);
	fmListbox2->enableMarquee(ZListBox::LISTITEM_REGION_A, true);
	fmListbox2->enableMarquee(ZListBox::LISTITEM_REGION_B, true);	
	
	//#ifdef ZN5
	//fmListbox->setFixedWidth ( FM_WIDGET_WIDTH );
	//fmListbox2->setFixedWidth ( SCREEN_WHIDTH-FM_WIDGET_WIDTH );
	//#endif
	sv->addChild(fmListbox2, FM_WIDGET_WIDTH, 0);	
	
	/******** get tab img **********/
	tabWidget = new ZNavTabWidget(parent);
	setContentWidget(tabWidget);//, true);//, 0, (ZSkinService::WidgetClsID)63);	
    tabWidget->stopNextWhenKeyRepeat(true);
	
	QPixmap imgTab;//, imgTab1, imgTab2, imgTab3, imgTab4;
	imgTab.load(getAppDir()+"/img/tab0.png");
	tabWidget->addTab(procListbox,QIconSet(imgTab), "");		
	imgTab.load(getAppDir()+"/img/tab1.png");
	tabWidget->addTab(favObjListbox, QIconSet(imgTab), "");	
	imgTab.load(getAppDir()+"/img/tab2.png");
    tabWidget->addTab(mpkgListbox, QIconSet(imgTab), "");	
	imgTab.load(getAppDir()+"/img/tab3.png");
    tabWidget->addTab(mgxListbox, QIconSet(imgTab), "");			
	imgTab.load(getAppDir()+"/img/tab4.png");
    //tabWidget->addTab(fmListbox, QIconSet(imgTab), "");		
	tabWidget->addTab(sv, QIconSet(imgTab), "");		

	/******** for tab widget ***************/
	
	QRect rect=ZGlobal::getContentR();
	QRect subRect=ZGlobal::getSubContentR();
	
	/*********************** softkeys and menus ***********************/	
	
	softKey = new ZSoftKey("CST_2A", parent, parent);
	setSoftKey(softKey);	
    softKey->setTextForOptMenuHide(tr("TXT_RID_SOFTKEY_OPTIONS", "Options"));
	softKey->setTextForOptMenuShow(tr("TXT_RID_SOFTKEY_SELECT", "Select"),
	                               tr("TXT_RID_SOFTKEY_CANCEL", "Cancel"));
	softKey->setText(ZSoftKey::LEFT,  tr("TXT_RID_SOFTKEY_OPTIONS", "Options"));
	softKey->setText(ZSoftKey::RIGHT, tr("TXT_RID_SOFTKEY_BACK", "Back"));

 	menu_left = new ZOptionsMenu(rect, parent, 0, 0);//, (ZSkinService::WidgetClsID)55);
    menu_adv = new ZOptionsMenu(rect, parent, 0, 0);//, (ZSkinService::WidgetClsID)55);	
	menu_turn = new ZOptionsMenu(rect, parent, 0, 0);//, (ZSkinService::WidgetClsID)55);
	menu_mgr = new ZOptionsMenu(rect, parent, 0, 0);//, (ZSkinService::WidgetClsID)55);	 
	
    softKey->setOptMenu(ZSoftKey::LEFT, menu_left);
	#ifdef RIGHT_KEY_NO_HIDE
	softKey->setClickedSlot( ZSoftKey::RIGHT, parent, SLOT ( slotQuit() ) );
	#else
	softKey->setClickedSlot( ZSoftKey::RIGHT, parent, SLOT ( slotHide() ) );
	#endif
	/********* channel *********/
	
	tTimer = new QTimer(parent);
	connect( tTimer, SIGNAL( timeout() ), parent, SLOT( slotUpdateTime() ) );
	
	QCopChannel *chnl = new QCopChannel("/EZX/ROKRTOOLS", parent);//, "RokrToolsPro"); 
	connect(chnl,SIGNAL(received(const QCString &,const QByteArray &)),parent,SLOT(slotCHNL(const QCString &,const QByteArray &)));
	#ifndef VE66_FIX_SWITCH
	connect(qApp, SIGNAL( signalRaise() ), parent, SLOT( slotMenuClick() ) );
	connect(qApp, SIGNAL( clickAppIcon() ), parent, SLOT( slotMenuClick() ) );
	#endif
	connect(procListbox, SIGNAL( taskSelected(int) ), parent, SLOT( proc_click(int) ) );	
	connect(favObjListbox, SIGNAL( itemSelected(int) ), parent, SLOT(obj_click(int)));
	connect(mpkgListbox, SIGNAL( itemSelected(int) ), parent, SLOT(obj_click(int)));
	connect(mgxListbox, SIGNAL( itemSelected(int) ), parent, SLOT(obj_click(int)));	
	connect(tabWidget, SIGNAL( currentChanged(QWidget* ) ), parent, SLOT( slotTabChg(QWidget* ) ) );	
}

megabox::~megabox()
{
	#ifdef DEBUG
    qDebug("===== delete megabox \n");
	#endif
	if( tTimer->isActive() ) {
		tTimer->stop();	
		delete tTimer;
		tTimer = NULL;
	}
	this->removeEventFilter(this);
} 
void megabox::timerEvent(QTimerEvent *e)
{
	int tabIndex = tabWidget->currentPageIndex();
	if (tabIndex != curTab)
	{
		curTab = tabIndex;
		switch (tabIndex) 
		{
			case TAB_MPKG :
				if(cfg->cfg_refreshWhenTabChg) 
					QTimer::singleShot( 100, this, SLOT( mpkg_refresh() ) );
					break;
			case TAB_MGX :
				if(cfg->cfg_refreshWhenTabChg) 
					QTimer::singleShot( 100, this, SLOT( mgx_refresh() ) );	
					break;
			case TAB_PROC:
				if( codeComp->cpcp() )  {	
					#ifndef NEW_JAVA_LIST
					if(cfg->cfg_markItem) 
						QTimer::singleShot( 100, this, SLOT( updateTask() ) );
					else 
					#endif
						QTimer::singleShot( 100, this, SLOT( initReadWindowsProc() ) );
				} else {
					QTimer::singleShot( 100, this, SLOT( initReadWindowsProc() ) );
				}	
				break;
			case TAB_FM:
				QTimer::singleShot( 100, this, SLOT( initReadFm() ) );	
				break;	
			case TAB_FAVTAB :
				break;	 
			default:
				break;
		}
	}
	qApp->processEvents();	
	ZKbMainWidget::timerEvent(e);
}
void megabox::slotUpdateTime()
{
	if( !cfg->cfg_timeHeader )
		return;
	
	QTime time = QTime::currentTime();	
	#ifdef SCREEN_V
	QString title = QString("[%1]").arg(time.toString());
	#else
	QString title = QString("%1 [%2]").arg(APP_TITLE).arg(time.toString());
	#endif
	this->setAppTitle(title);
	qApp->processEvents();		
}
void megabox::slotTabChg(QWidget* )
{
	int tabIndex = tabWidget->currentPageIndex();
	switchTab(tabIndex);
}
void megabox::slotCHNL(const QCString &msg,const QByteArray &data)
{
    if(msg == "RaiseUI()")  {
        #ifdef DEBUG 
		qDebug("mgs is RaiseUI() \n");
		#endif
		curTab = -1;
		
		if ( QCopChannel::isRegistered("PE/SDLLib/") ) 
		    QCopChannel::send("PE/SDLLib/", "hide()");
			
		this->show();	
		this->raise();
		//slotUpdateTime();
		tabWidget->setCurrentPage(TAB_PROC);
		if(cfg->cfg_timeHeader) {
			if( !tTimer->isActive() )
				tTimer->start(1000);
		}	
	}	
	else if(msg == "MenuRaise()") {
        #ifdef DEBUG 
		qDebug("mgs is MenuRaise() \n");
		#endif		
		slotMenuClick();
	}
	else
	{
	   qDebug("message error. \n");
	}
	qApp->processEvents();		
}

void megabox::switchTab(int index)//, ITEMMODE mode)
{
	
	menu_left->clear(); 
	menu_adv->clear();
	menu_turn->clear();
	menu_mgr->clear();
	#ifdef DEBUG 
	qDebug("swich tab \n");
	#endif
	int i = -1, j=-1;
	switch (index)
	{
	case TAB_MPKG:		
		#ifdef TAB3_JAVA			
		menu_mgr->insertItem(lng->tr("FT_BACKUP"), NULL, this, SLOT( java_back_up() ), true, false, false, ++i, ++j);
		#endif
		menu_mgr->insertItem(lng->tr("FT_DELETE"), NULL, this, SLOT( mpkg_remove() ), true, false, false,  ++i, ++j);
		#if defined(TAB3_JAVA) && !defined(VE66)
		menu_mgr->insertItem(lng->tr("FT_SET_PROFILE"), NULL, this, SLOT( java_set_profile() ), true, false, false, ++i, ++j);
		#endif
		i = -1, j=-1;
		menu_left->insertItem(lng->tr("FT_PROPS"), NULL, this, SLOT(app_props()), true, false, false,  ++i, ++j);
        menu_left->insertItem(lng->tr("FT_RENAME"), NULL, this, SLOT(mpkg_rename()), true, false, false,  ++i, ++j);	
		menu_left->insertItem(lng->tr("FT_MANAGER"), menu_mgr, NULL, true,  ++i, ++j, false, false, false );
		menu_left->insertItem(lng->tr("FT_REFRESH"), NULL, this, SLOT(mpkg_refresh()), true, false, false,  ++i, ++j);
		menu_left->insertSeparator( ++i, ++j);
		menu_left->insertItem(lng->tr("FT_ABOUT"), NULL, this, SLOT(appSet()), true, false, false,  ++i, ++j);
		menu_left->insertSeparator( ++i, ++j);
		menu_left->insertItem(lng->tr("FT_QUIT"), NULL, this, SLOT(slotQuit()), true, false, false, ++i, ++j);	
		mpkgListbox->setFocus();
        break;
	case TAB_MGX:
		if(mgxListbox->getMode() == MULTI)
		{
			menu_left->insertItem(lng->tr("FT_SELECT_ALL"), NULL, this, SLOT(mgx_sel_all()), true, false, false,  ++i, ++j);
			menu_left->insertItem(lng->tr("FT_SELECT_NONE"), NULL, this, SLOT(mgx_sel_none()), true, false, false,  ++i, ++j);
			menu_left->insertItem(lng->tr("FT_CANCEL"), NULL, this, SLOT(mgx_sel_back()), true, false, false,  ++i, ++j);		
			menu_left->insertItem(lng->tr("FT_ADD_TO_MENU"), NULL, this, SLOT(mgx_m_add_2_menu() ), true, false, false,  ++i, ++j);
			menu_left->insertItem(lng->tr("FT_MOVE_TO_MAIN"), NULL, this, SLOT(mgx_m_move_2_main() ), true, false, false,  ++i, ++j);
			menu_left->insertItem(lng->tr("FT_SAVE"), NULL, this, SLOT(mgx_m_save()), true, false, false,  ++i, ++j);		
			menu_left->insertItem(lng->tr("FT_MOVE"), NULL, this, SLOT(mgx_m_move()), true, false, false,  ++i, ++j);
			menu_left->insertItem(lng->tr("FT_DELETE"), NULL, this, SLOT(mgx_m_del()), true, false, false,  ++i, ++j);			
		}else
		{
			if( m_eMgxListOrderType == TYPE_ITME_SELECTED ) {
				softKey->setText( ZSoftKey::LEFT, lng->tr("FT_ORDER") );
				softKey->setText( ZSoftKey::RIGHT, lng->tr("FT_CANCEL") );
				softKey->setClickedSlot( ZSoftKey::LEFT, this, SLOT(slotOrder()) );	
				softKey->setClickedSlot( ZSoftKey::RIGHT, this, SLOT(slotOrderBack()) );			
			} else 
			if( m_eMgxListOrderType == TYPE_START_ORDER) {
				softKey->setText( ZSoftKey::LEFT, lng->tr("FT_INSERT") );
				softKey->setText( ZSoftKey::RIGHT, lng->tr("FT_CANCEL") );
				softKey->setClickedSlot( ZSoftKey::LEFT, this, SLOT(slotOrder()) );	
				softKey->setClickedSlot( ZSoftKey::RIGHT, this, SLOT(slotOrderBack()) );			
			} else {
				softKey->setText(ZSoftKey::LEFT,  tr("TXT_RID_SOFTKEY_OPTIONS", "Options"));
				softKey->setText(ZSoftKey::RIGHT, tr("TXT_RID_SOFTKEY_BACK", "Back"));
				softKey->setOptMenu(ZSoftKey::LEFT, menu_left);
				#ifdef RIGHT_KEY_NO_HIDE
				softKey->setClickedSlot( ZSoftKey::RIGHT, this, SLOT ( slotQuit() ) );
				#else
				softKey->setClickedSlot( ZSoftKey::RIGHT, this, SLOT ( slotHide() ) );
				#endif						
				menu_mgr->insertItem(lng->tr("FT_ADD_TO_MENU"), NULL, this, SLOT(mgx_add_2_menu()), true, false, false, ++i, ++j);	
				menu_mgr->insertItem(lng->tr("FT_MOVE_TO_MAIN"), NULL, this, SLOT(mgx_move_2_main()), true, false, false,++i, ++j);		
				menu_mgr->insertItem(lng->tr("FT_CREATE_MENU"), NULL, this, SLOT(mgx_create_menu()), true, false, false, ++i, ++j);	
				menu_mgr->insertItem(lng->tr("FT_DELETE_MENU"), NULL, this, SLOT(mgx_delete_menu()), true, false, false, ++i, ++j);	
				menu_mgr->insertItem(lng->tr("FT_RENAME_MENU"), NULL, this, SLOT(mgx_mod_menu()), true, false, false, ++i, ++j);	
				menu_mgr->insertItem(lng->tr("FT_SELMULTI"), NULL, this, SLOT(mgx_setMult()), true, false, false, ++i, ++j);	
				menu_mgr->insertItem(lng->tr("FT_FT_ORDER"), NULL, this, SLOT(slotOrder()), true, false, false, ++i, ++j);					
				i = -1, j=-1;
				menu_left->insertItem(lng->tr("FT_PROPS"), NULL, this, SLOT(app_props()), true, false, false, ++i, ++j);	
				menu_left->insertItem(lng->tr("FT_SAVE"), NULL, this, SLOT(mgx_save()), true, false, false, ++i, ++j);		
				menu_left->insertItem(lng->tr("FT_MOVE"), NULL, this, SLOT(mgx_move()), true, false, false, ++i, ++j);	
				menu_left->insertItem(lng->tr("FT_DELETE"), NULL, this, SLOT(mgx_del()), true, false, false, ++i, ++j);		
				menu_left->insertItem(lng->tr("FT_RENAME"), NULL, this, SLOT(mgx_rename()), true, false, false, ++i, ++j);			
				menu_left->insertItem(lng->tr("FT_MANAGER"), menu_mgr, NULL, true, ++i, ++j,  false, false, false );		
				menu_left->insertItem(lng->tr("FT_REFRESH"), NULL, this, SLOT(mgx_refresh()), true, false, false, ++i, ++j);	
				menu_left->insertItem(lng->tr("FT_FORCE_REFRESH"), NULL, this, SLOT(mgx_force_refresh()), true, false, false, ++i, ++j);	
				menu_left->insertSeparator(++i, ++j);	
				menu_left->insertItem(lng->tr("FT_ABOUT"), NULL, this, SLOT(appSet()), true, false, false, ++i, ++j);			
				menu_left->insertSeparator(++i, ++j);	
				menu_left->insertItem(lng->tr("FT_QUIT"), NULL, this, SLOT(slotQuit()), true, false, false, ++i, ++j);
			}
		}
		mgxListbox->setFocus();	
		break;
    case TAB_FM:
		if(getFileBrowser()->getMode() == MULTI)
		{
			menu_left->insertItem(lng->tr("FT_SELECT_ALL"), NULL, this, SLOT(fm_sel_all()), true, false, false, ++i, ++j);	
			menu_left->insertItem(lng->tr("FT_SELECT_NONE"), NULL, this, SLOT(fm_sel_none()), true, false, false, ++i, ++j);	
			menu_left->insertItem(lng->tr("FT_CANCEL"), NULL, this, SLOT(fm_sel_back()), true, false, false, ++i, ++j);		
			menu_left->insertItem(lng->tr("FT_DELETE"), NULL, this, SLOT(fm_del()), true, false, false, ++i, ++j);	
			menu_left->insertItem(lng->tr("FT_CUT"), NULL, this, SLOT(fm_cut()), true, false, false, ++i, ++j);					
			menu_left->insertItem(lng->tr("FT_COPY"), NULL, this, SLOT(fm_copy()), true, false, false, ++i, ++j);	
			//menu_left->insertItem(lng->tr("FT_SEND_BY_BT"), NULL, this, SLOT( fm_sendBy_bt() ), true, false, false, 6, 6);	
			menu_left->insertSeparator(++i, ++j);	
			menu_left->insertItem(lng->tr("FT_ABOUT"), NULL, this, SLOT(appSet()), true, false, false, ++i, ++j);	
			menu_left->insertSeparator(++i, ++j);	
			menu_left->insertItem(lng->tr("FT_QUIT"), NULL, this, SLOT(slotQuit()), true, false, false, ++i, ++j);			
		}else
		{
			menu_turn->insertItem(lng->tr("FT_OPEN_BOOKMARK"), NULL, this, SLOT(fm_openBookMark()), true, false, false, ++i, ++j);
			menu_turn->insertItem(lng->tr("FT_ROOT"), NULL, this, SLOT(fm_turn2root()), true, false, false, ++i, ++j);	
			menu_turn->insertItem(lng->tr("FT_PHONE"), NULL, this, SLOT(fm_turn2phone()), true, false, false, ++i, ++j);	
			menu_turn->insertItem(lng->tr("FT_MMCA1"), NULL, this, SLOT(fm_turn2mmc()), true, false, false,++i, ++j);		
			menu_turn->insertItem(lng->tr("FT_EDIT_BOOKMARK"), NULL, this, SLOT(fm_editBookMark()), true, false, false, ++i, ++j);
			menu_turn->insertItem(lng->tr("FT_ADD_BOOKMARK"), NULL, this, SLOT(fm_addBookMark()), true, false, false, ++i, ++j);			
			i = -1, j=-1;	
			menu_mgr->insertItem(lng->tr("FT_DELETE"), NULL, this, SLOT(fm_del()), true, false, false, ++i, ++j);	
			menu_mgr->insertItem(lng->tr("FT_CUT"), NULL, this, SLOT(fm_cut()), true, false, false, ++i, ++j);					
			menu_mgr->insertItem(lng->tr("FT_COPY"), NULL, this, SLOT(fm_copy()), true, false, false, ++i, ++j);	
			menu_mgr->insertItem(lng->tr("FT_PASTE"), NULL, this, SLOT(fm_paste()), true, false, false, ++i, ++j);	
			i = -1, j=-1;
			menu_adv->insertItem(lng->tr("FT_CLEAR"), NULL, this, SLOT(fm_clear()), true, false, false, ++i, ++j);	
			menu_adv->insertItem(lng->tr("FT_SELMULTI"), NULL, this, SLOT(fm_mult()), true, false, false,++i, ++j);			
			menu_adv->insertItem(lng->tr("FT_SEARCH"), NULL, this, SLOT(fm_search()), true, false, false, ++i, ++j);	
			menu_adv->insertItem(lng->tr("FT_PACK"), NULL, this, SLOT(fm_pack()), true, false, false, ++i, ++j);	
			menu_adv->insertItem(lng->tr("FT_PROPS"), NULL, this, SLOT( fm_fileInfo() ), true, false, false, ++i, ++j);	
			menu_adv->insertItem(lng->tr("FT_OPENWITH"), NULL, this, SLOT( fm_openWith() ), true, false, false, ++i, ++j);	
			menu_adv->insertItem(lng->tr("FT_SEND_BY_BT"), NULL, this, SLOT( fm_sendBy_bt() ), true, false, false, ++i, ++j);				
			i = -1, j=-1;
			menu_left->insertItem(lng->tr("FT_CREATE"), NULL, this, SLOT(fm_create()), true, false, false, ++i, ++j);	
			menu_left->insertItem(lng->tr("FT_MANAGER"), menu_mgr, NULL, true, ++i, ++j, false, false, false );
			menu_left->insertItem(lng->tr("FT_BOOKMARK"), menu_turn, NULL, true, ++i, ++j, false, false, false );
			menu_left->insertItem(lng->tr("FT_ADV"), menu_adv, NULL, true, ++i, ++j, false, false, false );
			menu_left->insertItem(lng->tr("FT_RENAME"), NULL, this, SLOT(fm_rename()), true, false, false, ++i, ++j);			
			menu_left->insertItem(lng->tr("FT_REFRESH"), NULL, this, SLOT(initReadFm()), true, false, false, ++i, ++j);	
			menu_left->insertSeparator(++i, ++j);	
			menu_left->insertItem(lng->tr("FT_ABOUT"), NULL, this, SLOT(appSet()), true, false, false, ++i, ++j);	
			menu_left->insertSeparator(++i, ++j);	
			menu_left->insertItem(lng->tr("FT_QUIT"), NULL, this, SLOT(slotQuit()), true, false, false, ++i, ++j);	
					
			menu_adv->setItemEnabled(0, false);
		}
		getFileBrowser()->setFocus(); 
	    break;
	case TAB_FAVTAB:
       	menu_left->insertItem(lng->tr("FT_ADD"), NULL, this, SLOT(fav_add()), true, false, false, ++i, ++j);	
		menu_left->insertItem(lng->tr("FT_DELETE"), NULL, this, SLOT(fav_del()), true, false, false, ++i, ++j);	
		menu_left->insertItem(lng->tr("FT_RENAME"), NULL, this, SLOT(fav_rename()), true, false, false, ++i, ++j);	
		menu_left->insertItem(lng->tr("FT_REFRESH"), NULL, this, SLOT(fav_refresh()), true, false, false, ++i, ++j);	
     	menu_left->insertSeparator(++i, ++j);	
		menu_left->insertItem(lng->tr("FT_ABOUT"), NULL, this, SLOT(appSet()), true, false, false, ++i, ++j);	
		menu_left->insertSeparator(++i, ++j);	
        menu_left->insertItem(lng->tr("FT_QUIT"), NULL, this, SLOT(slotQuit()), true, false, false, ++i, ++j);	  
		favObjListbox->setFocus();
	   break;
	case TAB_PROC:
	    menu_turn->insertItem(lng->tr("FT_SHOW_WIN"), NULL, this, SLOT(initReadWindowsProc()), true, false, false, ++i, ++j);	
	    menu_turn->insertItem(lng->tr("FT_SHOW_ALL"), NULL, this, SLOT(initReadAllProc()), true, false, false, ++i, ++j);	
		i = -1, j=-1;
		menu_adv->insertItem(lng->tr("FT_ADD_PROC_FILTER"), NULL, this, SLOT(proc_setFilter()), true, false, false, ++i, ++j);	
		menu_adv->insertItem(lng->tr("FT_RESET_FILTER"), NULL, this, SLOT(proc_resetFilter()), true, false, false, ++i, ++j);		
		menu_adv->insertItem(lng->tr("FT_PROC_INFO"), NULL, this, SLOT(proc_info()), true, false, false, ++i, ++j);	
		menu_adv->insertItem(lng->tr("FT_SYSTEM_STATE"), NULL, this, SLOT(proc_state()), true, false, false, ++i, ++j);	
		menu_adv->insertItem(lng->tr("FT_REBOOT"), NULL, this, SLOT( proc_reboot() ), true, false, false, ++i, ++j);	
		menu_adv->insertItem(lng->tr("FT_POWEROFF"), NULL, this, SLOT( proc_poweroff() ), true, false, false, ++i, ++j);	
		i = -1, j=-1;
		menu_left->insertItem(lng->tr("FT_IDLE_SWITCH_JAVA"), NULL, this, SLOT(proc_idle2pidToTop()), true, false, false, ++i, ++j);		
		menu_left->insertItem(lng->tr("FT_SWITCH_APP"), NULL, this, SLOT(proc_pid2top()), true, false, false, ++i, ++j);				
		menu_left->insertItem(lng->tr("FT_QUIT_PROC"), NULL, this, SLOT(proc_quit()), true, false, false, ++i, ++j);	
		menu_left->insertItem(lng->tr("FT_KILL"), NULL, this, SLOT(proc_kill()), true, false, false, ++i, ++j);	
		//menu_left->insertItem(lng->tr("FT_BACK2MENU"), NULL, this, SLOT(proc_back2menu()), true, false, false, 2, 2);			
		menu_left->insertItem(lng->tr("FT_SHOW"), menu_turn, NULL, true, ++i, ++j, false, false, false );    
		menu_left->insertItem(lng->tr("FT_ADV"), menu_adv, NULL, true, ++i, ++j, false, false, false );  
        menu_left->insertItem(lng->tr("FT_REFRESH"), NULL, this, SLOT(initReadWindowsProc()), true, false, false, ++i, ++j);	
		menu_left->insertSeparator(++i, ++j);	
		menu_left->insertItem(lng->tr("FT_APPSET"), NULL, this, SLOT(appSet()), true, false, false, ++i, ++j);	
        menu_left->insertSeparator(++i, ++j);	
		menu_left->insertItem(lng->tr("FT_QUIT"), NULL, this, SLOT(slotQuit()), true, false, false, ++i, ++j);				
		if( !codeComp->cpcp() ) {
			menu_left->setItemEnabled(0, false);
		}
		procListbox->setFocus();
		break;
	default:
		break;
	}
}

void megabox::obj_click(int index)
{
	int tabIndex = tabWidget->currentPageIndex();
	#ifdef DEBUG
	qDebug("Debug : obj click, cur tab %d \n", tabIndex);
	#endif
	ZTaskItem* pItem = NULL;
	#ifndef NEW_JAVA_LIST
	bool runJavaInMult = cfg->cfg_runJavaInMult;
	#else
	bool runJavaInMult = false;
	#endif
	switch(tabIndex)
	{
		case TAB_MPKG :
		{
			pItem= (ZTaskItem*)mpkgListbox->item(index); 
			#ifdef TAB3_JAVA
			loadJava(pItem->getGname(), pItem->isSD(), runJavaInMult, procListbox, TAB_MPKG);	
			#else
			loadObj(pItem->getGname(), pItem->getType(), runJavaInMult, procListbox, TAB_MPKG);	
			#endif	
			#ifdef CLICK_NO_HIDE
			slotQuit();
			#else
			if(cfg->cfg_hideAfterRunApp) { slotHide(); }
			#endif	
		}	
			break;
		case TAB_MGX :
		{
			pItem= (ZTaskItem*)mgxListbox->item(index); 
			if( pItem->getType() == ITEM_MGX_OBJ ) {
				loadObj( pItem->getGname(), pItem->getType(), false, NULL, TAB_MGX );	
				#ifdef CLICK_NO_HIDE
				slotQuit();
				#else
				if(cfg->cfg_hideAfterRunApp) { slotHide(); }
				#endif					
			} 
			else if ( pItem->getType() == ITEM_MENU_OBJ ) {
				mgxListbox->openMenu( pItem->getGname() );				
			} 
			else
				return;
		}		
			break;
		case TAB_FAVTAB :
		{
			pItem= (ZTaskItem*)favObjListbox->item(index); 
			loadObj(pItem->getGname(), pItem->getType(), runJavaInMult, procListbox, TAB_FAVTAB);	
			#ifdef CLICK_NO_HIDE
			slotQuit();
			#else
			if(cfg->cfg_hideAfterRunApp) { slotHide(); }
			#endif	
		}		
			break;
		default :
			break;	
	}
	qApp->processEvents();	
}

void megabox::slotHide()
{
	int tabIndex = tabWidget->currentPageIndex();
	if (tabIndex == TAB_FM) {
		getFileBrowser()->cdBack();	
		switchTab(TAB_FM);		
	}
	else  
	{	
		if( tTimer->isActive() ) {
			tTimer->stop();	
			fmListbox->clear();
			if( fmListbox2 != NULL ) { fmListbox2->clear(); }
		}	
		this->hide();		
	}	
}
void megabox::initTabs() 
{
	QTimer::singleShot( 100, this, SLOT( mpkg_refresh() ) );	
	QTimer::singleShot( 100, this, SLOT( mgx_refresh() ) );	
	//QTimer::singleShot( 100, this, SLOT( initReadFm() ) );	
	QTimer::singleShot( 100, this, SLOT( fav_refresh() ) );
	QTimer::singleShot( 100, this, SLOT( initReadWindowsProc() ) );
	qApp->processEvents();		
}

void megabox::slotQuit()
{
	if( showQ(lng->tr("FT_QUIT"), lng->tr("FT_CONFIRM_QUIT"), 1) ) {
		qApp->quit();
	} else {
		return;
	}	
} 
void megabox::slotOrder()
{	
	if( !codeComp->cpcp() ) {
		regNotify();
		return;
	}	
	if( m_eMgxListOrderType == TYPE_START_ORDER ) {
		//cout<<" ============  m_eMgxListOrderType == TYPE_START_ORDER, item selected "<<endl;
		m_eMgxListOrderType = TYPE_ITME_SELECTED;
	} else 
	if( m_eMgxListOrderType == TYPE_ITME_SELECTED ) {
		//cout<<" ============  m_eMgxListOrderType == TYPE_ITME_SELECTED,  wait to focus "<<endl;
		m_eMgxListOrderType = TYPE_START_ORDER;	
	} else 
	if( m_eMgxListOrderType == TYPE_ORDER_OVER ) {
		//cout<<" ============  m_eMgxListOrderType == TYPE_ORDER_OVER,  start order function"<<endl;
		m_eMgxListOrderType = TYPE_START_ORDER;
	}
	int tabIndex = tabWidget->currentPageIndex();
	switchTab(tabIndex);
}
void megabox::slotOrderBack()
{
	//cout<<"=========== order back ============="<<endl;
	m_eMgxListOrderType = TYPE_ORDER_OVER;
	int tabIndex = tabWidget->currentPageIndex();
	QStringList gnameList;
	for( int i=0; i<mgxListbox->count(); ++i) {
		ZTaskItem* pItem= (ZTaskItem*)mgxListbox->item(i); 
		if( pItem->getType() != ITEM_MGX_OBJ ) { continue; }
		gnameList += pItem->getGname();
		gnameList += ";";	
	}
	mgxDB->orderSubItemList( gnameList, mgxListbox->getCurMenuId() );
	mgxDB->flush();
	
	QTimer::singleShot( 100, this, SLOT( mgx_refresh() ) );	
	switchTab(tabIndex);
}
void megabox::appSet()
{
	bool mark = cfg->cfg_usrMultiWindow;
	
	SettingDlg *dlg = new SettingDlg();
	dlg->exec();
	bool isCancel = dlg->isCancel;
	//bool mode = dlg->isFullScreenMode;
	if( !isCancel ) {
		delete dlg;
		dlg = NULL;	
		/*if( mode == true ) 	{
			if( this->getFullScreenMode() == false )
				this->setFullScreenMode(true);	
		}
		else if( mode == false ) {
			if( this->getFullScreenMode() == true )
				this->setFullScreenMode(false);	
			if(keyTimer->isActive())
				keyTimer->stop();
			//tabWidget->setSubHeaderVisible(true);	
			softKey->show();		
		}*/
		if( !cfg->cfg_timeHeader ) 	{
			setAppTitle( QString(APP_TITLE) );	
			if( tTimer->isActive() ) 
				tTimer->stop();
		} else 	{
			if( !tTimer->isActive() )
				tTimer->start(1000);		
		}
		initTabs();	
		
		if( mark != cfg->cfg_usrMultiWindow ) {
			QTimer::singleShot( 100, this, SLOT( initReadFm() ) );
		}
	}else 
	{
		delete dlg;
		dlg = NULL;		
	}
} 

void megabox::slotMenuClick()
{
    this->show();
	this->raise();
	slotUpdateTime();
	if( cfg->cfg_timeHeader ) {
		if( !tTimer->isActive() )
			tTimer->start(1000);
	}	
	tabWidget->setCurrentPage(TAB_FAVTAB);
	qApp->processEvents();	
} 
void megabox::slotEfProcKill()
{
	if( !codeComp->cpcp() ) { return; }
	if( cfg->cfg_cKeyToKill ) {
		if( showQ( lng->tr("POP_KILL"), lng->tr("FT_CONFIRM_KILL"), 1) ) {	proc_kill(); }
	}		
}
void megabox::slotEfRefreshProc()
{
	if( !codeComp->cpcp() ) { return; }
	if( cfg->cfg_cKeyToKill ) initReadWindowsProc();
}
void megabox::slotEfHide()
{
	if( !codeComp->cpcp() ) { return; }
	if( cfg->cfg_cKeyToKill ) { this->hide(); }
}	
void megabox::slotEfFmDel()
{
	if( !codeComp->cpcp() ) { return; }
	if( cfg->cfg_cKeyToKill ) {
		if( showQ( lng->tr("FT_DELETE"), lng->tr("FT_CONFIRM_DELETE"), 1) ) { fm_del(); }
	}	
}
void megabox::slotEfRefreshMpkg()
{
	if( !codeComp->cpcp() ) { return; }
	if( cfg->cfg_cKeyToKill ) { mpkg_refresh(); }
}
void megabox::slotEfRefreshMgx()
{
	if( !codeComp->cpcp() ) { return; }
	if( cfg->cfg_cKeyToKill ) { mgx_refresh(); }
}
void megabox::slotEfRefreshFav()
{
	if( !codeComp->cpcp() ) { return; }
	if( cfg->cfg_cKeyToKill ) { fav_refresh(); }
}
void megabox::slotEfRefreshFm()
{
	if( !codeComp->cpcp() ) { return; }
	if( cfg->cfg_cKeyToKill ) { initReadFm(); }
}
void megabox::slotEfChgFmGeometry()
{
	if( !cfg->cfg_usrMultiWindow ) {
		//#ifdef ZN5
		//fmListbox->setFixedWidth ( SCREEN_WHIDTH );
		//#endif
		fmListbox->setGeometry( 0, 0, SCREEN_WHIDTH, sv->height());
		fmListbox2->setGeometry( SCREEN_WHIDTH, 0, 0, sv->height());	
		fmListbox->setFocus();
		slotEfRefreshFm();
		return;
	}

	if( getFileBrowser()->getMode() == MULTI )
		return;
		
	if( !m_bFmState) {	
		//#ifdef ZN5
		//fmListbox->setFixedWidth ( FM_WIDGET_WIDTH );
		//fmListbox2->setFixedWidth (SCREEN_WHIDTH-FM_WIDGET_WIDTH );
		//#endif
		fmListbox->setGeometry( 0, 0, FM_WIDGET_WIDTH, sv->height());
		fmListbox2->setGeometry( FM_WIDGET_WIDTH, 0, SCREEN_WHIDTH-FM_WIDGET_WIDTH, sv->height());	
		fmListbox->setFocus();
	} else {
		//#ifdef ZN5
		//fmListbox->setFixedWidth ( SCREEN_WHIDTH-FM_WIDGET_WIDTH );
		//fmListbox2->setFixedWidth ( FM_WIDGET_WIDTH );
		//#endif
		fmListbox->setGeometry( 0, 0, SCREEN_WHIDTH-FM_WIDGET_WIDTH, sv->height());	
		fmListbox2->setGeometry( SCREEN_WHIDTH-FM_WIDGET_WIDTH, 0, FM_WIDGET_WIDTH, sv->height());
		fmListbox2->setFocus();
	}	
	m_bFmState = !m_bFmState;
}
void megabox::slotEfOrder(int tabIndex, bool isUp)
{
	if(tabIndex == TAB_MGX ) {
		int i = mgxListbox->currentItem();
		int j = (int)mgxListbox->count();
		int k = i;
		if( isUp ) {
			if( --k < 0 ) k = j-1;
			mgxListbox->moveItem( i, k );	
			mgxListbox->setCurrentItem(k, false );
		} else {
			if( ++k > j-1 ) k = 0;
			mgxListbox->moveItem( i,k );	
			mgxListbox->setCurrentItem(k, false );				
		}			
	}
}
bool megabox::eventFilter(QObject *obj, QEvent *pEvent)
{
	QKeyEvent *pKey = (QKeyEvent*)pEvent;
	
	if (QEvent::KeyPress == pEvent->type()) {
		if( pKey->key() == KEY_CENTER ) {
			if( obj == mgxListbox ) {
				switchTab( TAB_MGX );
				if( m_eMgxListOrderType == TYPE_START_ORDER ) {
					m_eMgxListOrderType = TYPE_ITME_SELECTED;
					return true;
				} else 
				if( m_eMgxListOrderType == TYPE_ITME_SELECTED ) {
					m_eMgxListOrderType = TYPE_START_ORDER ;
					return true;
				} else 
				{
					return QWidget::eventFilter( obj, pEvent ); 
				}			
			}
		}	
		if( pKey->key() == KEY_UP || pKey->key() == KEY_DOWN ) {
			if( obj == mgxListbox ) {
				if( m_eMgxListOrderType == TYPE_ITME_SELECTED ) {
					slotEfOrder( TAB_MGX, pKey->key()==KEY_UP ? true : false) ;
					return true; 
				} else {
					return QWidget::eventFilter( obj, pEvent ); 
				}			
			}		
		}
	}	
	if (QEvent::KeyRelease == pEvent->type()) 
    {	    
		if( pKey->key() == KEYCODE_CLEAR ) {
			if(obj == procListbox) { slotEfProcKill(); return true; }
			if(obj == fmListbox || obj == fmListbox2) { slotEfFmDel(); return true; }		
		}
		if( pKey->key() == KEYCODE_GREEN ) {
			if(obj == procListbox) { slotEfRefreshProc() ;return true; }
			if(obj == fmListbox || obj == fmListbox2) { slotEfChgFmGeometry(); return true; }
			if(obj == mpkgListbox) { slotEfRefreshMpkg(); return true;}
			if(obj == mgxListbox) { slotEfRefreshMgx(); return true;}
			if(obj == favObjListbox) { slotEfRefreshFav(); return true;}		
		}
		if( pKey->key() == KEY_CENTER ) {
			if( obj == mgxListbox && m_eMgxListOrderType != TYPE_ORDER_OVER) {
				return true;
			}
		}
		if( pKey->key() == KEY_UP || pKey->key() == KEY_DOWN ) {
			if( obj == mgxListbox ) {
				if( m_eMgxListOrderType == TYPE_ITME_SELECTED ) {
					return true; 
				} else {
					return QWidget::eventFilter( obj, pEvent ); 
				}			
			}		
		}
		if( pKey->key() == KEYCODE_RED ) { slotEfHide(); /*return true;*/}
		
		return QWidget::eventFilter( obj, pEvent ); 
	}
	return QWidget::eventFilter( obj, pEvent ); 
}
void megabox::regNotify()
{
	showNotify(lng->tr("POP_DONATE"), lng->tr("POP_THIS_FUNCTION_NEED_DONATE"), 2, 3);
}
void megabox::fileModCore(QString title, QString text, QString cmdStr1, QString cmdStr2, FILE_MOD_TYPE type, bool isShowProcess)
{
	char exec_buf[1000];	
	switch ( type ) {
		case FILE_PASTE :
			sprintf(exec_buf, "cp -fR '%s' '%s'", (const char*)cmdStr1.local8Bit(), (const char*)cmdStr2.local8Bit() );
			break;
		case FILE_MOVE :
			sprintf(exec_buf, "mv -f '%s' '%s'", (const char*)cmdStr1.local8Bit(), (const char*)cmdStr2.local8Bit() );
			break;
		case FILE_DELETE :
			sprintf(exec_buf, "rm -rf '%s'", (const char*)cmdStr1.local8Bit() );
			break;
		default :
			return;	
	}	
	deathDlg *execDlg = new deathDlg(title, text, exec_buf);
	execDlg->exec();
	delete execDlg; 
	execDlg = NULL;	
	qApp->processEvents();		
}		

void megabox::loadJava(const QString &gname, bool isSD, bool isMult, TaskListBox *lb, int tabIndex)
{
	#ifndef NEW_JAVA_LIST
	QString exec;
	if(isSD) {
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
		for(int i=0; i<lb->count(); i++ ) {
			pItem= (ZTaskItem*)lb->item(i); 
			line = pItem->getOriName();
			if( line != "" ) {
				if( line.find(exec) > -1 && line.find("/usr/SYSjava/kvm") > -1 ) {
					isLoaded = true;
					pid = pItem->getPid();
					break;
				}			
			}		
		}		
		
		if( isLoaded && pid > -1 ) {
			if( showQ( "", lng->tr("FT_RUN_JAVA_INFO"), 1) ) {
				system( QString("exec /usr/SYSjava/kvm -launch %1 &").arg(exec) );
			} else {
				pidToTop(pid);
			}	
		} else {
			system( QString("exec /usr/SYSjava/kvm -launch %1 &").arg(exec) );
		}	
	} else {
		system( QString("exec /usr/SYSjava/kvm -launch %1 &").arg(exec) );	
	#endif
	#ifdef NEW_JAVA_LIST	
		launchAppObject( gname );
	#endif	
	#ifndef NEW_JAVA_LIST	
	}
	#endif	
	qApp->processEvents();		
}
void megabox::loadObj(const QString &gname, ITEMTYPE t, bool isMult, TaskListBox *lb, int tabIndex)
{
	if(t == ITEM_FAV_OBJ)
	{
		#if defined(E8) || defined(ZN5)
		if( favDB->getObjectType(gname) == "1" )
		{
			QString exec =  favDB->getObjectExec( gname );
			if ( isMult ) {				
				system( QString("exec /usr/SYSjava/kvm -launch %1 &").arg(exec) );	
			} else {
				bool isLoaded = false;
				int pid = -1;	
				ZTaskItem* pItem = NULL;
				QString line = "";
				for(int i=0; i<lb->count(); i++ ) {
					pItem= (ZTaskItem*)lb->item(i); 
					line = pItem->getOriName();
					if( line != "" ) {
						if( line.find(exec) > -1 && line.find("/usr/SYSjava/kvm") > -1 ) {
							isLoaded = true;
							pid = pItem->getPid();
							break;
						}			
					}		
				}
				if( isLoaded && pid>-1)  {
					if( showQ( "", lng->tr("FT_RUN_JAVA_INFO"), 1) ) {
						system( QString("exec /usr/SYSjava/kvm -launch %1 &").arg(exec) );
					} else {
						pidToTop(pid);
					}
				}else {
					system( QString("exec /usr/SYSjava/kvm -launch %1 &").arg(exec) );
				}					
			}	
		}
		else
		#endif
		launchAppObject(gname); 
	} 
	else if(t == ITEM_MGX_OBJ)
	{
		QString dir = mgxDB->getObjectDir(gname);
		QString exec = mgxDB->getObjectExec(gname);
		system(QString("exec '%1/%2' &").arg(dir).arg(exec) );
	}
	else if(t == ITEM_MPKG_OBJ) 
	    launchAppObject(gname); 
	else return;
	
	qApp->processEvents();		
}
FileBrowser *megabox::getFileBrowser()
{
	if( !cfg->cfg_usrMultiWindow ) {
		return fmListbox;
	}		
	
	if( m_bFmState ) 
		return fmListbox;
	else
		return fmListbox2;
}			  
#ifndef RAISE_PHONE
void megabox::fixKvmStats()
{
	if ( procListbox->getKvmStats() )
	{
		#ifdef DEBUG
		qDebug("!!! HIDE KVM !!!");	
		#endif	
		SETUP_Utility_Impl util;
		SETUP_Utility::SLIDE_CLOSED_SETTING_E iSliderAction = util.getSlideClosedSetting();

		if ( iSliderAction != SETUP_Utility::SLIDE_CLOSE_ALL )
			util.setSlideClosedSetting(SETUP_Utility::SLIDE_CLOSE_ALL);	
		
		QCopChannel::send( "EZX/System", "sliderClosed" );

		if ( iSliderAction != SETUP_Utility::SLIDE_CLOSE_ALL )
		{
			qApp->processEvents();
			qApp->processEvents();

			util.setSlideClosedSetting( iSliderAction );
		}
	}
}
#endif
void megabox::fixFmWidget()
{
	if( !cfg->cfg_usrMultiWindow ) {	
		//#ifdef ZN5
		//fmListbox->setFixedWidth ( SCREEN_WHIDTH );
		//#endif
		//fmListbox->setGeometry( 0, 0, SCREEN_WHIDTH-5, sv->height());
		//fmListbox2->setGeometry( SCREEN_WHIDTH-5, 0, 5, sv->height());	
		fmListbox->setGeometry( 0, 0, SCREEN_WHIDTH, sv->height());
		fmListbox2->setGeometry( SCREEN_WHIDTH, 0, 0, sv->height());			
		fmListbox->setFocus();
		return;
	} 
	
	if( getFileBrowser()->getMode() == MULTI )
		return;
	
	//cout<<" =========== fixFmWidget() setGeometry() ============"<<endl;
	
	if( m_bFmState) {
		//#ifdef ZN5
		//fmListbox->setFixedWidth ( FM_WIDGET_WIDTH );
		//fmListbox2->setFixedWidth (SCREEN_WHIDTH-FM_WIDGET_WIDTH );
		//#endif
		fmListbox->setGeometry( 0, 0, FM_WIDGET_WIDTH, sv->height());	
		fmListbox2->setGeometry( FM_WIDGET_WIDTH, 0, SCREEN_WHIDTH-FM_WIDGET_WIDTH, sv->height());		
		fmListbox->setFocus();
	} else {
		//#ifdef ZN5
		//fmListbox->setFixedWidth ( SCREEN_WHIDTH-FM_WIDGET_WIDTH );
		//fmListbox2->setFixedWidth ( FM_WIDGET_WIDTH );
		//#endif
		fmListbox->setGeometry( 0, 0, SCREEN_WHIDTH-FM_WIDGET_WIDTH, sv->height());	
		fmListbox2->setGeometry( SCREEN_WHIDTH-FM_WIDGET_WIDTH, 0, FM_WIDGET_WIDTH, sv->height());		
		fmListbox2->setFocus();
	}		
}	

/**********************************************************************************/
/**********************************************************************************/

void megabox::initReadWindowsProc()
{
 	#ifdef DEBUG
    qDebug("===== initReadWindowsProc() \n");
	#endif  
    QString origin_name = getAppTitle();
	setAppTitle(lng->tr("FT_REFRESHING"));	
	
	procListbox->RefreshTaskList(false);
	
	if(cfg->cfg_showNativeIcon) { 
		procListbox->showNativeIcon();
	}	
	#ifdef NEW_JAVA_LIST
	procListbox->buildJavaListNew();
	#else
	procListbox->kvmToInfo();
	#endif
	if( cfg->cfg_showNativeMgxInfo ) {
		showMgxInfoInProc();
	}
	
	setAppTitle(origin_name);
}

void megabox::updateTask()
{
 	#ifdef VE66
	    initReadWindowsProc();
	#else	
		#ifdef DEBUG
		qDebug("===== updateTask() \n");
		#endif  
		QString origin_name = getAppTitle();
		setAppTitle(lng->tr("FT_REFRESHING"));	

		procListbox->updateTaskList();
		if(cfg->cfg_showNativeIcon) {
			procListbox->updateNativeIcon();
		}
		#ifdef NEW_JAVA_LIST
		procListbox->updateJavaListNew();
		#else
		procListbox->updateKvmToInfo();
		#endif
		if( cfg->cfg_showNativeMgxInfo ) {
			showMgxInfoInProc();
		}
		setAppTitle(origin_name);
    #endif	
}

void megabox::initReadAllProc()
{
    QString origin_name = getAppTitle();
	setAppTitle(lng->tr("FT_REFRESHING"));	
	procListbox->RefreshTaskList(true);
	setAppTitle(origin_name);
}
void megabox::showMgxInfoInProc()
{
	int index = 0;
	if( mgxListbox->getMode() == MULTI ) {
		index = 1;
	}
	for( unsigned int i=0; i<procListbox->count(); ++i ) {
		ZTaskItem* pItem= (ZTaskItem*)procListbox->item (i); 
		if( pItem->getType() == ITEM_NATIV ) {
			QString cmd = pItem->getOriName();
			for( unsigned int j=0; j<mgxListbox->count();++j) {
				ZTaskItem* qItem= (ZTaskItem*)mgxListbox->item (j); 
				if( qItem->getType() == ITEM_MENU_OBJ ) { 
					continue; 
				}
				if( cmd.find( mgxDB->getObjectExec(qItem->getGname()) ) > -1 ) {
					pItem->setSubItem ( 1, 0, qItem->getSubItemText(index+1, 0) );
					if( !cfg->cfg_showMgxBigIcon ) {
						pItem->setPixmap( 0, qItem->getPixmap(index) );
					} else {
						QPixmap pm = qItem->getPixmap(index);
						QImage img = pm.convertToImage();
						img = img.smoothScale(24,24);
						pm.convertFromImage(img);	
						pItem->setPixmap( 0, pm );
					}	
					break;	
				}		
			}		
		}	
	}
}
void megabox::proc_click(int index)
{
	ZTaskItem* pItem= (ZTaskItem*)procListbox->item (index); 
	if (pItem == NULL) return;	
	switch ( pItem->getType() ) 
	{
		case ITEM_SPECIAL:
			// fix for message
			if ( (pItem->getOriName() ).find("messaging") > -1)  {
				launchAppObject(AM_APP_UID_MESSAGING); 
			}
			else if(  ( ( pItem->getOriName() ).find("flowplayer") > -1 ) || ( ( pItem->getOriName() ).find("xp") > -1 ) )  {
				AM_MainMenuInterface menu;
				menu.ShowMainMenu();	
			}			
			#ifdef FIX_ITEM_SPECIAL
			else if( ( ( pItem->getOriName() ).find("phone") > -1 ) || ( ( pItem->getOriName() ).find("xp") > -1 ) ) {
				return;
			}			
			#endif
			else {
				pidToTop( pItem->getPid());
			}	
		break;
		case ITEM_JAVA :
		{
			if( cfg->cfg_sendReturnToIdle) {
				// 修正，，，单个java时不杀死QT
				int j = -1;
				for( unsigned int i=0; i<procListbox->count(); ++i) {
					ZTaskItem* pItem= (ZTaskItem*)procListbox->item (i); 
					if( pItem->getType() == ITEM_JAVA ) { j++;}				
				}
				if( j > 0 ) {
					if( codeComp->cpcp() ) {
						sendReturnToIdle();
						#ifndef RAISE_PHONE	
						fixKvmStats();
						#endif
					}
				}	
			}	
			#ifdef NEW_JAVA_LIST
			javaToTop(pItem->getGname());
			#else
			pidToTop( pItem->getPid());
			#endif		
		}	
		break;
		case ITEM_NATIV :
			pidToTop( pItem->getPid());
		break;
		default :
			return;
	}
	qApp->processEvents();			
	#ifdef CLICK_NO_HIDE
    slotQuit();
    #else
	slotHide();
	#endif	
}
void megabox::proc_pid2top() 
{
    #ifdef NEW_JAVA_LIST
	if(procListbox->getHlType() == ITEM_JAVA) return;
	#endif	
	ZTaskItem* pItem= (ZTaskItem*)procListbox->item( procListbox->currentItem() ); 
	if (pItem == NULL) return;	
	switch ( pItem->getType() ) 
	{
		case ITEM_SPECIAL:
		{
			if ( (pItem->getOriName() ).find("messaging") > -1)  {
				launchAppObject(AM_APP_UID_MESSAGING); 
			}
			else if(  ( ( pItem->getOriName() ).find("flowplayer") > -1 ) || ( ( pItem->getOriName() ).find("xp") > -1 ) )  {
				AM_MainMenuInterface menu;
				menu.ShowMainMenu();	
			}			
			#ifdef FIX_ITEM_SPECIAL
			else if( ( ( pItem->getOriName() ).find("phone") > -1 ) || ( ( pItem->getOriName() ).find("xp") > -1 ) ) {
				return;
			}			
			#endif
			else {
				pidToTop( pItem->getPid());
			}
		}
		break;
		case ITEM_JAVA :
		case ITEM_NATIV :
		default :
			pidToTop( pItem->getPid());
		break;
	}
	qApp->processEvents();			
	#ifdef CLICK_NO_HIDE
    slotQuit();
    #else
	slotHide();
	#endif	
}
void megabox::proc_idle2pidToTop() 
{
	if( !codeComp->cpcp() ) {
		regNotify();
		return;
	}
	ZTaskItem* pItem= (ZTaskItem*)procListbox->item( procListbox->currentItem() ); 
	if (pItem == NULL) return;	
	switch ( pItem->getType() ) 
	{
		case ITEM_SPECIAL:
		{
			// fix for message
			if ( (pItem->getOriName() ).find("messaging") > -1)  {
				launchAppObject(AM_APP_UID_MESSAGING); 
			}else 
			if(  ( ( pItem->getOriName() ).find("flowplayer") > -1 ) || ( ( pItem->getOriName() ).find("xp") > -1 ) )  {
				AM_MainMenuInterface menu;
				menu.ShowMainMenu();	
			}	
			#ifdef FIX_ITEM_SPECIAL
			else if( ( ( pItem->getOriName() ).find("phone") > -1 ) || ( ( pItem->getOriName() ).find("xp") > -1 ) ) {
				return;
			}			
			#endif
			else {
				pidToTop( pItem->getPid());
			}
		}		
		break;
		case ITEM_JAVA :
		{
			sendReturnToIdle();
			#ifndef RAISE_PHONE	
			fixKvmStats();
			#endif	
			#ifdef NEW_JAVA_LIST
			javaToTop(pItem->getGname());
			#else
			pidToTop( pItem->getPid());
			#endif		
		}	
		break;
		case ITEM_NATIV :
			pidToTop( pItem->getPid());
		break;
		default :
			return;
	}
	#ifdef CLICK_NO_HIDE
    slotQuit();
    #else
	slotHide();
	#endif		
}
void megabox::proc_quit()
{
    #ifdef NEW_JAVA_LIST
	if(procListbox->getHlType() == ITEM_JAVA) return;
	#endif
	QString origin_name = getAppTitle();
	setAppTitle(lng->tr("FT_QUITING_PROC"));
	
	int j = procListbox->currentItem(); 
	
	pidQuit( procListbox->getHlPid() );
	
	for ( int i = 0; i < 10; i++ )   {
        if ( !processIsRuning( procListbox->getHlPid() ) )  {
		    procListbox->removeItem (j);
            break;
		}
		for ( int j = 0; j < 50; j++ ) {
			qApp->processEvents();
			usleep(50);
		}	
	}
	setAppTitle(origin_name);	
} 

void megabox::proc_kill()
{
    #ifdef NEW_JAVA_LIST
	if(procListbox->getHlType() == ITEM_JAVA) return;
	#endif
	
	if ( killProc( procListbox->getHlPid() ) ) 
		qDebug("=== kill proc ok ===\n");
	else 
		qDebug("=== kill proc error ===\n");

	QTimer::singleShot( 500, this, SLOT( initReadWindowsProc() ) );	
} 

void megabox::proc_setFilter()
{
    if( !codeComp->cpcp() ) {
	    regNotify();
		return;
	}
	//if( tabItemError(curTab) ) return;
	ZTaskItem* pItem= (ZTaskItem*)procListbox->item( procListbox->currentItem() ); 
	if( pItem == NULL ) {
		return;
	}
	int ret = showQ(lng->tr("FT_ADD_FILTER"), lng->tr("SURE_TO_ADD")+pItem->getSubItemText(1,0), 1);
	if(ret != 1) return;
	
    cfg->procFilterStr = cfg->procFilterStr.append(pItem->getOriName()+"||");
	cfg->saveCfg();
	
    QTimer::singleShot( 100, this, SLOT( initReadWindowsProc() ) );
}

void megabox::proc_resetFilter()
{
	if( !codeComp->cpcp() ) {
	    regNotify();
		return;
	}
	QString listStr = cfg->procFilterStr;
	if( listStr == "" ) {
		showNotify( lng->tr("FT_RESET_FILTER"), lng->tr("FT_NO_FILTER") );
		return;	
	}
	QStringList list = QStringList::split( "||", listStr );
	QString str = "";
	
	ZSingleSelectDlg *dlg = new ZSingleSelectDlg( lng->tr("FT_RESET_FILTER") ,NULL);
	ZListBox *lb = dlg->getListBox();
	lb->enableMarquee(ZListBox::LISTITEM_REGION_A, true);
    lb->enableMarquee(ZListBox::LISTITEM_REGION_B, true);
	ZSoftKey *softKey = dlg->getSoftKey();
	softKey->setText(ZSoftKey::LEFT, lng->tr("FT_DELETE") );	
	
	dlg->addItemsList(list);
	
	if(dlg->exec() == ZPopup::Accepted)  {
		int i = dlg->getCheckedItemIndex();
		delete dlg;
		dlg = NULL;
		str = list[i]+"||";
	} else {
		delete dlg;
		return;
	}	
	listStr = listStr.replace( str, "" );
	cfg->procFilterStr = listStr;
	cfg->saveCfg();	

    QTimer::singleShot( 100, this, SLOT( initReadWindowsProc() ) );
	qApp->processEvents();	
}

void megabox::proc_info()
{
    #ifdef NEW_JAVA_LIST
	if(procListbox->getHlType() == ITEM_JAVA) return;
	#endif	
	
	ZTaskItem* pItem= (ZTaskItem*)procListbox->item( procListbox->currentItem() ); 
	if( pItem == NULL ) {
		return;
	}
	
	procInfoDlg *dlg = new procInfoDlg( (int)pItem->getPid() );
	dlg->exec();
	delete dlg;
	dlg = NULL;
}

void megabox::proc_state()
{
	if( tTimer->isActive() )
		tTimer->stop();  
		
	stateDlg *dlg = new stateDlg();
	dlg->exec();
	delete dlg;
	dlg = NULL;
	
	if( cfg->cfg_timeHeader ) {
		if( !tTimer->isActive() )
			tTimer->start(1000); 
	}	
}
void megabox::proc_reboot()
{
	int ret = showQ("", lng->tr("FT_CONFIRM_REBOOT"), 1);
	if( ret != 1 ) return;
	
	system("sync");   //更新 i-node 表，并将缓冲文件写到硬盘中
	reboot(RB_AUTOBOOT);
}
void megabox::proc_poweroff()
{
	int ret = showQ("", lng->tr("FT_CONFIRM_POWEROFF"), 1);
	if( ret != 1 ) return;
	
	system("sync");   //更新 i-node 表，并将缓冲文件写到硬盘中
	reboot(RB_POWER_OFF);
}
/**********************************************************************************/
/**********************************************************************************/

void megabox::fav_refresh()
{
    QString origin_name = getAppTitle();
	setAppTitle(lng->tr("FT_REFRESHING"));
	
	favDB->flush();
	
	favObjListbox->clear();
	//favObjListbox->setDB(QString(FAV_OBJ_PATH));
	favObjListbox->bulidAppList();
	
	setAppTitle(origin_name);
}

void megabox::fav_add()
{
	QString title = lng->tr("FT_ADD_APP");
	bool mark = codeComp->cpcp();//codeCmp();
	
	appAddDlg *dlg = new appAddDlg( title, appAddDlg::DT_addFavApp, mark );
	dlg->exec();
	bool isCancel = dlg->isCancel;
	delete dlg;
	dlg = NULL;
	if( !isCancel ) {
		favDB->flush();
		
		QTimer::singleShot( 100, this, SLOT( fav_refresh() ) );	
	}	
}

void megabox::fav_del()
{
	ZTaskItem* pItem= (ZTaskItem*)favObjListbox->item( favObjListbox->currentItem() ); 	
	if( pItem == NULL ) {
		return;
	}
	favDB->clearGroup( pItem->getGname() );
	favDB->flush();
	
    QTimer::singleShot( 100, this, SLOT( fav_refresh() ) );	
}

void megabox::fav_rename()
{
    if( !codeComp->cpcp() ) {
	    regNotify();
		return;
	}
	ZTaskItem* pItem= (ZTaskItem*)favObjListbox->item( favObjListbox->currentItem() ); 
	if( pItem == NULL ) {
		return;
	}
	QString fileName = pItem->getSubItemText(1,0);
	if (fileName == QString::null) return;
	
	ZSingleCaptureDlg* dlg = new ZSingleCaptureDlg(
	    lng->tr("FT_RENAME"), lng->tr("FT_CUR_NAME")+fileName,
	    (ZSingleCaptureDlg::CaptureDlgType)0, NULL, "ZSingleCaptureDlg", true, 0, 0);
	ZLineEdit *line = dlg->getLineEdit();
	line->setText(fileName);
	int ret=dlg->exec();

	QString newName = "";
	if (ret == 1) 	{
		newName = dlg->getResultText().stripWhiteSpace();
		delete dlg;
	} else 	{
		delete dlg;
		return;
	}
	if( isValidFileName(newName) ) {
		showQ(lng->tr("FT_ERR0R"), lng->tr("FT_INVALID"), 2);
		return;
	}
	favDB->writeEntry(pItem->getGname(), "Name", newName);
	favDB->flush();

	QTimer::singleShot( 100, this, SLOT( fav_refresh() ) );	
}

/**********************************************************************************/
/**********************************************************************************/

void megabox::mpkg_refresh()
{
    QString origin_name = getAppTitle();
	setAppTitle(lng->tr("FT_REFRESHING")); 	
	
	mpkgListbox->clear();
	mpkgListbox->setDB( QString(PHONE_INSTALL_DB) );
	mpkgListbox->bulidAppList();
	mpkgListbox->setDB( QString(SD_INSTALL_DB) );
	mpkgListbox->bulidAppList();	

	setAppTitle(origin_name);	
}

void megabox::app_props()
{
	int index=-1;
	int tabIndex = tabWidget->currentPageIndex();
	
	ZTaskItem* pItem = NULL;
	switch(tabIndex) {
		case TAB_MPKG:
			index = mpkgListbox->currentItem();
			if(index == -1) return;
			pItem= (ZTaskItem*)mpkgListbox->item(index);	
			break;
		case TAB_MGX:
			index = mgxListbox->currentItem();
			if(index == -1) return;
			pItem= (ZTaskItem*)mgxListbox->item(index);	
			break;
		default :
			return;	
	}

	if (pItem == NULL) return;
	
    ZConfig *cfg = NULL;
	QString text = QString::null;
	QString uid = pItem->getGname();
	
	if (tabIndex == TAB_MPKG) {
		if( pItem->isSD() ) cfg = new ZConfig(SD_INSTALL_DB);
		else cfg = new ZConfig(PHONE_INSTALL_DB); 
	}	
    if(tabIndex == TAB_MGX)
	    cfg = new ZConfig(MGX_OBJ_PATH);
		
	QString dir = cfg->readEntry(uid, "Directory", "");
	unsigned int size = getDirSize( (const char*)dir.local8Bit() );	
	
	text += lng->tr("PROP_NAME");
	text += cfg->readEntry(uid, "Name", "")+"\n";
	text += lng->tr("PROP_PATH");
	text += cfg->readEntry(uid, "Directory", "")+"\n";
	text += lng->tr("PROP_TYPE");
	text += cfg->readEntry(uid, "AppType", "")+"\n";
	text += lng->tr("PROP_SIZE");
	text += size2string(size)+"\n";
	text += lng->tr("PROP_VERSION");
	text += cfg->readEntry(uid, "Version", "")+"\n";	
	text += lng->tr("PROP_AUTHOR");
	text += cfg->readEntry(uid, "Vendor", "")+"\n";
	text += lng->tr("PROP_COMMENT")+"\n";	
	text += cfg->readEntry(uid, "Comment", "")+"\n";	
	delete cfg;
	cfg = NULL;
	
	showQ(lng->tr("FT_PROPS"), text, 2);
}

void megabox::mpkg_rename()
{
	ZTaskItem* pItem= (ZTaskItem*)mpkgListbox->item( mpkgListbox->currentItem() );
	if (pItem == NULL) return;

	ZConfig *cfg = NULL;
	if( pItem->isSD() ) 
		cfg = new ZConfig(SD_INSTALL_DB);
	else 
		cfg = new ZConfig(PHONE_INSTALL_DB);

	QString uid = pItem->getGname(); 
	QString fileName = cfg->readEntry(uid, "Name", "");
	if (fileName == "") {
		delete cfg;
		return;	
	}
	ZSingleCaptureDlg* dlg = new ZSingleCaptureDlg(
	    lng->tr("FT_RENAME"), lng->tr("FT_CUR_NAME")+fileName,
	    (ZSingleCaptureDlg::CaptureDlgType)0, NULL, "ZSingleCaptureDlg", true, 0, 0);
	ZLineEdit *line = dlg->getLineEdit();
	line->setText(fileName);
	int ret=dlg->exec();
	
	QString newName = QString::null;
	if (ret == 1)  {
		newName = dlg->getResultText().stripWhiteSpace();//).local8Bit();
		delete dlg;
	} else
	{
		delete dlg;
		return;
	}

	if( isValidFileName(newName) ) {
		showQ(lng->tr("FT_ERR0R"), lng->tr("FT_INVALID"), 2);
		return;
	}

	cfg->writeEntry(uid, "Name", newName);
	cfg->flush();
	delete cfg;

	mpkg_refresh();	
}
void megabox::java_set_profile()
{
#if defined(TAB3_JAVA) && !defined(VE66)
	if( !codeComp->cpcp() ) {
		regNotify();
		return;
	}	
	QString profile;

	ZTaskItem* pItem= (ZTaskItem*)mpkgListbox->item( mpkgListbox->currentItem() );
	if (pItem == NULL) return;		

	ZConfig *regCfg = NULL;
	if( pItem->isSD() ) 
		regCfg = new ZConfig(SD_INSTALL_DB);
	else 
		regCfg = new ZConfig(PHONE_INSTALL_DB);
	
	QString destUid = pItem->getGname();
	QString path = regCfg->readEntry(destUid, "Directory", "");
	delete regCfg;
	regCfg = NULL;
	
	QDir pdir(JAVA_NET_PROFILE_PATH);
	QStringList plist = pdir.entryList(QDir::Files);
	ZSingleSelectDlg *dlg = new ZSingleSelectDlg(lng->tr("FT_SET_PROFILE"),NULL);
	dlg->addItemsList(plist);
	if(dlg->exec() == ZPopup::Accepted) {
		int i = dlg->getCheckedItemIndex();
		profile = plist[i].local8Bit();	
		delete dlg;		
	} else
	{
		delete dlg;
		return;
	}
	QString file = QString("%1/registry.txt").arg(path);
	QString key = QString("Connection-Profile");
	QString type = QString(":");
	if( !setLineConfig(file, key, type, profile) )
		qDebug("============= set java profile error =============");
#endif		
}

#ifdef TAB3_JAVA
void megabox::java_back_up()
{
	ZTaskItem* pItem= (ZTaskItem*)mpkgListbox->item( mpkgListbox->currentItem() );
	if (pItem == NULL) return;

	ZConfig *regCfg = NULL;
	if( pItem->isSD() ) regCfg = new ZConfig(SD_INSTALL_DB);
	else regCfg = new ZConfig(PHONE_INSTALL_DB);
	
	QString destUid = pItem->getGname();
	
	QString name = regCfg->readEntry(destUid, "Name", "");
	QString path = regCfg->readEntry(destUid, "Directory", "");
	QString text(lng->tr("FT_CONFIRM_BACKUP")+"\n"+name+"\n"+path+"\n");

	int ret = showQ(lng->tr("FT_BACKUP"), text, 1);
	if (ret != 1)  {
	    delete regCfg;
		return;
	}
	
	QDir dir;
	if( !dir.exists( SD_BACKUP_DIR ) )
		dir.mkdir( SD_BACKUP_DIR );
	if( !dir.exists( PHONE_BACKUP_DIR ) )	
		dir.mkdir( PHONE_BACKUP_DIR );
	
	QString fromPath, destName;
	
	QFile file(path+"/registry.txt");
    if ( file.open( IO_ReadOnly ) ) 
    {
        QTextStream stream( &file );
        QString line;
        while ( !stream.atEnd() ) 
		{
            line = stream.readLine();	
			if( line.find("Jar-Path:") > -1 ) {
				fromPath = line.replace("Jar-Path:", "");
				fromPath = fromPath.stripWhiteSpace();
				break;
			}	
		}
	}
	
	destName = fromPath;
	destName = destName.remove(0, destName.findRev("/")+1 );
	
	QString title = "";
	text = lng->tr("POP_PLEASE_WAIT"); 

	if( pItem->isSD() ) 
		fileModCore(title, text, fromPath, QString("%1/%2").arg(SD_BACKUP_DIR).arg(destName), FILE_PASTE );
	else
		fileModCore(title, text, fromPath, QString("%1/%2").arg(PHONE_BACKUP_DIR).arg(destName), FILE_PASTE);
}
#endif

void megabox::mpkg_remove()
{
	ZTaskItem* pItem= (ZTaskItem*)mpkgListbox->item(  mpkgListbox->currentItem() );
	if (pItem == NULL) return;

	ZConfig *regCfg = NULL;
	#ifdef TAB3_JAVA
	QString indexFile;
	#endif
		
	if( pItem->isSD() ) { 
		regCfg = new ZConfig(SD_INSTALL_DB);
		#ifdef TAB3_JAVA
		indexFile = QString("/mmc/mmca1/.system/java_%1/JAMInfo/index").arg( getIMEI() );//usrIMEI);
		#endif
	} else {
		regCfg = new ZConfig(PHONE_INSTALL_DB);
		#ifdef TAB3_JAVA
		indexFile = QString("/ezxlocal/download/mystuff/.system/java/JAMInfo/index");
		#endif		
	}	
	QString destUid = pItem->getGname(); 
	#ifdef TAB3_JAVA
		#ifdef ZN5
		QString execMark = regCfg->readEntry(destUid, "JavaId", "");
		#else
		QString execMark = regCfg->readEntry(destUid, "ExecId", "");
		#endif
	#endif
	
	QString text;
	text += lng->tr("FT_CONFIRM_DELETE")+"\n";
	text += lng->tr("PROP_NAME")+regCfg->readEntry(destUid, "Name", "")+"\n";
	text += lng->tr("PROP_PATH")+regCfg->readEntry(destUid, "Directory", "")+"\n";	
	
	int ret = showQ(lng->tr("FT_DELETE"), text, 1);

	if (ret != 1) {
	    delete regCfg;
		return;
	}
	
	QString itemStr = QString::null;
	QString destMenuUid = QString::null;
	
	QStringList grouplist;
	QStringList::ConstIterator p;

	ZConfig *sdMenu = NULL;
	ZConfig *menuCfg = NULL;
	if ( pItem->isSD() )  
		sdMenu = new ZConfig(SD_MENU_CFG);//, false);
    else 
		delete sdMenu;

	system( QString("rm -rf %1").arg(regCfg->readEntry(destUid, "Directory", "")) );	
	/******** clear reg information in reg file ******/
	regCfg->clearGroup(destUid);
	regCfg->flush();	
	delete regCfg;
	
	/*****  clear reg information in menu tree ******/
	
	menuCfg = new ZConfig(MENU_TREE);
	menuCfg->getGroupsKeyList(grouplist);
	for (p = grouplist.begin(); p != grouplist.end(); ++p ) {
        QString tmpItem = menuCfg->readEntry(*p, "Items", "");
		if(tmpItem.find(destUid) > -1) 	{
			itemStr = tmpItem;
			destMenuUid = *p;
			break;	
		}
    }	
	
	if( !itemStr.endsWith(";") )
		itemStr += ";";		
	itemStr = itemStr.replace(destUid+";", "");
	
	menuCfg->writeEntry(destMenuUid, "Items", itemStr);
	menuCfg->flush();	
	delete menuCfg;	
	
	if(sdMenu != NULL) // for sd menu cfg
	{
	    sdMenu->clearGroup(destUid);
	    sdMenu->flush();	
		delete sdMenu;
    }	
	#ifdef TAB3_JAVA
	// delete file in java index file
	QString deleteMark = execMark+"||";
	if( ! deleteKeyLine(indexFile, deleteMark) )
		qDebug("======= delete java index info error =========\n");		
	#endif	

	mpkg_refresh();
	
	showNotify(lng->tr("POP_OK_TITLE"), lng->tr("POP_OK_TEXT"), 1, 2);
	#ifdef VE66
	system("kill `pidof xp` >/dev/null 2>&1");	
	#else	
	system("kill `pidof flowplayer` >/dev/null 2>&1");	
	#endif	
}
void megabox::mpkg_set_permission()
{
    #ifdef VE66_OLD_MP
	ZTaskItem* pItem= (ZTaskItem*)mpkgListbox->item( mpkgListbox->currentItem() );
	if (pItem == NULL) return;

	int mark = 0;
	
	ZConfig *cfg = NULL;
	if( pItem->isSD() ) cfg = new ZConfig(SD_INSTALL_DB);
	else cfg = new ZConfig(PHONE_INSTALL_DB);

	QString uid = pItem->getGname();

	ZSingleSelectDlg *dlg = new ZSingleSelectDlg(lng->tr("POP_SET_PERMISSION"), lng->tr("FT_PLEASCHOOSE"));
    QStringList strlist;
    strlist += lng->tr("FT_PER_EZX");
    strlist += lng->tr("FT_PER_ROOT");
    dlg->addItemsList(strlist);
    dlg->exec();
    int ret = dlg->getCheckedItemIndex();
    delete dlg;			
	if (ret == 0) {
	    if (cfg->writeEntry(uid, "GroupID", "ezx")) mark++;
		if (cfg->writeEntry(uid, "UserID", "ezx")) mark++;
	} else if (ret == 1) {
	    if (cfg->writeEntry(uid, "GroupID", "root")) mark++;
		if (cfg->writeEntry(uid, "UserID", "root")) mark++;		
	}
	
	if (mark == 2)
		showNotify("", lng->tr("POP_OPERATION_OK"), 1);
	cfg->flush();
	delete cfg;	
	#endif
}
void megabox::mpkg_run_type()
{
	#ifdef VE66_OLD_MP
	ZTaskItem* pItem= (ZTaskItem*)mpkgListbox->item( mpkgListbox->currentItem() );
	if (pItem == NULL) return;

	int mark = 0;
	
	ZConfig *cfg = NULL;
	if( pItem->isSD() ) cfg = new ZConfig(SD_INSTALL_DB);
	else cfg = new ZConfig(PHONE_INSTALL_DB);

	QString uid = pItem->getGname();
	
	ZSingleSelectDlg *dlg = new ZSingleSelectDlg(lng->tr("POP_SET_TYPE"), lng->tr("FT_PLEASCHOOSE"));
    QStringList strlist;
    strlist += lng->tr("FT_SIMPLE");
    strlist += lng->tr("FT_ULTIMATE");
    dlg->addItemsList(strlist);
    dlg->exec();
    int ret = dlg->getCheckedItemIndex();
    delete dlg;			
	if (ret == 0) {
	    if (cfg->writeEntry(uid, "AppType", "0")) mark++;
    } else if (ret == 1) {
	    if (cfg->writeEntry(uid, "AppType", "3")) mark++;
    }
	if (mark == 1)
		showNotify("", lng->tr("POP_OPERATION_OK"), 1);
	else
		showNotify("", lng->tr("POP_OPERATION_ERROR"), 1);

	cfg->flush();	
	//delete note;
	delete cfg;	
	#endif
}
//#endif
/**********************************************************************************/
/**********************************************************************************/
void megabox::mgx_refresh()
{
    QString origin_name = getAppTitle();
	setAppTitle(lng->tr("FT_REFRESHING"));

	mgxDB->ChkMgxList();
	mgxListbox->clear();	 
	mgxListbox->bulidAppList();	
	
	setAppTitle(origin_name);	
}
void megabox::mgx_force_refresh()
{
	int ret = showQ(lng->tr("FT_FORCE_REFRESH"), lng->tr("FT_FORCE_REFRESH_INFO"), 1);
	if( ret != 1 ) return;
	
	if( QFile::remove(MGX_OBJ_PATH) )
	    qDebug("remove mgx db ok\n");
	else
        qDebug("remove mgx db faile\n")	;
		
    mgx_refresh();
}

void megabox::mgx_move()
{
	ZTaskItem* pItem= (ZTaskItem*)mgxListbox->item( mgxListbox->currentItem() );
	if (pItem == NULL) return;
	if( pItem->getType() == ITEM_MENU_OBJ ) return;
	QString gname = pItem->getGname();
	
	QString name = mgxDB->getObjectName( gname);
	
	saveDlg *sDlg = new saveDlg(lng->tr("FT_MOVE"), name, 1);
	sDlg->exec();
	int ret = sDlg->checkedIndex;
	if (ret == -1)
	{
		delete sDlg;
		sDlg = NULL;
		return;
	}
	QString newName = sDlg->newName;
	delete sDlg;
	sDlg = NULL;
	
    if ( (!pItem->isSD()  && ret == 0) || (pItem->isSD() && ret == 1) )
		return;
	
	QString path = mgxDB->getObjectDir(gname);
	QString objDir = path;
	objDir = objDir.remove(0, objDir.findRev("/")+1);
	
	QString nowPath;
	
	QString title = "";
	QString text = lng->tr("POP_PLEASE_WAIT"); 
	if(ret == 1) {
		nowPath = QString("%1/%2").arg(SD_MPKG_PATH).arg(objDir);
		fileModCore(title, text, path, QString(SD_MPKG_PATH), FILE_MOVE);		
		
	} else {
		nowPath = QString("%1/%2").arg(PHONE_MPKG_PATH).arg(objDir);
		fileModCore(title, text, path, QString(PHONE_MPKG_PATH), FILE_MOVE);		
	}	
	
	mgxDB->writeEntry(gname, "Directory", nowPath);
	mgxDB->writeEntry(gname, "Path", !pItem->isSD() );
	mgxDB->flush();
	QTimer::singleShot( 500, this, SLOT( mgx_refresh() ) );
}
void megabox::mgx_m_move()
{
	QStringList nameList = mgxListbox->getSelectedItemNameList();
	QStringList gNameList = mgxListbox->getSelectedItemGNameList();
		
	saveDlg *sDlg = new saveDlg(lng->tr("FT_MOVE"), nameList, 1);
	sDlg->exec();
	int ret = sDlg->checkedIndex;
	if (ret == -1)
	{
		delete sDlg;
		sDlg = NULL;
		return;
	}
	delete sDlg;
	sDlg = NULL;	

	QString nowPath;
	QStringList::ConstIterator p;   
	QString title = "";
	QString subtitle = lng->tr("POP_PLEASE_WAIT"); 
	
	for (p=gNameList.begin(); p!=gNameList.end(); p++)
	{	
		QString path = mgxDB->getObjectDir( *p);//cfg.readEntry(*p, "Directory", "");
		QString objDir = path;
		objDir = objDir.remove(0, objDir.findRev("/")+1);	
		bool bp = mgxDB->isSDObject(*p); //cfg.readBoolEntry(*p, "Path", false);

		if(ret == 0)
		{
			if( !bp )  continue;
			nowPath = QString("%1/%2").arg(PHONE_MPKG_PATH).arg(objDir);
			fileModCore(title, subtitle, path, QString(PHONE_MPKG_PATH), FILE_MOVE);
		}
		else
		{			
			if( bp == true ) 	continue;
			nowPath = QString("%1/%2").arg(SD_MPKG_PATH).arg(objDir);
			fileModCore(title, subtitle, path, QString(SD_MPKG_PATH), FILE_MOVE);
		} 	
		mgxDB->writeEntry(*p, "Directory", nowPath);
		mgxDB->writeEntry(*p, "Path", !bp );
	}	
	mgxDB->flush();
	
	QTimer::singleShot( 500, this, SLOT( mgx_sel_back() ) );	
}
void megabox::mgx_del()
{
	ZTaskItem* pItem= (ZTaskItem*)mgxListbox->item( mgxListbox->currentItem() );
	if (pItem == NULL) return;
	if( pItem->getType() == ITEM_MENU_OBJ ) return;
	
	QString gname = pItem->getGname();
	//ZConfig cfg(MGX_OBJ_PATH);
    QString path = mgxDB->getObjectDir( gname);//cfg.readEntry(gname, "Directory", "");	
	QString name = mgxDB->getObjectName( gname); //cfg.readEntry(gname, "Name", "");		
	
	QString text;
	text += lng->tr("FT_CONFIRM_DELETE")+"\n";
	text += lng->tr("PROP_NAME")+name+"\n";
	text += lng->tr("PROP_PATH")+path+"\n";	

	int ret = showQ(lng->tr("FT_DELETE"), text, 1);
	if (ret != 1) return;

	QString title = "";
	QString subtitle = lng->tr("POP_PLEASE_WAIT"); 
	
	fileModCore(title, subtitle, path, "", FILE_DELETE);
	
	mgxDB->clearGroup(gname);
	mgxDB->flush();
	
	QTimer::singleShot( 500, this, SLOT( mgx_refresh() ) );
}
void megabox::mgx_m_del()
{
	QString nameListStr = mgxListbox->getSelectedItemNameStr( "||");
	QStringList gNameList = mgxListbox->getSelectedItemGNameList();

	int ret = showQ(lng->tr("FT_DELETE"), tr("FT_CONFIRM_DELETE")+nameListStr+"?", 1);		
	if (ret == 1)
	{		
		QString title = "";
		QString text;
		QString path;
		QString name;
		QStringList::ConstIterator q;   		
		for(q=gNameList.begin(); q!=gNameList.end(); q++)
		{
			path = mgxDB->getObjectDir(*q);//cfg.readEntry(*q, "Directory", "");	
			name = mgxDB->getObjectName(*q);//cfg.readEntry(*q, "Name", "");	
			text = QString("%1 %2").arg(lng->tr("FT_DELETE")).arg(name);
			
			fileModCore(title, text, path, "", FILE_DELETE);
		}	
		QTimer::singleShot( 500, this, SLOT( mgx_sel_back() ) );
	}
	else
		return;
}

void megabox::mgx_save()
{
	ZTaskItem* pItem= (ZTaskItem*)mgxListbox->item( mgxListbox->currentItem() );
	if (pItem == NULL) return;
	if( pItem->getType() == ITEM_MENU_OBJ )  return;
	
	QString name = mgxDB->getObjectName(pItem->getGname());//cfg.readEntry(pItem->getGname(), "Name", "");	

	saveDlg *sDlg = new saveDlg(lng->tr("FT_BACKUP"), name);
	sDlg->exec();
	int ret = sDlg->checkedIndex;
	if (ret == -1)
	{
		delete sDlg;
		sDlg = NULL;
		return;
	}
	QString newName = sDlg->newName;
	delete sDlg;
	sDlg = NULL;	
	
    QString path = mgxDB->getObjectDir(pItem->getGname());//cfg.readEntry(pItem->getGname(), "Directory", "");		

	char exec_buf[768];
	if(ret == 0) 
	{
		system( QString("rm -f %1/%2.mgx").arg(PH_MGX_FILE).arg(newName) );
		sprintf(exec_buf, "'%s/7z' a -y '%s/%s.mgx' '%s'", 
				P7Z_PATH, PH_MGX_FILE, (const char*)newName.local8Bit(), (const char*)path.local8Bit() );
	}			
	else if (ret == 1)		
	{
		system( QString("rm -f %1/%2.mgx").arg(SD_MGX_FILE).arg(newName) );
		sprintf(exec_buf, "'%s/7z' a -y '%s/%s.mgx' '%s'",
				P7Z_PATH, SD_MGX_FILE, (const char*)newName.local8Bit(), (const char*)path.local8Bit() );
	}			
	else
		return;

	QString title = "";
	QString subtitle = lng->tr("POP_PLEASE_WAIT"); 
	deathDlg *execDlg = new deathDlg(title, subtitle, exec_buf);
	execDlg->exec();
	delete execDlg;
	execDlg = NULL;	
}
void megabox::mgx_m_save()
{
	QStringList nameList = mgxListbox->getSelectedItemNameList();
	QStringList gNameList = mgxListbox->getSelectedItemGNameList();	

	saveDlg *sDlg = new saveDlg(lng->tr("FT_SAVE"), nameList);
	sDlg->exec();
	int ret = sDlg->checkedIndex;
	if (ret == -1)
	{
		delete sDlg;
		sDlg = NULL;
		return;
	}
	QString newName = sDlg->newName;
	delete sDlg;
	sDlg = NULL;	
	
	char exec_buf[768];
	if(ret == 0) {
		system( QString("rm -f %1/%2.mgx").arg(PH_MGX_FILE).arg(newName) );
	} else if (ret == 1) {
		system( QString("rm -f %1/%2.mgx").arg(SD_MGX_FILE).arg(newName) );
	} else
		return;	
		
	QStringList::ConstIterator q;   
	QString title = "";
	QString subtitle = lng->tr("POP_PLEASE_WAIT"); 
	
	for (q=gNameList.begin(); q!=gNameList.end(); q++)
	{	
		QString path = mgxDB->getObjectDir( *q );//cfg.readEntry(*q, "Directory", "");	
		if(ret == 0) 
			sprintf(exec_buf, "'%s/7z' a -y '%s/%s.mgx' '%s'", 
					P7Z_PATH, PH_MGX_FILE, (const char*)newName.local8Bit(), (const char*)path.local8Bit() );
		else
			sprintf(exec_buf, "'%s/7z' a -y '%s/%s.mgx' '%s'",
					P7Z_PATH, SD_MGX_FILE, (const char*)newName.local8Bit(), (const char*)path.local8Bit() );
	
		deathDlg *execDlg = new deathDlg(title, subtitle, exec_buf);
		execDlg->exec();
		delete execDlg;
		execDlg = NULL;					
	}
	QTimer::singleShot( 500, this, SLOT( mgx_sel_back() ) );	
}
void megabox::mgx_setMult()
{
	mgxListbox->setMode(MULTI);
	mgx_refresh();
	switchTab(TAB_MGX);//, MULTI);
}
void megabox::mgx_sel_all()
{
	mgxListbox->selAll();
}
void megabox::mgx_sel_none()
{
	mgxListbox->selNone();
}
void megabox::mgx_sel_back()
{
	mgxListbox->setMode(NORMAL);
	mgx_refresh();	
	switchTab(TAB_MGX);//, NORMAL);
}
void megabox::mgx_create_menu()
{
	if( !codeComp->cpcp() ) {
	    regNotify();
		return;
	}	
	
	ZSingleCaptureDlg *dlg = new ZSingleCaptureDlg(lng->tr("FT_CREATE"), lng->tr("FT_INPUT_NAME"),
								(ZSingleCaptureDlg::CaptureDlgType)0, NULL, "ZSingleCaptureDlg", true, 0, 0);	
	dlg->exec();
	QString name = (dlg->getResultText().stripWhiteSpace());//.local8Bit();	
    delete dlg;
	
	if( isValidFileName(name) ) {
		showQ(lng->tr("FT_ERR0R"), lng->tr("FT_INVALID"), 2);
		return;
	}
	mgxDB->addMenu(name);
	if( mgxDB->flush() ) {
		QTimer::singleShot( 300, this, SLOT( mgx_refresh() ) );
	}	
}
void megabox::mgx_delete_menu()
{
	if( !codeComp->cpcp() ) {
	    regNotify();
		return;
	}
	
	QString gName = QString::null;
	QString name = QString::null;
	QString title = lng->tr("FT_DELETE_MENU");
	appAddDlg *dlg = new appAddDlg(title, appAddDlg::DT_showMgxMenuList, false);
	dlg->exec();
	bool isCancel = dlg->isCancel;
	if( !isCancel ) {
		gName = dlg->appGname;
		name = dlg->appName;	
	}	
	delete dlg;
	dlg = NULL;
	if(isCancel) return;

	int ret = showQ(lng->tr("FT_DELETE"), lng->tr("FT_CONFIRM_DELETE")+"\n"+name, 1);
	if (ret != 1) return;
	
	mgxDB->clearGroup(gName);

	QTimer::singleShot( 300, this, SLOT( mgx_refresh() ) );	
}
void megabox::mgx_mod_menu()
{
	if( !codeComp->cpcp() ) {
	    regNotify();
		return;
	}	
	
	QString gName = QString::null;
	QString name = QString::null;
	QString title = lng->tr("FT_RENAME_MENU");
	appAddDlg *dlg = new appAddDlg(title, appAddDlg::DT_showMgxMenuList, false);
	dlg->exec();
	bool isCancel = dlg->isCancel;
	if( !isCancel ) {
		gName = dlg->appGname;
		name = dlg->appName;	
	}	
	delete dlg;
	dlg = NULL;
	if(isCancel) return;

	if (name == QString::null) return;
	
	ZSingleCaptureDlg *sdlg = new ZSingleCaptureDlg(
	    lng->tr("FT_RENAME"), lng->tr("FT_CUR_NAME")+name,
	    (ZSingleCaptureDlg::CaptureDlgType)0, NULL, "ZSingleCaptureDlg", true, 0, 0);
	ZLineEdit *line = sdlg->getLineEdit();
	line->setText(name);
	int ret=sdlg->exec();

	QString newName = QString::null;
	if (ret == 1) 	{
		newName = sdlg->getResultText().stripWhiteSpace();//).local8Bit();
		delete sdlg;
	} else 	{
		delete sdlg;
		return;
	}
	if( isValidFileName(newName) )  {
		showQ(lng->tr("FT_ERR0R"), lng->tr("FT_INVALID"), 2);
		return;
	}	
	mgxDB->writeEntry(gName, "Name", newName);
	mgxDB->flush();

	QTimer::singleShot( 300, this, SLOT( mgx_refresh() ) );		
}
void megabox::mgx_add_2_menu()
{
	if( !codeComp->cpcp() ) {
	    regNotify();
		return;
	}	
	
	ZTaskItem* pItem= (ZTaskItem*)mgxListbox->item( mgxListbox->currentItem() );
	if (pItem == NULL) return;
	if( pItem->getType() == ITEM_MENU_OBJ ) return;
	
	QString menuGName = QString::null;
	QString menuName = QString::null;
	
	QString title = lng->tr("FT_ADD");
	appAddDlg *dlg = new appAddDlg(title, appAddDlg::DT_showMgxMenuList, false);
	dlg->exec();
	bool isCancel = dlg->isCancel;
	if( !isCancel ) {
		menuGName = dlg->appGname;
		menuName = dlg->appName;	
	}
	delete dlg;
	dlg = NULL;	
	if(isCancel) return;
	
	QString name = mgxDB->getObjectName(pItem->getGname());//cfg.readEntry(pItem->getGname(), "Name", "");

	QString text = lng->tr("FT_CONFIRM_MOVE")+"\n";
	text += name+"\n";
	text += ctr("↓↓↓↓↓↓↓↓↓")+"\n";	
	text += menuName+"\n";
	
	int ret = showQ(lng->tr("FT_ADD"),	text, 1);
	if(ret != 1 ) return;
	mgxDB->add2Menu( pItem->getGname(), menuGName);

	QTimer::singleShot( 300, this, SLOT( mgx_refresh() ) );		
}

void megabox::mgx_m_add_2_menu()
{
	if( !codeComp->cpcp() ) {
	    regNotify();
		return;
	}	
	
	QString nameListStr = mgxListbox->getSelectedItemNameStr();
	QStringList gNameList = mgxListbox->getSelectedItemGNameList();

	QString menuGName = QString::null;
	QString menuName = QString::null;
	
	QString title = lng->tr("FT_ADD");
	appAddDlg *dlg = new appAddDlg(title, appAddDlg::DT_showMgxMenuList, false);
	dlg->exec();
	bool isCancel = dlg->isCancel;
	if( !isCancel ) {
		menuGName = dlg->appGname;
		menuName = dlg->appName;	
	}
	delete dlg;
	dlg = NULL;	
	if(isCancel) return;	

	nameListStr = QString("%1 \n%2\n%3\n%4\n").arg(lng->tr("FT_CONFIRM_MOVE")).arg(nameListStr).arg(ctr("↓↓↓↓↓↓↓↓↓")).arg(menuName);
	int ret = showQ(lng->tr("FT_ADD"),	nameListStr, 1);
	if(ret != 1 ) return;
	QStringList::ConstIterator q;  	
	for(q=gNameList.begin(); q!=gNameList.end(); q++) 
	{	
		mgxDB->add2Menu( *q, menuGName);
	}	
	QTimer::singleShot( 300, this, SLOT( mgx_sel_back() ) );	
}
void megabox::mgx_move_2_main()
{
	if( !codeComp->cpcp() ) {
	    regNotify();
		return;
	}	
	
	ZTaskItem* pItem= (ZTaskItem*)mgxListbox->item( mgxListbox->currentItem() );
	if (pItem == NULL) return;
	if( pItem->getType() == ITEM_MENU_OBJ ) return;
	
	QString name = mgxDB->getObjectName(pItem->getGname());

	int ret = showQ(lng->tr("FT_MOVE"), lng->tr("FT_CONFIRM_MOVE_TO_MAIN_MENU"), 1);
	if(ret != 1)  return;
	
	mgxDB->move2Root( pItem->getGname(), mgxListbox->getCurMenuId() );
	
	QTimer::singleShot( 300, this, SLOT( mgx_refresh() ) );	
}
void megabox::mgx_m_move_2_main()
{
	if( !codeComp->cpcp() ) {
	    regNotify();
		return;
	}	
	
	QString nameListStr = mgxListbox->getSelectedItemNameStr();
	QStringList gNameList = mgxListbox->getSelectedItemGNameList();

	int ret = showQ(lng->tr("FT_MOVE"),lng->tr("FT_CONFIRM_MOVE_TO_MAIN_MENU")+"\n"+nameListStr+"?", 1);
	if(ret != 1 ) return;
	mgxDB->move2Root( gNameList, mgxListbox->getCurMenuId() );
	
	QTimer::singleShot( 300, this, SLOT( mgx_sel_back() ) );	
}
void megabox::mgx_rename()
{
	ZTaskItem* pItem= (ZTaskItem*)mgxListbox->item( mgxListbox->currentItem() );
	if (pItem == NULL) return;
	if( pItem->getType() == ITEM_MENU_OBJ )  return;
	QString gname = pItem->getGname();
	QString oldName = mgxDB->getObjectName( gname);//cfg.readEntry(pItem->getGname(), "Name", "");	
    QString path = mgxDB->getObjectDir( gname);//cfg.readEntry(pItem->getGname(), "Directory", "");
	QString newName = QString::null;
	
	ZSingleCaptureDlg* dlg = new ZSingleCaptureDlg(
	    lng->tr("FT_RENAME"), "",
	    (ZSingleCaptureDlg::CaptureDlgType)0, NULL, "ZSingleCaptureDlg", true, 0, 0);
	ZLineEdit *line = dlg->getLineEdit();
	line->setText(oldName);
	int ret = dlg->exec();	
	if (ret == 1) 	{
		newName = dlg->getResultText().stripWhiteSpace();
		delete dlg;
		dlg = NULL;
	} else {
		delete dlg;
		dlg = NULL;
		return;
	}
	
	if( isValidFileName(newName) ) {
		showQ(lng->tr("FT_ERR0R"), lng->tr("FT_INVALID"), 2);
		return;
	}		
	mgxDB->writeEntry( gname, "Name", newName );
	mgxDB->flush();
	
	QString destName = path;
	destName = destName.remove(0, destName.findRev("/")+1);
	QString file = QString("%1/%2.cfg").arg(path).arg(destName);
	QString key = QString("Name");
	QString type = QString(" = ");
	if( setLineConfig(file, key, type, newName) )
		qDebug("== mgx rename ok ==");
	else
		qDebug("== mgx rename fail ==");
	
	QTimer::singleShot( 300, this, SLOT( mgx_refresh() ) );
}
/**********************************************************************************/
/**********************************************************************************/

void megabox::initReadFm()
{
    QString origin_name = getAppTitle();
	setAppTitle(lng->tr("FT_REFRESHING"));

	if(cfg->cfg_showHideObj) {
		fmListbox->setFilter(SHOW_ALL);	
		fmListbox2->setFilter(SHOW_ALL);	
	} else { 
		fmListbox->setFilter(NO_SHOW_HIDE); 
		fmListbox2->setFilter(NO_SHOW_HIDE); 
	}	
	
	fmListbox->enableMimeType(cfg->cfg_showMimeType);
	fmListbox2->enableMimeType(cfg->cfg_showMimeType);	
	
	fmListbox->init();
	fmListbox2->init();			
	
 	fixFmWidget();	
	
	setAppTitle(origin_name);
}

void megabox::fm_turn2root()
{
    m_strCurDir = "/";
	getFileBrowser()->setDir(m_strCurDir);
}

void megabox::fm_turn2phone()
{
    m_strCurDir = "/ezxlocal/download/mystuff";
	getFileBrowser()->setDir(m_strCurDir);
}

void megabox::fm_turn2mmc()
{
    m_strCurDir = "/mmc/mmca1";
	getFileBrowser()->setDir(m_strCurDir);
}

void megabox::fm_del()
{
	if (getFileBrowser()->getMode() == NORMAL)
    {	
	    if( getFileBrowser()->isItemInfo ) return;
		
		QString fpath = getFileBrowser()->getFilePath();

		int ret = showQ(lng->tr("FT_DELETE"), lng->tr("FT_CONFIRM_DELETE")+fpath+"?", 1);
	    if (ret != 1)  {
			return;
		}	
		QString title="";
		QString text=lng->tr("POP_PLEASE_WAIT");
		
		fileModCore(title, text, fpath, "", FILE_DELETE);
	}
    else if (getFileBrowser()->getMode() == MULTI)
	{
		QString text = "||";
		QStringList::ConstIterator p; 
		QStringList fmMultList = getFileBrowser()->getCheckedItemList();
		for (p=fmMultList.begin(); p!=fmMultList.end(); p++) {
			text += *p;
			text += "|| ";
		}
		
		int ret = showQ(lng->tr("FT_DELETE"), lng->tr("FT_CONFIRM_DELETE")+text+"?", 1);
		if (ret == 1)
		{		
			QString path = getFileBrowser()->getCurDir(); 
			QString full_path;			
			for (p=fmMultList.begin(); p!=fmMultList.end(); p++)
			{
				if(path == "/") full_path = path+*p;
				else full_path = path+"/"+*p;
				
				QString title = lng->tr("FT_DELETE");
				QString text = lng->tr("FT_DELETE")+full_path;

				fileModCore(title, text, full_path, "", FILE_DELETE);
			}
		}
	}
    menu_adv->setItemEnabled(0, false);	
	getFileBrowser()->setMode(NORMAL);
	getFileBrowser()->init();
	switchTab(TAB_FM);	
}

void megabox::fm_mult()
{
	if( !codeComp->cpcp() ) {
	    regNotify();
		return;
	}
	
	getFileBrowser()->setMode(MULTI);
	getFileBrowser()->init();
	switchTab(TAB_FM);//, MULTI);
} 

void megabox::fm_sel_all()
{
    getFileBrowser()->selAll();
} 
void megabox::fm_sel_none()
{
    getFileBrowser()->selNone();
} 
void megabox::fm_sel_back()
{
	getFileBrowser()->setMode(NORMAL);
	getFileBrowser()->init();
    switchTab(TAB_FM);	
} 

void megabox::fm_cut()
{
	if (getFileBrowser()->getMode() == NORMAL)
    {	
	    if( getFileBrowser()->isItemInfo ) return;
		
		fm_function_str = getFileBrowser()->getFilePath();
		m_eFmFunctionMod = NORMAL;
	}
    else if (getFileBrowser()->getMode() == MULTI)
	{
		m_strCurDir = getFileBrowser()->getCurDir();
		
		QString full_path;
		QStringList fmMultList = getFileBrowser()->getCheckedItemList();
		QStringList::ConstIterator p; 
		
		if(cfg->cfg_clearBufEveryMultModeInCut) fmTmpCutList = NULL;//.clear();
		
		for (p=fmMultList.begin(); p!=fmMultList.end(); p++)
		{
			if(m_strCurDir == "/") full_path = m_strCurDir+*p;
			else full_path = m_strCurDir+"/"+*p;
			fmTmpCutList.append(full_path);		
		}
		m_eFmFunctionMod = MULTI;
    }
    m_eMod = FM_MOD_CUT;
	getFileBrowser()->setMode(NORMAL);	
    getFileBrowser()->init();	
	switchTab(TAB_FM);
	menu_adv->setItemEnabled(0, true);	
}

void megabox::fm_copy()
{
	if (getFileBrowser()->getMode() == NORMAL)
    {	
		if( getFileBrowser()->isItemInfo  ) return;
		fm_function_str = getFileBrowser()->getFilePath();
		m_eFmFunctionMod = NORMAL;
	}
    else if (getFileBrowser()->getMode() == MULTI)
	{
		m_strCurDir = getFileBrowser()->getCurDir();
		
		QString full_path;
		QStringList fmMultList = getFileBrowser()->getCheckedItemList();
		QStringList::ConstIterator p; 
		
		if(cfg->cfg_clearBufEveryMultModeInCut) fmTmpCopyList = NULL;//.clear();
		
		for (p=fmMultList.begin(); p!=fmMultList.end(); p++)
		{
			if(m_strCurDir == "/") full_path = m_strCurDir+*p;
			else full_path = m_strCurDir+"/"+*p;
			fmTmpCopyList.append( full_path );
		}		
		
		m_eFmFunctionMod = MULTI;
    }
    m_eMod = FM_MOD_COPY;
	getFileBrowser()->setMode(NORMAL);	
    getFileBrowser()->init();	
	switchTab(TAB_FM);
	menu_adv->setItemEnabled(0, true);
}

void megabox::fm_clear()
{
    m_eMod = FM_MOD_NORMAL;
	m_eFmFunctionMod = NORMAL;
	fm_function_str = QString::null;
	fmTmpCutList = NULL;
	fmTmpCopyList = NULL;
    menu_adv->setItemEnabled(0, false);
}

void megabox::fm_paste()
{
    m_strCurDir = getFileBrowser()->getCurDir(); 
	#ifdef DEBUGFM
	std::cout<<"DEBUG fm_function_str is "<<fm_function_str<<std::endl;
	std::cout<<"DEBUG cur path is "<<m_strCurDir<<std::endl;
	#endif		
    if(m_eFmFunctionMod == NORMAL)
	{
	    if(m_eMod == FM_MOD_COPY) {
			int ret = showQ(lng->tr("FT_COPY"), lng->tr("FT_CONFIRM_COPY")+fm_function_str+"->"+m_strCurDir+"?", 1);
			if(ret == 1) {
				QString title = lng->tr("POP_PLEASE_WAIT");
				QString text = lng->tr("FT_COPY")+" : "+fm_function_str;
				fileModCore(title, text, fm_function_str, m_strCurDir, FILE_PASTE);
			}
		}
	    if(m_eMod == FM_MOD_CUT) 	{
			int ret = showQ(lng->tr("FT_CUT"), lng->tr("FT_CONFIRM_MOVE")+fm_function_str+"->"+m_strCurDir+"?", 1);
			if (ret == 1) {
				QString title = lng->tr("POP_PLEASE_WAIT");
				QString text = lng->tr("FT_MOVE")+" : "+fm_function_str;
				fileModCore(title, text, fm_function_str, m_strCurDir, FILE_MOVE);
		    }	
		}	
	}
	if(m_eFmFunctionMod == MULTI)
	{
		QString full_path;
		QStringList::Iterator p;
		QString text = "|| ";

		if(m_eMod == FM_MOD_COPY) {
	        for(p=fmTmpCopyList.begin(); p!=fmTmpCopyList.end(); ++p ) {
				full_path = *p;
				if(full_path == "") continue;
				while ( full_path.find("/") > -1 )
					full_path.remove(0,full_path.find("/")+1 );
				text += full_path;
				text += " || ";
			}
			
			int ret = showQ(lng->tr("FT_COPY"), lng->tr("FT_CONFIRM_COPY")+text+"->"+m_strCurDir+"?", 1);
			if(ret == 1) {
				for(p=fmTmpCopyList.begin(); p!=fmTmpCopyList.end(); ++p ) 	{
					full_path = *p;
					if(full_path == "") continue;
				
					QString title = lng->tr("FT_COPY");
					QString subtitle = lng->tr("FT_COPY")+" : "+full_path;
					fileModCore(title, subtitle, full_path, m_strCurDir, FILE_PASTE);
				}
			}
		}
		if(m_eMod == FM_MOD_CUT) 	{
	        for(p=fmTmpCutList.begin(); p!=fmTmpCutList.end(); ++p ) {
				full_path = *p;
				if(full_path == "") continue;
				while ( full_path.find("/") > -1 )
					full_path.remove(0,full_path.find("/")+1 );				
				text += full_path.remove(0, full_path.find("/")+1);
				text += " || ";
			}

			int ret = showQ(lng->tr("FT_CUT"), lng->tr("FT_CONFIRM_MOVE")+text+"->"+m_strCurDir+"?", 1);
            if(ret == 1) {
				for(p=fmTmpCutList.begin(); p!=fmTmpCutList.end(); ++p ) {
					full_path = *p;
					if(full_path == "") continue;
				
					QString title = lng->tr("FT_CUT");
					QString subtitle = lng->tr("FT_CUT")+" : "+full_path;
					
					fileModCore(title, subtitle, full_path, m_strCurDir, FILE_MOVE);
				}
			}
	    }	
	}
	fm_clear();
	getFileBrowser()->setMode(NORMAL);	
	getFileBrowser()->init();
	//switchTab(TAB_FM);
}

void megabox::fm_create()
{
	m_strCurDir = getFileBrowser()->getCurDir(); 
	int ret = -1;
	ZSingleSelectDlg *ssdlg = new ZSingleSelectDlg(lng->tr("FT_PLEASCHOOSE") , "");
	QStringList strlist;
	strlist += lng->tr("FT_CREATE_FILE");
	strlist += lng->tr("FT_CREATE_DIR");
	ssdlg->addItemsList(strlist);
	if( ssdlg->exec() == ZPopup::Accepted ) {
		ret = ssdlg->getCheckedItemIndex();
		delete ssdlg;
		ssdlg = NULL;
	} else {
		delete ssdlg;
		ssdlg = NULL;
		return;	
	}
	
	QString filename = "";
	ZSingleCaptureDlg *dlg = new ZSingleCaptureDlg(
	    lng->tr("FT_CREATE"), lng->tr("FT_INPUT_NAME"),
		(ZSingleCaptureDlg::CaptureDlgType)0, NULL, "ZSingleCaptureDlg", true, 0, 0);	
	if( dlg->exec() == ZPopup::Accepted ) {
		filename = (dlg->getResultText().stripWhiteSpace());//.local8Bit();	
		delete dlg;
		dlg = NULL;
	}else{
		delete dlg;
		dlg = NULL;
		return;
	}
	
	if( isValidFileName(filename))
	{
		showQ(lng->tr("FT_ERR0R"), lng->tr("FT_INVALID"), 2);
		return;
	}
	QString path;
	if(m_strCurDir == "/") path = m_strCurDir+filename;
	else path = m_strCurDir+"/"+filename;
	QDir dFile(path);
	QFile fFile(path);
	if(dFile.exists() || fFile.exists() )
	{
		showNotify("", lng->tr("POP_FILE_EXISTS"), 1);
		return;
	}
	if (ret == 0)  {
		//char buf[216];
		//sprintf(buf, "RokrToolsPro\n");
		QString text = QString("RokrToolsPro\n");
		if( fFile.open ( IO_WriteOnly )) {
			fFile.writeBlock( (const char*)text.local8Bit(), text.length() );
			fFile.close();
		}
	}else if (ret == 1) {
		dFile.setPath(m_strCurDir);
		dFile.mkdir(filename);
	}
	dFile.setPath(path);
	if( !dFile.exists() && !fFile.exists() ) {
		showQ(lng->tr("FT_ERR0R"), lng->tr("FT_CANNOT_CREAT"), 2);
		return;
	}
    getFileBrowser()->init();
}

void megabox::fm_rename()
{ 
	int index=-1;
    index = getFileBrowser()->currentItem();
    if (index == -1) return;
    
	if( getFileBrowser()->isItemInfo ) return;
	
	m_strCurDir = getFileBrowser()->getCurDir(); 

	QString newName = QString::null;
	QString newFpath = QString::null;
    QString oldName = getFileBrowser()->getFileName();
    QString fpath = getFileBrowser()->getFilePath();	
	QString spath = m_strCurDir;
	
	ZSingleCaptureDlg* dlg = new ZSingleCaptureDlg(
	    lng->tr("FT_RENAME"), lng->tr("PROP_PATH")+spath+"\n"+lng->tr("PROP_NAME")+oldName,
	    (ZSingleCaptureDlg::CaptureDlgType)0, NULL, "ZSingleCaptureDlg", true, 0, 0);
	ZLineEdit *line = dlg->getLineEdit();
	line->setText(oldName);

	if ( dlg->exec() == ZPopup::Accepted) 	{
		newName = dlg->getResultText().stripWhiteSpace();
		delete dlg;
	} else {
		delete dlg;
		return;
	}
	if( isValidFileName(newName) ) {
		showQ(lng->tr("FT_ERR0R"), lng->tr("FT_INVALID"), 2);
		return;
	}	
	QString title = lng->tr("FT_RENAME");
	QString text = lng->tr("POP_PLEASE_WAIT");
	
	fileModCore(title, text, fpath, QString("%1/%2").arg(spath).arg(newName), FILE_MOVE);

    getFileBrowser()->init();
}

void megabox::fm_search()
{
	m_strCurDir = getFileBrowser()->getCurDir(); 

	ZSingleCaptureDlg* dlg = new ZSingleCaptureDlg(
	    lng->tr("FT_SEARCH"), lng->tr("FT_MANY_FILES_SLOW"),
	    (ZSingleCaptureDlg::CaptureDlgType)0, NULL, "ZSingleCaptureDlg", true, 0, 0);
	
	ZLineEdit *line1 = dlg->getLineEdit();
	line1->setTitlePosition(ZLineEdit::TitleTop);
	line1->setTitle( lng->tr("FT_SEARCH") );
	line1->resize(300, 40);

	ZCheckBox *chk = new ZCheckBox(lng->tr("FT_SEARCH_TEXT_FILE"), dlg); 
	chk->resize(300, 40);
	dlg->insertChild(chk);	
	
	ZLineEdit *line2 = new ZLineEdit(dlg);
	line2->setTitlePosition(ZLineEdit::TitleTop);
	line2->setTitle( lng->tr("PROP_PATH") );
	line2->setText( m_strCurDir );
	line2->resize(300, 60);
	dlg->insertChild(line2);
	
	QString newPath, newKey;
	bool isSearchText = false; 
	if( dlg->exec() == ZPopup::Accepted	) {
		newPath = line2->text().stripWhiteSpace();
		newKey = line1->text().stripWhiteSpace();
		isSearchText = chk->isChecked();
		delete dlg;
		dlg = NULL;	
	} else {
		delete dlg;
		dlg = NULL;
		return;
	}
	if( newKey == "" || newKey == QString::null || newPath == "" || newPath == QString::null) return;
	
	searchDlg *sdlg = new searchDlg( newPath, newKey, isSearchText );
	sdlg->exec();
	QStringList itemList = sdlg->getResultList();
	delete sdlg;
	sdlg = NULL;
	
	bool dirState = true;
	QString dirName, fileName;
	searchUI *ui = new searchUI( itemList );
	if( ui->exec() == ZPopup::Accepted ) {
		dirState = ui->isClickedDir();
		if( dirState ) {
			dirName = ui->getClickedObjDirname();
		} else {
			dirName = ui->getClickedObjDirname();
			fileName = ui->getClickedObjName();		
		}
		delete ui;
		ui = NULL;	
	} else {
		delete ui;
		ui = NULL;
		return;
	}
	//cout<<" ======= clicked "<<dirName<<endl;
	m_strCurDir = dirName;
	getFileBrowser()->setDir(m_strCurDir);
	if( !dirState ) {
		getFileBrowser()->setCursor( fileName );	
	}
	switchTab(TAB_FM);
}

void megabox::fm_pack()
{
	if( !codeComp->cpcp() ) {
	    regNotify();
		return;
	}	
	
	int index=-1;
    index = getFileBrowser()->currentItem();
    if (index == -1) return;
	if( getFileBrowser()->isItemInfo ) return;
	
	QString destPath, newName, fromPath;
	int pathType = -1, packageType = -1;
	compressDlg *cpDlg = new compressDlg( lng->tr("POP_PACK"), getFileBrowser()->getFileName() );
	if( cpDlg->exec() == ZPopup::Accepted) 	{
		pathType = cpDlg->pathIndex;
		packageType = cpDlg->typeIndex;
		newName = cpDlg->newName;
		delete cpDlg;
		cpDlg = NULL;
	} else {
		delete cpDlg;
		cpDlg = NULL;
		return;
	}
	switch(pathType) {
		case 0:
			destPath = QString(PH_BASE);
			break;
		case 1:
			destPath =  getFileBrowser()->getCurDir();
			break;
		case 2:
			destPath =  QString(SD_BASE);
			break;
		default:
			destPath = QString(PH_BASE);
			break;
	}
	fromPath = getFileBrowser()->getFilePath();
	
	char exec_buf[1000];
	switch( packageType ) {
		case 0:
			sprintf(exec_buf, "'%s/7z' a -y '%s/%s.7z' '%s'", P7Z_PATH,
							(const char*)destPath.local8Bit(), (const char*)newName.local8Bit(), 
							(const char*)fromPath.local8Bit() );	
			break;
		case 1: //tar cvf FileName.tar DirName
			sprintf(exec_buf, "busybox_full tar cvf '%s/%s.tar' '%s'",
							(const char*)destPath.local8Bit(), (const char*)newName.local8Bit(), 
							(const char*)fromPath.local8Bit() );
			break;
		case 2: //tar zcvf FileName.tar.gz DirName
			sprintf(exec_buf, "busybox_full tar zcvf '%s/%s.tar.gz' '%s'",
							(const char*)destPath.local8Bit(), (const char*)newName.local8Bit(), 
							(const char*)fromPath.local8Bit() );
			break;
		case 3: //tar jcvf FileName.tar.bz2 DirName
			sprintf(exec_buf, "busybox_full tar jcvf '%s/%s.tar.bz2' '%s'",
							(const char*)destPath.local8Bit(), (const char*)newName.local8Bit(), 
							(const char*)fromPath.local8Bit() );
			break;		
		default:
			sprintf(exec_buf, "'%s/7z' a -y '%s/%s.7z' '%s'", P7Z_PATH,
							(const char*)destPath.local8Bit(), (const char*)newName.local8Bit(), 
							(const char*)fromPath.local8Bit() );	
			break;			
	}
				
	QString title = lng->tr("POP_PACK");
	QString text = lng->tr("POP_PLEASE_WAIT");
    deathDlg *execDlg=new deathDlg(title, text, exec_buf);
    execDlg->exec();
    delete execDlg;
	execDlg = NULL;

    getFileBrowser()->init();	
}
void megabox::fm_openWith()
{
	if( !codeComp->cpcp() ) {
	    regNotify();
		return;
	}	

	QString dir = QString::null;
	QString exec = QString::null;
	QString name = QString::null;
		
	QString title = lng->tr("FT_OPENWITH");
	appAddDlg *dlg = new appAddDlg(title, appAddDlg::DT_getAppInfo, false);
	dlg->exec();
	bool isCancel = dlg->isCancel;
	if( !isCancel )
	{
		dir = dlg->appDir;
		exec = dlg->appExec;
		name = dlg->appName;
	}
	delete dlg;
	dlg = NULL;
	if( isCancel )	return;
	
	if( !dir.startsWith("/") )
		dir = QString("/usr/SYSqtapp/%1").arg(dir);
	QString fpath = getFileBrowser()->getFilePath();
	QString cmd = QString("%1/%2 -d %3").arg(dir).arg(exec).arg(fpath);
	QString info = QString("%1%2\n%3%4").arg( lng->tr("PROP_NAME") ).arg(name).arg( lng->tr("FT_OPENWITH_PROP") ).arg(cmd);
	int ret = showQ(lng->tr("FT_OPENWITH"), lng->tr("FT_OPENWITH_INFO")+"\n"+info);
	if(ret != 1)
		return;
	system(cmd);	
}	
void megabox::fm_fileInfo()
{
	int index=-1;
    index = getFileBrowser()->currentItem();
    if (index == -1) return;	
	if( getFileBrowser()->isItemInfo ) return;
	
	QString text;
	QString f = getFileBrowser()->getFilePath();
	QFileInfo *qf = new QFileInfo(f);
    ///////////////////////////////
	text += lng->tr("PROP_NAME")+getFileBrowser()->getFileName()+"\n";
	/////////////////////////////////////
    text += lng->tr("PROP_TYPE");
	if (qf->isSymLink())
        text += lng->tr("PROP_SYMLINK");
    else if (qf->isDir())
        text += lng->tr("PROP_FOLDER");
    else if (qf->isFile())
        text += lng->tr("PROP_FILE");
    else text += lng->tr("PROP_UNKNOWN");
	text += "\n";
	//////////////////////////////////////
	unsigned long size = qf->size();
		
	text += lng->tr("PROP_SIZE");
	text += size2string(size);
    text += "\n";
	////////////////////////////////////
	int c_year, c_month, c_day;
	int m_year, m_month, m_day;
	unsigned int c_hour, c_min;
	unsigned int m_hour, m_min;
	QDateTime cdt = qf->firstCreate();	
	c_year = cdt.date().year();
	c_month = cdt.date().month();
	c_day = cdt.date().day();
	c_hour = cdt.time().hour();
	c_min = cdt.time().minute();
	
	QDateTime mdt = qf->lastModified();
	m_year = mdt.date().year();
	m_month = mdt.date().month();
	m_day = mdt.date().day();
	m_hour = mdt.time().hour();
	m_min = mdt.time().minute();
	
	text += lng->tr("PROP_CREATE_TIME")+QString("%1-%2-%3  %4:%5").arg(c_year).arg(c_month).arg(c_day).arg(c_hour).arg(c_min)+"\n";
	text += lng->tr("PROP_MOD_TIME")+QString("%1-%2-%3  %4:%5").arg(m_year).arg(m_month).arg(m_day).arg(m_hour).arg(m_min)+"\n";
	///////////////////////////////////////////////////////////
	QString rights = "";
    if (qf->isDir()) rights = "d";
    else rights = "-";
    if (qf->permission(QFileInfo::ReadUser)) rights.append("r");
    else rights.append("-");
    if (qf->permission(QFileInfo::WriteUser)) rights.append("w");
    else rights.append("-");
    if (qf->permission(QFileInfo::ExeUser)) rights.append("x");
    else rights.append("-");
    rights.append(" ");
    if (qf->permission(QFileInfo::ReadGroup)) rights.append("r");
    else rights.append("-");
    if (qf->permission(QFileInfo::WriteGroup)) rights.append("w");
    else rights.append("-");
    if (qf->permission(QFileInfo::ExeGroup)) rights.append("x");
    else rights.append("-");
    rights.append(" ");
    if (qf->permission(QFileInfo::ReadOther)) rights.append("r");
    else rights.append("-");
    if (qf->permission(QFileInfo::WriteOther)) rights.append("w");
    else rights.append("-");
    if (qf->permission(QFileInfo::ExeOther)) rights.append("x");
    else rights.append("-");
	text += lng->tr("PROP_PERM")+rights+"\n";
	/////////////////////////////////////////////////////
	text += lng->tr("PROP_PATH")+f+"\n";
	/////////////////////////////////////////////////////

	delete qf;
	showQ(lng->tr("FT_PROPS"), text, 2);
}
void megabox::fm_sendBy_bt()
{
	int index=-1;
    index = getFileBrowser()->currentItem();
    if (index == -1) return;
	if( getFileBrowser()->isItemInfo ) return;
	
	QString file = getFileBrowser()->getFilePath();	
	QFileInfo *qf = new QFileInfo(file);
	if( qf->isDir() ) {
		delete qf;
		return;
	}
	delete qf;	
	
	int ret = showQ(lng->tr("FT_SEND_BY_BT"), lng->tr("FT_CONFIRM_TO_SEND")+"\n"+file, 1);
	if( ret != 1)
		return;
	
	QString tmp = file;
	while ( tmp.find(".") > -1 )
		tmp.remove(0, tmp.find(".")+1 );
		
	SHARE_Tool::OBJECT_INFO info;
	info.n = file.right( file.findRev("/")!=-1 ? ( file.length()-file.findRev("/") ) : file.length() );
	info.t = "media/"+tmp;
	if ( !SHARE_Tool::shareByBt(info, file) )
		qDebug("Error send file!");	
	else
		qDebug("send ok");
}
void megabox::fm_openBookMark()
{
	QString nameStr = cfg->cfg_bookMarkName;
	QString pathStr = cfg->cfg_bookMarkPath;
	
	if( nameStr == "" || pathStr == "" ) {
		showNotify( lng->tr("FT_ERR0R"), lng->tr("FT_NO_BOOKMARK") );//, 2, 2 );
		return;	
	}
	QStringList nameStrList = QStringList::split( ";", nameStr );
	QStringList pathStrList = QStringList::split( ";", pathStr );
	
	ZSingleSelectDlg *dlg = new ZSingleSelectDlg(lng->tr("FT_OPEN_BOOKMARK") ,NULL);
	dlg->addItemsList(nameStrList);
	if(dlg->exec() == ZPopup::Accepted)  {
		int i = dlg->getCheckedItemIndex();
		pathStr = pathStrList[i];
		delete dlg;		
	} else {
		delete dlg;
		return;
	}
	if( pathStr != "" ) {
		m_strCurDir = pathStr;
		getFileBrowser()->setDir(m_strCurDir);	
	}	
}
void megabox::fm_editBookMark()
{
	QString nameStr = ";";
	QString pathStr = ";";
	nameStr = cfg->cfg_bookMarkName;
	pathStr = cfg->cfg_bookMarkPath;
	QString nameStrNew, pathStrNew;
	
	QStringList nameStrList = QStringList::split( ";", nameStr );
	QStringList pathStrList = QStringList::split( ";", pathStr );
	
	ZSingleSelectDlg *dlg = new ZSingleSelectDlg(lng->tr("FT_EDIT_BOOKMARK") ,NULL);
	ZSoftKey *softKey = dlg->getSoftKey();
	softKey->setText(ZSoftKey::LEFT, lng->tr("FT_DELETE") );	
	
	dlg->addItemsList(nameStrList);
	
	if(dlg->exec() == ZPopup::Accepted)  {
		int i = dlg->getCheckedItemIndex();
		delete dlg;
		dlg = NULL;
		pathStrNew = pathStrList[i]+";";
		nameStrNew = nameStrList[i]+";";	
	} else {
		delete dlg;
		return;
	}
	
	nameStr = nameStr.replace( nameStrNew, "" );
	pathStr = pathStr.replace( pathStrNew, "" );
	cfg->cfg_bookMarkName = nameStr;
	cfg->cfg_bookMarkPath = pathStr;
	cfg->saveCfg();
}
void megabox::fm_addBookMark()
{
	m_strCurDir = getFileBrowser()->getCurDir(); 

	ZSingleCaptureDlg *dlg = new ZSingleCaptureDlg(
		lng->tr("FT_INPUT_BOOKMARK_NAME"), QString("%1 : %2").arg(lng->tr("PROP_PATH")).arg(m_strCurDir),
		(ZSingleCaptureDlg::CaptureDlgType)0, NULL, "ZSingleCaptureDlg", true, 0, 0);
	ZLineEdit *line = dlg->getLineEdit();
	line->setText( QString("RokrToolsPro BookMark"));
	int ret=dlg->exec();

	QString name = "";//, path = "";
	if (ret == 1) 	{
		name = dlg->getResultText().stripWhiteSpace();//).local8Bit();
		delete dlg;
	} else 	{
		delete dlg;
		return;
	}	
	
	if( isValidFileName(name) ) {
		showQ(lng->tr("FT_ERR0R"), lng->tr("FT_INVALID"), 2);
		return;
	}
	
	cfg->cfg_bookMarkName += QString("%1;").arg(name);
	cfg->cfg_bookMarkPath += QString("%1;").arg(m_strCurDir);
	cfg->saveCfg();
}

