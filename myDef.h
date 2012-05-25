#ifndef MYDEF_H
#define MYDEF_H

#define E8
//#define VE66
//#define ZN5

//#define BETA_VER
//#define DEVELOP_NO_REG

//#define DEBUG
//#define DEBUGAPP
//#define DEBUGFM
//#define DEBUGSYS
//#define DEBUG_TASK_MESSAGE
//#define DEBUG_APP_LIST
//#define DEBUG_THREAD

//#define SLOW_TEST 

//#define INSTALL_MGX_IN_FM

//#define NEWTEST

//#define COMMON_VER

//#define START_NO_HIDE // show app widget immediately

//VE66_FIX_SWITCH //don't show app when click app in menu or run in telnet

//#define qDebug printf // fixme why qDebug didn't work in ve66?


#define TAB3_JAVA

//#define VE66_APPBASE_FIX

//#define VE66_OLD_MP

/***********************************************************************************/

/***********************************************************************************/


#if defined(E8) || defined(EM30)  
	#define SCREEN_H
#else
	#define SCREEN_V
#endif	



#ifdef COMMON_VER
    #define CLICK_NO_HIDE
    #define INSTALL_MGX_IN_FM
    #define RIGHT_KEY_NO_HIDE
#endif	


#ifndef E8
    #ifdef VE66
        #define ES_EVENT //  for ve66
        #define NEW_JAVA_LIST //for ve66
		
		#ifdef VE66_OLD_MP
			#define VE66_FIX_SWITCH
			#define FIX_ITEM_SPECIAL
		#else
			#ifdef VE66_APPBASE_FIX
				#define NEW_KEY_MSG //daemon 
			#endif	
		#endif	
		
	#endif	
#endif

#ifndef NEW_JAVA_LIST
	#define RAISE_PHONE
#endif	

#ifdef SCREEN_H // for screen size 320 240
    #define SCREEN_WHIDTH 320
	#define PANEL_WHIDTH 120
	#define FM_WIDGET_WIDTH 250
#endif

#ifdef SCREEN_V // for screen size 240 320
	#define SCREEN_WHIDTH 240
	#define PANEL_WHIDTH 90 
	#define FM_WIDGET_WIDTH 200
#endif		


#define RETURN_IDLE_EVENT            5678
#define RETURN_IDLE_BY_END_KEY       -5000
#define RETURN_IDLE_BY_FLIP          -5001
#define RETURN_IDLE_BY_APP_TIMEOUT   -5002
#define RETURN_IDLE_BY_SLIDER        -5003
//Need patch libezxappbase.so
#define RETURN_IDLE_BY_ZTASKMAN      0


#define ICON_SMALL 20,20
#define ICON_BIG 40,40
	
#define KEYCODE_CLEAR 			0x1032     // 4146
#define KEYCODE_GREEN 			0x1030 
#define KEYCODE_RED 			0x1031	 
#define KEYCODE_LEFT_SK 		0x1038 //4152
#define KEYCODE_RIGHT_SK 		0x103A //4154

#define KEY_LEFT              0x1012
#define KEY_UP                0x1013
#define KEY_RIGHT             0x1014
#define KEY_DOWN              0x1015
#define KEY_CENTER            0x1004

#define TAB_PROC 0
#define TAB_MPKG 2
#define TAB_MGX 3
#define TAB_FM 4
#define TAB_FAVTAB 1

#define ROKR_ROOT "RokrToolsPro_RootFolder"

#define PHONE_INSTALL_DB "/ezxlocal/download/appwrite/am/InstalledDB"
#define SD_INSTALL_DB "/mmc/mmca1/.system/java/CardRegistry" 
#define SD_MENU_CFG "/mmc/mmca1/.system/java/MemoryCardConfig"
#define MENU_TREE "/ezxlocal/download/appwrite/am/UserMenuTree"
#define SYS_REGISTRY "/usr/data_resource/preloadapps/SysRegistry"

#define JAVA_NET_PROFILE_PATH "/ezxlocal/download/appwrite/NetProfile"

#define PH_BASE "/ezxlocal/download/mystuff"
#define SD_BASE "/mmc/mmca1"

#define PHONE_MPKG_PATH "/ezxlocal/download/mystuff/mpkgbox"
#define SD_MPKG_PATH "/mmc/mmca1/mpkgbox"
#define CFG_FILE_PATH "/ezxlocal/download/appwrite/setup/ezx_rokrtool.cfg"

#define FAV_OBJ_PATH "/ezxlocal/fav_applist.db"
#define MGX_OBJ_PATH "/ezxlocal/mgxapp_list.db"
#define MGX_MENU_PATH "/ezxlocal/mgxapp_menu.db"

#define SD_BACKUP_DIR "/mmc/mmca1/backup"
#define PHONE_BACKUP_DIR "/ezxlocal/download/mystuff/backup"

#define SD_MGX_FILE "/mmc/mmca1/mgx_file"
#define PH_MGX_FILE "/ezxlocal/download/mystuff/mgx_file"

#ifdef VE66_OLD_MP //VE66
	#define P7Z_PATH "/usr/data_resource/extra/p7zip"
#else	
	#define P7Z_PATH "/usr/local/p7zip"
#endif	

#define appUid "f0bc8dd8-b1aa-43ad-aba2-d32bcad7a9b4"

#ifdef VE66
	#define musicUid "df0581bc-7bb1-43be-98db-1940d7cb8746"
#else
	#define musicUid "0f691cc5-ba54-4c81-9a56-e0b14c0eb4f0"	
#endif	

#define ctr QString::fromLocal8Bit

#define APP_TITLE "RokrToolsPro"
#define TITLE  "RokrTools Pro\n"
#ifdef E8
	#define VERSION "Version : 3.0 beta(For E8)\n"// \nBuild 2010/08/28"
#else
	#ifdef VE66
		#define VERSION "Version : 3.0 beta(For VE66)\n"// \nBuild 2010/08/28"
	#else
		#define VERSION "Version : 3.0 beta(For ZN5)\n"// \nBuild 2010/08/28"
	#endif
#endif	
#define BUILD  "Build 2011/05/13\n"
#define AUTHOR "Author ：wanggjghost【泪の单翼天使】\n"
#ifdef BETA_VER
	#define TryInfo "试用日期，可使用到2011-05-13至2011-05-20\n"
#else	
	#define TryInfo "试用本软件：如需试用全部功能，请将系统时间设为2010-11-20\n"
#endif	
#define aboutText "Special thanks to Ro1ne\nE-Mail：41245110@qq.com\nwww.591fan.com\n"
#define REGINFO "捐助/注册说明"
#define info1 "新版本的RokrToolsPro默认只提供基本的使用功能，额外的功能需要捐助/注册以得到注册码方可使用。"
#define info2 "提供IMEI/IMSI两种注册码，IMEI和手机绑定，刷机换卡均可使用，适合于经常换卡的用户。IMSI和手机绑定，换卡(包括换sim卡不换号)后失效。捐助>10元者送IMSI一个个，>20元者送IMEI一个"
#define info3 "你可以直接在支付宝转账，也可以银行卡转账, 支付宝【wanggjghost@126.com】，银行帐号【建设银行：6222 8013 3146 1004 784】，请在邮件里注明是IMEI还是IMSI，同时留下你的IMEI或者IMSI号和有效邮箱地址，我会把注册码发到你的邮箱"
#define info4 "联系方式"
#define info5 "E-Mail：41245110@qq.com"

#define KEY_LIST ""


#endif //MYDEF_H
