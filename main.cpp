#include <ZApplication.h>
#include <ZMessageDlg.h>
#include <ZNoticeDlg.h>

#include <qdatetime.h>
#include <qtranslator.h>
#include <qtextcodec.h>

#include "megabox.h"
#include "myapplication.h"
#include "cfg.h"
#include "source.h"
#include "daemon.h"
#include "lng.h"
#include "myDef.h"
#include "codeComp.h"
#include "UtilsDBOperation.h"

Settings *cfg;
megabox *gui;
MyApplication *app;
Daemon *mDaemon;
Lng *lng;
//ResTool *res;
CDCP *codeComp;
UtilsDBOperation *mgxDB;
UtilsDBOperation *favDB;


#ifdef VE66_FIX_SWITCH
void note()
{
	ZNoticeDlg *dlg = new ZNoticeDlg((ZNoticeDlg::Type)1 ,2000, "", ctr("App daemon start OK!!"));     
	dlg->exec();
	delete dlg;
    dlg = NULL;
}
#endif
int main(int argc, char** argv)
{
    int ret = -1;
	app = new MyApplication(argc,argv);
	gui = NULL;

	if(QCopChannel::isRegistered("/EZX/ROKRTOOLS"))
	{
		qDebug("app is registered , raise app \n");
		QCopChannel::send("/EZX/ROKRTOOLS","RaiseUI()");
		ret = 0;
	}	
	
	if (ret == -1)
	{
		#ifdef VE66_FIX_SWITCH
		note();
		#endif
		mgxDB = new UtilsDBOperation( MGX_OBJ_PATH );
		favDB = new UtilsDBOperation( FAV_OBJ_PATH );
		
		cfg = new Settings();
		codeComp = new CDCP();
		lng = new Lng(getAppDir()+"lng/"+cfg->lngStr);
		gui = new megabox(NULL, NULL);
		#ifdef DEBUG
		std::cout<<"lng file is "<<getAppDir()+"lng/"+cfg->lngStr<<std::endl;
		#endif
		printf("start daemon\n");
		mDaemon = new Daemon(NULL);
		
		#ifdef START_NO_HIDE
		printf("start no hide\n");
		app->setMainWidget(gui);
		gui->show();
		#endif		
		
		//lng->start();
		mDaemon->start();

		ret = app->exec();
		
		mDaemon->wait();
		//lng->wait();
		//delete res;
		//res = NULL;
		delete cfg;
		cfg = NULL;
		delete codeComp;
		codeComp = NULL;
		delete gui;
		gui = NULL;
		delete mDaemon;
		mDaemon = NULL;	
		delete lng;
		lng = NULL;
		delete mgxDB;
		mgxDB = NULL;
		delete favDB;
		favDB = NULL;
	}
	delete app;
	app = NULL;
	return ret;
}

