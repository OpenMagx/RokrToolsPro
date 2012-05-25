
#include <ZApplication.h>
#include <ZFormContainer.h>
#include <ZConfig.h>
#include <ZSeparator.h>
#include <ZSingleCaptureDlg.h>
#include <ZComboBox.h>
#include <ZSoftKey.h>
#include <ZLabel.h>

#include <qdir.h>
#include <qcopchannel_qws.h>

#include <iostream>

#include "settingDlg.h"
#include "myDef.h"
#include "source.h"
#include "cfg.h"
#include "myapplication.h"
#include "lng.h"
#include "codeComp.h"
#include "appAddDlg.h"
//#include "megabox.h"

extern Settings *cfg;
extern MyApplication *app;
extern Lng *lng;
//extern ResTool *res;
extern CDCP *codeComp;

SettingDlg::SettingDlg():MyBaseDlg()
{
	isCancel = true;
	isFullScreenMode = false;
	
	setMainWidgetTitle("App Setting");

	form0 = new ZFormContainer(this, 0, ZSkinService::clsZFormContainer);
	//listBox0 = new ZListBox("%C%M", this);
	form1 = new ZFormContainer(this, 0, ZSkinService::clsZFormContainer);
	form2 = new ZFormContainer(this, 0, ZSkinService::clsZFormContainer);
	form3 = new ZFormContainer(this, 0, ZSkinService::clsZFormContainer);
	form4 = new ZFormContainer(this, 0, ZSkinService::clsZFormContainer);

	tabWidget = new ZNavTabWidget(this);
	this->setContentWidget(tabWidget);
	tabWidget->stopNextWhenKeyRepeat(true);

	QPixmap imgTab; 
	imgTab.load(getAppDir()+"/img/tab0.png");
	tabWidget->addTab(form0, QIconSet(imgTab), "");		
	imgTab.load(getAppDir()+"/img/tab1.png");
	tabWidget->addTab(form1, QIconSet(imgTab), ""); 
	imgTab.load(getAppDir()+"/img/tab2.png");
    tabWidget->addTab(form2, QIconSet(imgTab), ""); 	
	imgTab.load(getAppDir()+"/img/tab3.png");
    tabWidget->addTab(form3, QIconSet(imgTab), ""); ;			
	imgTab.load(getAppDir()+"/img/tab4.png");
    tabWidget->addTab(form4, QIconSet(imgTab), ""); 	

	ZSoftKey *softKey = new ZSoftKey ( NULL, this, this );
	softKey->setText(ZSoftKey::LEFT,  lng->tr("FT_SELECT"), (ZSoftKey::TEXT_PRIORITY)0);
    softKey->setText(ZSoftKey::RIGHT, lng->tr("FT_CANCEL"), (ZSoftKey::TEXT_PRIORITY)0);
	softKey->setClickedSlot(ZSoftKey::RIGHT, this, SLOT (reject()));
	softKey->setClickedSlot(ZSoftKey::LEFT, this, SLOT (accept()));
	setCSTWidget(softKey);
  
	imei = getIMEI();
	imsi = getIMSI();	    

	buildTab0();
	buildTab1(); 
	buildTab2();
	buildTab3();
	buildTab4();
	
	startTimer(300);
}

SettingDlg::~SettingDlg()
{
}

void SettingDlg::buildTab0()
{
    cout<<" ================ buildTab0() ============"<<endl;
	
	// Global
 	refreshWhenTabChg = new ZCheckBox(lng->tr("refreshWhenTabChg"),form0); 
	cKeyToKill = new ZCheckBox(lng->tr("cKeyToKill"),form0); 
	#ifndef ZN5
	shortMKeyToOpen = new ZCheckBox(lng->tr("shortMKeyToOpen"),form0);
	#endif	
	timeHeader = new ZCheckBox(lng->tr("timeHeader"),form0);
	hideAfterRunApp	= new ZCheckBox(lng->tr("hideAfterRunApp"),form0);
	//fullScreenStatus = new ZCheckBox(lng->tr("fullScreenStatus"),form0); 
    //task
	procFilter = new ZCheckBox(lng->tr("procFilter"),form0); 
	showNativeIcon = new ZCheckBox(lng->tr("showNativeIcon"),form0); 
	showJavaIcon = new ZCheckBox(lng->tr("showJavaIcon"),form0); 
	showJavaName = new ZCheckBox(lng->tr("showJavaName"),form0); 
	sendReturnToIdle = new ZCheckBox(lng->tr("sendReturnToIdle"),form0); 
	showNativeMgxInfo = new ZCheckBox(lng->tr("showNativeMgxInfo"),form0); 
	#ifndef NEW_JAVA_LIST
	markItem = new ZCheckBox(lng->tr("markItem")+"(beta version)",form0);
	#endif
	
	//app
	showMgxBigIcon = new ZCheckBox(lng->tr("showMgxBigIcon"),form0); 
	showMpkgBigIcon = new ZCheckBox(lng->tr("showMpkgBigIcon"),form0); 
	#ifndef NEW_JAVA_LIST
	runJavaInMult = new ZCheckBox(lng->tr("runJavaInMult"),form0); 
	#endif
	//fm
	clearBufEveryMultModeInCut = new ZCheckBox(lng->tr("clearBufEveryMultModeInCut"),form0); 
	clearBufEveryMultModeInCopy = new ZCheckBox(lng->tr("clearBufEveryMultModeInCopy"),form0); 
	showHideObj = new ZCheckBox(lng->tr("showHideObj"),form0); 
	showMimeType = new ZCheckBox(lng->tr("showMimeType"),form0); 
	showMultiWindow = new ZCheckBox(lng->tr("showMultiWindow"),form0); 

    ////////////////////////////////////////////////////

 	refreshWhenTabChg->setChecked(cfg->cfg_refreshWhenTabChg);
	cKeyToKill->setChecked(cfg->cfg_cKeyToKill);
	#ifndef ZN5
	shortMKeyToOpen->setChecked(cfg->cfg_shortMKeyToOpen);
	#endif
	timeHeader->setChecked(cfg->cfg_timeHeader);
	hideAfterRunApp->setChecked(cfg->cfg_hideAfterRunApp);
	//fullScreenStatus->setChecked(cfg->cfg_fullScreenStatus);
   	// task
    procFilter->setChecked(cfg->cfg_procFilter);
	showNativeIcon->setChecked(cfg->cfg_showNativeIcon);
	showJavaIcon->setChecked(cfg->cfg_showJavaIcon);
	showJavaName->setChecked(cfg->cfg_showJavaName);
	sendReturnToIdle->setChecked(cfg->cfg_sendReturnToIdle);
	showNativeMgxInfo->setChecked(cfg->cfg_showNativeMgxInfo);
	#ifndef NEW_JAVA_LIST
	markItem->setChecked(cfg->cfg_markItem);
	#endif	
	// app
	//bool cfg_showFavBigIcon;
	showMgxBigIcon->setChecked(cfg->cfg_showMgxBigIcon);
	showMpkgBigIcon->setChecked(cfg->cfg_showMpkgBigIcon);
	#ifndef NEW_JAVA_LIST
	runJavaInMult->setChecked(cfg->cfg_runJavaInMult);
	#endif
	
	//fm
	clearBufEveryMultModeInCut->setChecked(cfg->cfg_clearBufEveryMultModeInCut);
	clearBufEveryMultModeInCopy->setChecked(cfg->cfg_clearBufEveryMultModeInCopy);	
    showHideObj->setChecked(cfg->cfg_showHideObj);
	showMimeType->setChecked(cfg->cfg_showMimeType);
	showMultiWindow->setChecked(cfg->cfg_usrMultiWindow);
	
   /////////////////////////////////////

 	form0->addChild(refreshWhenTabChg);
	form0->addChild(cKeyToKill);
	#ifndef ZN5
	form0->addChild(shortMKeyToOpen);  
	#endif
	form0->addChild(timeHeader);  
	form0->addChild(hideAfterRunApp);  
	//form0->addChild(fullScreenStatus);  

    ZSeparator *sep = new ZSeparator(form0);	
	form0->addChild(sep);
   	// task
    form0->addChild(procFilter);
	form0->addChild(showNativeIcon);
	form0->addChild(showJavaIcon);
	form0->addChild(showJavaName);
	form0->addChild(sendReturnToIdle);
	form0->addChild(showNativeMgxInfo);
	#ifndef NEW_JAVA_LIST
	form0->addChild(markItem);	
	#endif

    sep = new ZSeparator(form0);	
	form0->addChild(sep);	
	// app
	//bool cfg_showFavBigIcon);
	form0->addChild(showMgxBigIcon);
	form0->addChild(showMpkgBigIcon);
	#ifndef NEW_JAVA_LIST
	form0->addChild(runJavaInMult);
	#endif
	
    sep = new ZSeparator(form0);	
	form0->addChild(sep);	
	//fm
	form0->addChild(clearBufEveryMultModeInCut);
	form0->addChild(clearBufEveryMultModeInCopy);
	form0->addChild(showHideObj);
	form0->addChild(showMimeType);
	form0->addChild(showMultiWindow);
}

void SettingDlg::buildTab1()
{
    lngcb = new ZComboBox(form1);
	lngcb->setTitle(lng->tr("FT_CHOOSE_LNG"));
	lngcb->setDlgTitle(lng->tr("FT_CHOOSE_LNG"));
	getLngList();
	
	occb = new ZComboBox(form1);
	occb->setTitle(lng->tr("FT_CPU_LOCK"));
	occb->setDlgTitle(lng->tr("FT_CHOOSE_LOCK_TYPE"));
	setOcList();
	
	#ifndef ZN5
	mKeycb = new ZComboBox(form1);
	mKeycb->setTitle(lng->tr("FT_2ND_MKEY_TYPE"));
	mKeycb->setDlgTitle(lng->tr("FT_CHOOSE_2ND_MKEY_TYPE"));	
	mKeycb->insertItem(lng->tr("FT_DEFAULT"), 0);
	mKeycb->insertItem(lng->tr("FT_CUSTOMER"), 1);	
	if(cfg->mKeyType == "")
		mKeycb->setCurrentItem(0);
	else 
		mKeycb->setCurrentItem(1);

	#endif
	
	form1->addChild(lngcb);
	form1->addChild(occb);
	#ifndef ZN5
	form1->addChild(mKeycb);
	connect(mKeycb, SIGNAL( activated(int) ), this, SLOT( slotMkey(int) ) );
	#endif
}

void SettingDlg::buildTab2()
{
	QString text = "IMEI : "+imei+"\n"+"IMSI : "+imsi;
	tab2Label = new ZLabel(text, form2, "ZLabel" , 0, (ZSkinService::WidgetClsID)4);
	tab2Label->setAutoResize(true);
	tab2Label->setFixedWidth ( SCREEN_WHIDTH );		
	form2->addChild(tab2Label);
	
    ZSeparator *sep = new ZSeparator(form2);	
	form2->addChild(sep);	
	
	if( !codeComp->cpcp() )
	{
		regcb = new ZComboBox(form2);
		regcb->setTitle(lng->tr("FT_CHOOSE_REGTYPE"));
		regcb->setDlgTitle(lng->tr("FT_REGTYPE"));
		regcb->insertItem("IMEI", 0);
		regcb->insertItem("IMSI", 1);
		form2->addChild(regcb);
		
		if( cfg->reg_type == "IMEI" )
			regcb->setCurrentItem(0);
		else if ( cfg->reg_type == "IMSI" )	
			regcb->setCurrentItem(1);
			
		sep = new ZSeparator(form2);	
		form2->addChild(sep);

		QPixmap bt;
		bt.load(getAppDir()+"/img/reg.png");
		button = new ZPressButton(bt, lng->tr("FT_CONFIRM_REG") ,form2);
		button->setTitlePosition(ZPressButton::TitleLeft);
		form2->addChild(button);
		
		sep = new ZSeparator(form2);	
		form2->addChild(sep);
		sep = new ZSeparator(form2);	
		form2->addChild(sep);
		sep = new ZSeparator(form2);	
		form2->addChild(sep);	
		
		connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
	}
}

void SettingDlg::buildTab3()
{
	form3->setVScrollBarMode(ZScrollPanel::AlwaysOff);
	form3->setHScrollBarMode(ZScrollPanel::AlwaysOff);	

	QFont font ( qApp->font() ); 
	font.setBold(true);
	font.setPointSize ( 20 );
	
	tab3Label = new ZLabel(TITLE, this, "ZLabel" , 0, (ZSkinService::WidgetClsID)4);
	tab3Label->setFont( font );
	tab3Label->setAutoResize(true);
	tab3Label->setFixedWidth( SCREEN_WHIDTH);
	tab3Label->setAlignment( ZLabel::AlignHCenter );
	form3->addChild(tab3Label);//, 0, 0);
	
	QString text;
	text += ctr(VERSION);
	text += ctr(BUILD);
	text += ctr(AUTHOR);
	text += ctr(aboutText);
	text += ctr(TryInfo);

	tab3Label = new ZLabel(text, this, "ZLabel" , 0, (ZSkinService::WidgetClsID)4);	
	tab3Label->setAutoResize(true);
	tab3Label->setFixedWidth( SCREEN_WHIDTH);
	tab3Label->setAlignment( ZLabel::AlignLeft );
	form3->addChild(tab3Label);

}
void SettingDlg::buildTab4()
{
	form4->setVScrollBarMode(ZScrollPanel::AlwaysOff);
	form4->setHScrollBarMode(ZScrollPanel::AlwaysOff);		
	
	QFont font ( qApp->font() ); 
	font.setBold(true);
	font.setPointSize ( 20 );
	
	QString regInfo = ctr(REGINFO);
	tab4Label = new ZLabel(regInfo , this, "ZLabel" , 0, (ZSkinService::WidgetClsID)4);
	tab4Label->setFont( font );
	tab4Label->setAutoResize(true);
	tab4Label->setFixedWidth( SCREEN_WHIDTH);
	tab4Label->setAlignment( ZLabel::AlignHCenter );
	form4->addChild(tab4Label);//, 0, 0);

	QString text;
	text += ctr("1. ")+ctr(info1)+"\n";
	text += ctr("2. ")+ctr(info2)+"\n";
	text += ctr("3. ")+ctr(info3)+"\n";
	text += ctr("4. ")+ctr(info4)+"\n";
	text += ctr(info5)+"\n";
	
	tab4Label = new ZLabel(text, this, "ZLabel" , 0, (ZSkinService::WidgetClsID)4);	
	tab4Label->setAutoResize(true);
	tab4Label->setFixedWidth( SCREEN_WHIDTH);
	tab4Label->setAlignment( ZLabel::AlignLeft );
	form4->addChild(tab4Label);	
	
}
void SettingDlg::timerEvent(QTimerEvent *e)
{
	int tabIndex = tabWidget->currentPageIndex();
    if(tabIndex == 2) {
		if( codeComp->cpcp() ) 	tab2Label->setFocus();
	}
	else if(tabIndex == 3) tab3Label->setFocus();
	else if(tabIndex == 4) tab4Label->setFocus();

}

void SettingDlg::accept()
{
 	//qDebug("========== setting dlg accept ===============\n");	
	
	cfg->cfg_refreshWhenTabChg = refreshWhenTabChg->isChecked();
	cfg->cfg_cKeyToKill = cKeyToKill->isChecked();
	#ifndef ZN5
	cfg->cfg_shortMKeyToOpen = shortMKeyToOpen->isChecked();
	#endif
	cfg->cfg_timeHeader = timeHeader->isChecked();
	cfg->cfg_hideAfterRunApp = hideAfterRunApp->isChecked();
	//cfg->cfg_fullScreenStatus = fullScreenStatus->isChecked();
	// task
    cfg->cfg_procFilter = procFilter->isChecked();
	cfg->cfg_showNativeIcon = showNativeIcon->isChecked();
	cfg->cfg_showJavaIcon = showJavaIcon->isChecked();
	cfg->cfg_showJavaName = showJavaName->isChecked();
	cfg->cfg_sendReturnToIdle = sendReturnToIdle->isChecked();
	cfg->cfg_showNativeMgxInfo = showNativeMgxInfo->isChecked();
	#ifndef NEW_JAVA_LIST
	cfg->cfg_markItem = markItem->isChecked();
	#endif
	
	// app
	//bool cfg_showFavBigIcon;
	cfg->cfg_showMgxBigIcon = showMgxBigIcon->isChecked();
	cfg->cfg_showMpkgBigIcon = showMpkgBigIcon->isChecked();
	#ifndef NEW_JAVA_LIST
	cfg->cfg_runJavaInMult = runJavaInMult->isChecked();
	#endif
	
	//fm
	cfg->cfg_clearBufEveryMultModeInCut = clearBufEveryMultModeInCut->isChecked();
	cfg->cfg_clearBufEveryMultModeInCopy = 	clearBufEveryMultModeInCopy->isChecked();
	cfg->cfg_showHideObj = showHideObj->isChecked();
	cfg->cfg_showMimeType = showMimeType->isChecked();
	cfg->cfg_usrMultiWindow = showMultiWindow->isChecked();
	
	cfg->lngStr = lngcb->currentText();
	
	QString octmp = occb->currentText();

	if(octmp != "399" && octmp != "532")
		cfg->cpuLockType = "0";
	else
		cfg->cpuLockType = octmp;
	#ifndef NEW_JAVA_LIST
	if(!codeComp->cpcp() && (!refreshWhenTabChg->isChecked() || cKeyToKill->isChecked() || shortMKeyToOpen->isChecked() || procFilter->isChecked() || sendReturnToIdle->isChecked() || markItem->isChecked() ) )
	#else
		#ifdef ZN5
	if(!codeComp->cpcp() && (!refreshWhenTabChg->isChecked() || cKeyToKill->isChecked() || procFilter->isChecked() || sendReturnToIdle->isChecked() ) )
		#else
	if(!codeComp->cpcp() && (!refreshWhenTabChg->isChecked() || cKeyToKill->isChecked() || shortMKeyToOpen->isChecked() || procFilter->isChecked() || sendReturnToIdle->isChecked() ) )	
		#endif
	#endif	
	{	
		QString text = lng->tr("POP_THIS_FUNCTION_NEED_DONATE")+"\n";
		if( !refreshWhenTabChg->isChecked() )
			text += lng->tr("refreshWhenTabChg")+"::";	
		if( cKeyToKill->isChecked() )		
			text += lng->tr("cKeyToKill")+"::";
		#ifndef ZN5
		if( shortMKeyToOpen->isChecked() )		
			text +=	lng->tr("shortMKeyToOpen")+"::";
		#endif	
		if( procFilter->isChecked() )	
			text += lng->tr("procFilter")+"::";
		if(	sendReturnToIdle->isChecked() )
			text += lng->tr("sendReturnToIdle")+"::";
		#ifndef NEW_JAVA_LIST	
		if(	markItem->isChecked() )
			text += lng->tr("markItem")+"::";
		#endif	

		showNotify( lng->tr("POP_DONATE"), text, 0, 5 );
	}

	cfg->saveCfg();
	
	if(cfg->cpuLockType == "0")
		system("pmtool -d 1"); 

	isCancel = false;
	
 	//qDebug("========== setting dlg accept end ===============\n");		
	
	MyBaseDlg::reject();
}

void SettingDlg::reject()
{
	isCancel = true;
	MyBaseDlg::reject();
} 

void SettingDlg::buttonClicked()
{
	QString regType = regcb->currentText();
	QString title = lng->tr("FT_CONFIRM_REG");
	QString text = QString("%1:%2 \n").arg(lng->tr("FT_REGTYPE")).arg(regType);
	if( regType == "IMEI" )
		text += QString("IMEI:%1").arg(imei);
	else
		text += QString("IMSI:%1").arg(imsi);
		
	ZSingleCaptureDlg* dlg = new ZSingleCaptureDlg(title, text,
	    (ZSingleCaptureDlg::CaptureDlgType)0, NULL, "ZSingleCaptureDlg", true, 0, 0);	
		
	if( dlg->exec() == ZPopup::Accepted )  {
		cfg->reg_key = dlg->getResultText().stripWhiteSpace();
		delete dlg;
		dlg = NULL;	
		
		cfg->reg_type = regType;
		cfg->saveCfg();

		if( codeComp->cpcp() )  {
			showNotify("", QString("Register Successfully !!") );
			tabWidget->setCurrentPage(0);
		} else {
			showNotify("", QString("Register Error !!"), 1);
		}	
	} else {
		delete dlg;
		dlg = NULL;	
	}
}
void SettingDlg::slotMkey(int index)
{
	int i = -1;
	i = index;
	if( i == -1 )
		return;
		
	if( i == 0 ) 
		cfg->mKeyType = "";
	else if(i == 1)
	{
		QString title = lng->tr("FT_CHOOSE_2ND_MKEY_TYPE");
		appAddDlg *dlg = new appAddDlg(title, appAddDlg::DT_getAppInfo, false);
		dlg->exec();
		bool isCancel = dlg->isCancel;
		if( !isCancel ) {
			cfg->mKeyType = dlg->appGname;
			delete dlg;
			dlg = NULL;	
		} else {
			delete dlg;
			dlg = NULL;	
		}
	}
	#ifndef ZN5
	if(cfg->mKeyType == "")
		mKeycb->setCurrentItem(0);
	else 
		mKeycb->setCurrentItem(1);	
	#endif		
}
void SettingDlg::getLngList()
{
	QString curLang = cfg->lngStr; 
	
	QDir dir( QString("%1/lng").arg(getAppDir()) );
	dir.setFilter(QDir::Files);
	if ( !dir.isReadable() )
		return;
	
	QString qname;
	QStringList entries;
	QStringList::ConstIterator p;   
	entries = dir.entryList();	
	
	int i = 0;
	for (p=entries.begin(); p!=entries.end(); p++)
	{
		if ( *p == "." || *p == ".." ) 
			continue;	
		qname = QString(*p);
		if (qname.find(".qm") > -1)
		{
			#ifdef DEBUG 
			std::cout<<"getLngList: LNG->"<< qname<<std::endl;
			#endif
			lngcb->insertItem( qname, i);
			if(qname == curLang)
				lngcb->setCurrentItem(i);
			++i;
		}
	}
}

void SettingDlg::setOcList()
{
	QString type0 = lng->tr("FT_CLOSE_OC");
	QString type1("399");
	QString type2("532");
	
	occb->insertItem(type0, 0);
	occb->insertItem(type1, 1);
	occb->insertItem(type2, 2);
	
	if(cfg->cpuLockType == "0")
		occb->setCurrentItem(0);
	else if(cfg->cpuLockType == "399")
		occb->setCurrentItem(1);
	else
		occb->setCurrentItem(2);		
}

