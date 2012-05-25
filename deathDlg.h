#ifndef DEATHDLG_H
#define DEATHDLG_H

#include <ZPopup.h>
#include <AM_LauncherClient.h>
#include <RES_ICON_Reader.h>
#include <ZMessageDlg.h>
#include <qtimer.h>
#include "myDef.h"

class deathDlg : public ZPopup
{
	Q_OBJECT
	RES_ICON_Reader iconReader;
	char command[1024];
public:
	deathDlg(QString &title, QString &text, const char* cmd);
	//deathDlg(QString &title, QString &text, QString &cmd);
	
	~deathDlg();
	void do_exec();
	int ret;
public slots:
	void system_exec();
};
#endif
