
#include <ZConfig.h>
#include <qtimer.h>
#include <qdatetime.h>
#include "settingDlg.h"
#include "source.h"
#include "daemon.h"
#include "myapplication.h"
#include "megabox.h"
#include "codeComp.h"

extern Settings *cfg;
extern megabox *gui;
extern CDCP *codeComp;

Daemon::Daemon(QObject *parent)
    : QObject(parent)
{
	keycount = 0;
	isDoHold = false;
}

Daemon::~Daemon()
{
    #ifdef DEBUG
	qDebug("====== delete daemon \n");
	#endif
	if( timer->isActive() )
		timer->stop();
	delete timer;
	timer = NULL;
}

void Daemon::slotChannel(const QCString &msg,const QByteArray &data)
{
	if(msg == "keyMsg(int,int)") // hardkey bc
	{
	    QDataStream stream(data,IO_ReadOnly);
	    stream>>keycode>>status;
		#ifdef DEBUG
		cout<<"keycode   "<<keycode<<"    status "<<status<<endl;
		#endif 
		if( (keycode==4168 && status==2) || (keycode==65285 && status==1) || (keycode==4147 && status==1) )
		{
			if(keycount < 5)  {
				if(codeComp->cpcp()) {
					if(!cfg->cfg_shortMKeyToOpen) {
				        QCopChannel::send("/EZX/ROKRTOOLS","RaiseUI()");
					}else 
					{
					    if( cfg->mKeyType == "")
							launchAppObject( QString(musicUid) );
						else
							launchAppObject(cfg->mKeyType);						
					}	
				}else
				{
					qDebug("app opened, short music key to open app\n");
					QCopChannel::send("/EZX/ROKRTOOLS","RaiseUI()");
				}
		    }			
			keycount = 0;
			isDoHold = false;
		}
		if(keycode == 4147 && status == 4) {
			if(codeComp->cpcp()) {
				if(!cfg->cfg_shortMKeyToOpen) {
					if( cfg->mKeyType == "")
						launchAppObject( QString(musicUid) );
					else
						launchAppObject(cfg->mKeyType);	
				}	
				else
					QCopChannel::send("/EZX/ROKRTOOLS","RaiseUI()");
			}else
			{
				launchAppObject(musicUid);		
			}
		}
		//key hold
	    if(keycode == 4168 && status == 268435457)
		{
			keycount++;
			if(keycount >= 5) {
			    if(!isDoHold) {
					if(codeComp->cpcp()) {
						if(!cfg->cfg_shortMKeyToOpen) {
							if( cfg->mKeyType == "")
								launchAppObject( QString(musicUid) );
							else
								launchAppObject(cfg->mKeyType);	
						}else
						{
							QCopChannel::send("/EZX/ROKRTOOLS","RaiseUI()");
						}
					}else
					{
						launchAppObject(musicUid);	
					}						
					isDoHold = true;
				}
			}
		}
	}
	if (msg == "LCDOn")
	{
		if(cfg->cpuLockType != "0")
		{
			system("pmtool -d 0");
			system( QString("echo %1 > /sys/mpm/op").arg(cfg->cpuLockType) );
		}	
	}
	if (msg == "LCDOff")
	{
		if(cfg->cpuLockType != "0")
			system("pmtool -d 1");	
	}
	if(msg == "FreeCpu()")
	{
		if ( !timer->isActive() ) {
			timer->start(3000, true);		
		}
		else {	
			timer->stop();
			timer->start(3000, true);	
		}	
	}
	if(msg == "LockCpu()")
	{
		if( !timer->isActive() ) {
			system("pmtool -d 1");	
			system("pmtool -d 0");
			system( QString("echo 532 > /sys/mpm/op") );
		}	
	}
}

void Daemon::slotUpdateTimer()
{
	if(cfg->cpuLockType != "0") {
		system("pmtool -d 1");	
		system("pmtool -d 0");
		system( QString("echo %1 > /sys/mpm/op").arg(cfg->cpuLockType) );			
	}
	else {
		system("pmtool -d 1");		
	}
}

void Daemon::run()
{
	#ifdef DEBUG
	qDebug("====== start daemon \n");
	#endif
	QCopChannel *bcchnl = new QCopChannel("/hardkey/bc", this);
    connect(bcchnl,SIGNAL(received(const QCString &,const QByteArray &)),this,SLOT(slotChannel(const QCString &,const QByteArray &)));	
 
	QCopChannel *syschnl = new QCopChannel("EZX/System", this); 
    connect(syschnl,SIGNAL(received(const QCString &,const QByteArray &)),this,SLOT(slotChannel(const QCString &,const QByteArray &)));
	
	QCopChannel *marschnl = new QCopChannel("/EZX/marsRtChnl", this); 
    connect(marschnl,SIGNAL(received(const QCString &,const QByteArray &)),this,SLOT(slotChannel(const QCString &,const QByteArray &)));	
	
	timer = new QTimer(this);
	connect( timer, SIGNAL( timeout() ), this, SLOT( slotUpdateTimer() ) );
}

