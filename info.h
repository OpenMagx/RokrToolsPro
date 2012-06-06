#ifndef INFO_H
#define INFO_H

#define QT_THREAD_SUPPORT
#include <ZKbMainWidget.h>
#include <ZLabel.h>
#include <ZScrollPanel.h>
#include <ZSoftKey.h>
#include <ZPopup.h>
#include <ZListBox.h>
#include <ZImage.h>
#include <ZFormContainer.h>
#include <ZNavTabWidget.h>
#include <RES_ICON_Reader.h>

#include <qstring.h>
#include <qthread.h>
#include <qtimer.h>
#include <qlayout.h>

#include "BaseDlg.h"

class stateThread : public QThread
{
public :
    stateThread();
    ~stateThread();

    void initState();

    QString cpuFrec;
    QString cpuPerc;

    QString memTotal;
    QString memFree;
    QString memBuffers;
    QString memCached;
    QString swapTotal;
    QString swapFree;

protected:
    virtual void run();

private :
    void initCpuInfo();
    void initMemInfo();

    int oldBusy, oldWork;
};

class stateDlg : public MyBaseDlg
{

    Q_OBJECT

public:
    stateDlg();
    ~stateDlg();

public slots:
//	void setCPU();
//	void setMEM();
//	void refresh();

protected slots:
    void slotTimerEvent();
    void slotTimerEvent2();

//	virtual void timerEvent(QTimerEvent *e);
//	{
    //refresh();
//			thread->initState();
    //slotPaint();
//	update();
//		ZKbMainWidget::timerEvent(e);
//	}
protected:
    void paintEvent( QPaintEvent *);
    bool  eventFilter( QObject *, QEvent * );

private:
    stateThread *thread;

    //ZImage *img;
//	QImage img;
//	QString cpuF;
//	QString cpuP;
//	ZLabel *sepLabel;
//	ZLabel *sepLabel2;

///	ZLabel *cpuLabel;
//	ZLabel *cpuLabel2;

//	ZLabel *memLabel;//[6];
    /*ZLabel *memLabel2;
    ZLabel *memLabel3;
    ZLabel *memLabel4;
    ZLabel *memLabel5;
    ZLabel *memLabel6;	*/
    QPixmap *pixmap;
    QTimer *timer;
    QTimer *timer2;

//	ZScrollPanel *zsp;
    //ZFormContainer *form;

//	ZSoftKey *softkey;
    QString cpuinfo;

    int oldBusy, oldWork;

};

/**********************************************************************************/
/**********************************************************************************/

class procInfoDlg : public MyBaseDlg
{
    Q_OBJECT

public:
    procInfoDlg(int pid);
    ~procInfoDlg();
    void buildLibList();
    void buildInfo();
    void timerEvent(QTimerEvent *e);
    //void setProcPid(int procPid);

protected:
    bool  eventFilter( QObject *, QEvent * );

public slots:
    //void exit();

private:
    //ZScrollPanel *zsp;
    //ZLabel *lb;
    //ZSoftKey *softKey;
    //RES_ICON_Reader iconReader;
    //ZScrollPanel *zsp;
    ZNavTabWidget *tabWidget;
    ZFormContainer *infoForm;
    ZListBox *libBox;
    ZLabel * myLabel;

    //QString mInfoText;
    QString procName;
    QString procCmd;
    QString VmSize;
    QString VmRSS;
    QString procState;
    QString procThreads;
    QString uid;

    int procPid;
    //int pid;
    //QString procText;
};





#endif

