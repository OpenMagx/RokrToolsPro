
#include <ZConfig.h>

//#include <qstring.h>
#include <qfile.h>
#include <qcopchannel_qws.h>

#include "myDef.h"
#include "source.h"
#include "BaseDlg.h"
#include "cfg.h"

Settings::Settings()/*:QObject()*/
{
    m_strCfgFile = QString(CFG_FILE_PATH);
	if( !QFile::exists(m_strCfgFile) )   
		initCfg();

	loadCfg();	
	
}

Settings::~Settings()
{
    #ifdef DEBUG
    qDebug("===== delete cfg settings \n");
	#endif
}

void Settings::initCfg()
{
    qDebug("=== init cfg =====\n");
	ZConfig cfg(m_strCfgFile);
	
	cfg.writeEntry("FILTER", "filter", ""); 
	cfg.writeEntry("LNG", "lng", "zh-cn.qm"); 

	cfg.writeEntry("GLOBAL", "cfg_refreshWhenTabChg", true);
	cfg.writeEntry("GLOBAL", "cfg_cKeyToKill", false);
	cfg.writeEntry("GLOBAL", "cfg_shortMKeyToOpen", false);
	cfg.writeEntry("GLOBAL", "cfg_fullScreenStatus", false);
	cfg.writeEntry("GLOBAL", "cpuLockType", "0");
	cfg.writeEntry("GLOBAL", "mKeyType", "");
	cfg.writeEntry("GLOBAL", "cfg_timeHeader", false);
	cfg.writeEntry("GLOBAL", "cfg_hideAfterRunApp", false);
	
	cfg.writeEntry("TASK", "procFilter", false);
	cfg.writeEntry("TASK", "showNativeIcon", true);
	cfg.writeEntry("TASK", "showJavaIcon", true);
	cfg.writeEntry("TASK", "showJavaName", true);
	cfg.writeEntry("TASK", "sendReturnToIdle", false);
	cfg.writeEntry("TASK", "cfg_showNativeMgxInfo", false);
	#ifndef NEW_JAVA_LIST
	cfg.writeEntry("TASK", "cfg_markItem", false);
	#endif
	
	//cfg.writeEntry("APP", "showFavBigIcon", true);	
	cfg.writeEntry("APP", "showMgxBigIcon", true);	
	cfg.writeEntry("APP", "showMpkgBigIcon", true);	
	#ifndef NEW_JAVA_LIST
	cfg.writeEntry("APP", "cfg_runJavaInMult", true);
	#endif	
	
	cfg.writeEntry("FILEMGR", "cfg_clearBufEveryMultModeInCut", true);
	cfg.writeEntry("FILEMGR", "cfg_clearBufEveryMultModeInCopy", true);	
	cfg.writeEntry("FILEMGR", "cfg_showHideObj", true);	
	cfg.writeEntry("FILEMGR", "cfg_showMimeType", true);
	cfg.writeEntry("FILEMGR", "cfg_usrMultiWindow", true);
	
	cfg.writeEntry("FILEMGR", "cfg_bookMarkName", "");	
	cfg.writeEntry("FILEMGR", "cfg_bookMarkPath", "");	

	
	cfg.writeEntry("REG", "reg_type", "IMEI");	
	cfg.writeEntry("REG", "reg_code", "");
    cfg.writeEntry("REG", "reg_key", "");	
	
	cfg.flush();		
}



void Settings::loadCfg()
{
    ZConfig cfg(m_strCfgFile);
	
	procFilterStr = cfg.readEntry("FILTER", "filter", ""); 
	lngStr = cfg.readEntry("LNG", "lng", "zh-cn.qm"); 
	
	cfg_refreshWhenTabChg = cfg.readBoolEntry("GLOBAL", "cfg_refreshWhenTabChg", true);
	cfg_cKeyToKill = cfg.readBoolEntry("GLOBAL", "cfg_cKeyToKill", false);
	cfg_shortMKeyToOpen = cfg.readBoolEntry("GLOBAL", "cfg_shortMKeyToOpen", true);
	cfg_fullScreenStatus = cfg.readBoolEntry("GLOBAL", "cfg_fullScreenStatus", false);
	cfg_timeHeader = cfg.readBoolEntry("GLOBAL", "cfg_timeHeader", false);
	cfg_timeHeader = cfg.readBoolEntry("GLOBAL", "cfg_hideAfterRunApp", false);
	
	cpuLockType = cfg.readEntry("GLOBAL", "cpuLockType", "0");
	mKeyType = cfg.readEntry("GLOBAL", "mKeyType", "");
	
	cfg_procFilter = cfg.readBoolEntry("TASK", "procFilter", false);
	cfg_showNativeIcon = cfg.readBoolEntry("TASK", "showNativeIcon", false);
	cfg_showJavaIcon = cfg.readBoolEntry("TASK", "showJavaIcon", false);
	cfg_showJavaName = cfg.readBoolEntry("TASK", "showJavaName", false);
	cfg_sendReturnToIdle = cfg.readBoolEntry("TASK", "sendReturnToIdle", true);
	cfg_showNativeMgxInfo = cfg.readBoolEntry("TASK", "cfg_showNativeMgxInfo", false);
	#ifndef NEW_JAVA_LIST
	cfg_markItem = cfg.readBoolEntry("TASK", "cfg_markItem", true);
	#endif
	
	//cfg_showFavBigIcon = cfg.readBoolEntry("APP", "showFavBigIcon", true);	
	cfg_showMgxBigIcon = cfg.readBoolEntry("APP", "showMgxBigIcon", true);	
	cfg_showMpkgBigIcon = cfg.readBoolEntry("APP", "showMpkgBigIcon", true);	
	#ifndef NEW_JAVA_LIST
	cfg_runJavaInMult = cfg.readBoolEntry("APP", "cfg_runJavaInMult", true);
	#endif	
	
    cfg_clearBufEveryMultModeInCut = cfg.readBoolEntry("FILEMGR", "cfg_clearBufEveryMultModeInCut", true);	
    cfg_clearBufEveryMultModeInCopy = cfg.readBoolEntry("FILEMGR", "cfg_clearBufEveryMultModeInCopy", true);	
	cfg_showHideObj = cfg.readBoolEntry("FILEMGR", "cfg_showHideObj", true);	
	cfg_showMimeType = cfg.readBoolEntry("FILEMGR", "cfg_showMimeType", true);
	cfg_usrMultiWindow = cfg.readBoolEntry("FILEMGR", "cfg_usrMultiWindow", true);
	cfg_bookMarkName = cfg.readEntry("FILEMGR", "cfg_bookMarkName", "");	
	cfg_bookMarkPath = cfg.readEntry("FILEMGR", "cfg_bookMarkPath", "");	
	
	
	reg_type = cfg.readEntry("REG", "reg_type", "IMEI"); 
	reg_code = cfg.readEntry("REG", "reg_code", "");
	reg_key = cfg.readEntry("REG", "reg_key", "");
}

void Settings::saveCfg()
{
    qDebug("=== save cfg =====\n");
	
	ZConfig cfg(m_strCfgFile);
	
	cfg.writeEntry("FILTER", "filter", procFilterStr); 
	cfg.writeEntry("LNG", "lng", lngStr); 
	
	cfg.writeEntry("GLOBAL", "cfg_refreshWhenTabChg", cfg_refreshWhenTabChg);
	cfg.writeEntry("GLOBAL", "cfg_cKeyToKill", cfg_cKeyToKill);
	cfg.writeEntry("GLOBAL", "cfg_shortMKeyToOpen", cfg_shortMKeyToOpen);
	cfg.writeEntry("GLOBAL", "cfg_fullScreenStatus", cfg_fullScreenStatus);
	cfg.writeEntry("GLOBAL", "cfg_timeHeader", cfg_timeHeader);
	cfg.writeEntry("GLOBAL", "cfg_hideAfterRunApp", cfg_timeHeader);

	cfg.writeEntry("GLOBAL", "cpuLockType", cpuLockType);	
	cfg.writeEntry("GLOBAL", "mKeyType", mKeyType);
	
	cfg.writeEntry("TASK", "procFilter", cfg_procFilter);
	cfg.writeEntry("TASK", "showNativeIcon", cfg_showNativeIcon);
	cfg.writeEntry("TASK", "showJavaIcon", cfg_showJavaIcon);
	cfg.writeEntry("TASK", "showJavaName", cfg_showJavaName);
	cfg.writeEntry("TASK", "sendReturnToIdle", cfg_sendReturnToIdle);
	cfg.writeEntry("TASK", "cfg_showNativeMgxInfo", cfg_showNativeMgxInfo);
	#ifndef NEW_JAVA_LIST
	cfg.writeEntry("TASK", "cfg_markItem", cfg_markItem);
	#endif

	//cfg.writeEntry("APP", "showFavBigIcon", cfg_showFavBigIcon);	
	cfg.writeEntry("APP", "showMgxBigIcon", cfg_showMgxBigIcon);	
	cfg.writeEntry("APP", "showMpkgBigIcon", cfg_showMpkgBigIcon);
	
	#ifndef NEW_JAVA_LIST
	cfg.writeEntry("APP", "cfg_runJavaInMult", cfg_runJavaInMult);
	#endif	
	
    cfg.writeEntry("FILEMGR", "cfg_clearBufEveryMultModeInCut", cfg_clearBufEveryMultModeInCut);	
    cfg.writeEntry("FILEMGR", "cfg_clearBufEveryMultModeInCopy", cfg_clearBufEveryMultModeInCopy);	
	cfg.writeEntry("FILEMGR", "cfg_showHideObj", cfg_showHideObj);	
	cfg.writeEntry("FILEMGR", "cfg_showMimeType", cfg_showMimeType);
	cfg.writeEntry("FILEMGR", "cfg_usrMultiWindow", cfg_usrMultiWindow);		
	cfg.writeEntry("FILEMGR", "cfg_bookMarkName", cfg_bookMarkName);	
	cfg.writeEntry("FILEMGR", "cfg_bookMarkPath", cfg_bookMarkPath);	

    cfg.writeEntry("REG", "reg_type", reg_type);
	cfg.writeEntry("REG", "reg_code", reg_code);
	cfg.writeEntry("REG", "reg_key", reg_key);	
	
	cfg.flush();
	
	//if(cpuLockType == "0")
	//	system("pmtool -d 1"); 
	//if(cfg_fullScreenStatus == true)
	//	QCopChannel::send("/EZX/ROKRTOOLS", "SetFullScreenMode()");
	//else
	//	QCopChannel::send("/EZX/ROKRTOOLS", "BackFullScreenMode()");		
}


