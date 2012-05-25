#ifndef DAEMON_H
#define DAEMON_H

#define QT_THREAD_SUPPORT

#include <ZWidget.h>
#include <ZApplication.h>

#include <qdatastream.h>
#include <qcopchannel_qws.h>
#include <qthread.h>
#include <qtimer.h>


#include "myDef.h"
#include <iostream>

class Daemon : public QObject,public QThread
{
    Q_OBJECT
	
public:
	Daemon(QObject *parent = 0);
	~Daemon();
protected:
	void run();
private:
	int keycount;
	int keycode;
	int status;	
	bool isDoHold;
	QTimer *timer;

private slots:
	void slotChannel(const QCString &msg,const QByteArray &data);
	void slotUpdateTimer();

};

#endif


