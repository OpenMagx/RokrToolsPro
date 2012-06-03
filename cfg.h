#ifndef SETTINGS_H
#define SETTINGS_H


#include <qstring.h>
#include <qobject.h>

#include <ZCheckBox.h>

#include "myDef.h"
#include "BaseDlg.h"

class Settings/*: public QObject*/
{
//   Q_OBJECT

public :
    Settings();
    ~Settings();
    void initCfg();
    void loadCfg();
    void saveCfg();


    QString procFilterStr;
    QString lngStr;

    // extra
    QString cpuLockType;
    QString mKeyType; // 音乐键第二键的功能

    // Global
    bool cfg_refreshWhenTabChg;
    bool cfg_cKeyToKill;
    bool cfg_shortMKeyToOpen;
    bool cfg_fullScreenStatus;
    bool cfg_timeHeader;
    bool cfg_hideAfterRunApp;


    // task
    bool cfg_procFilter;
    bool cfg_showNativeIcon;
    bool cfg_showJavaIcon;
    bool cfg_showJavaName;
    bool cfg_sendReturnToIdle;
    bool cfg_showNativeMgxInfo;
#ifndef NEW_JAVA_LIST
    bool cfg_markItem;
#endif

    // app
    //bool cfg_showFavBigIcon;
    bool cfg_showMgxBigIcon;
    //bool cfg_showJavaBigIcon;
    bool cfg_showMpkgBigIcon;
#ifndef NEW_JAVA_LIST
    bool cfg_runJavaInMult; //单个java程序的多开
#endif

    //fm
    bool cfg_clearBufEveryMultModeInCut;
    bool cfg_clearBufEveryMultModeInCopy;
    bool cfg_showHideObj;
    bool cfg_showMimeType;
    bool cfg_usrMultiWindow;

    QString cfg_bookMarkName;
    QString cfg_bookMarkPath;

    //reg
    QString reg_type;
    QString reg_code;
    QString reg_key;


private :
    QString m_strCfgFile;

};

#endif //SETTINGS_H
