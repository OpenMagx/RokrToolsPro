#include <ZApplication.h>
#include <ZSeparator.h>

#include <qtextstream.h>
#include <qcolor.h>

#include "info.h"
#include "myDef.h"
#include "source.h"
#include "lng.h"

extern Lng *lng;

stateThread::stateThread()
{
    cpuFrec = "0";
    cpuPerc = "0";

    memTotal = "0";
    memFree = "0";
    memBuffers = "0";
    memCached = "0";
    swapTotal = "0";
    swapFree = "0";
}
stateThread::~stateThread()
{
    //printf("\b========= thread end ==========\n");
}
void stateThread::initState()
{
    //printf("========= thread initState() ==========\n");

    initCpuInfo();
    initMemInfo();
}
void stateThread::run()
{
    initState();
}
void stateThread::initCpuInfo()
{
    unsigned long long freq = 0;
    FILE *f = fopen("/sys/mpm/op", "r");
    if (fscanf(f, "%llu", &freq) < 1)
    {
        fclose(f);
        return;
    }
    fclose(f);

    unsigned long long cpu = 0;
    unsigned long long nice = 0;
    unsigned long long system = 0;
    unsigned long long idle = 0;

    FILE *fp = fopen("/proc/stat", "r");
    if (fscanf(fp, "cpu %llu %llu %llu %llu", &cpu, &nice, &system, &idle) < 4)
    {
        fclose(fp);
        return;
    }
    fclose(fp);

    unsigned long long busy = cpu + nice + system;
    unsigned long long work = busy + idle;

    double usage = 100.0 * (busy-oldBusy) / (double) (work-oldWork);

    oldBusy = busy;
    oldWork = work;

    if (usage > 100)
        usage = 100;

    cpuFrec = QString( "CPU Freq : %1 MHz").arg(freq);
    cpuPerc = QString( "CPU Perc : %1 %").arg(usage);
}
void stateThread::initMemInfo()
{
    int MemFree=0;
    int MemCach=0;
    int MemTotal=0;
    int MemBuffers=0;
    int SWAPFree=0;
    int SWAPTotal=0;

    QFile file( "/proc/meminfo" );
    if ( file.open( IO_ReadOnly ) )
    {
        QTextStream stream( &file );
        QString line;
        while ( !stream.atEnd() )
        {
            line = stream.readLine();
            if ( (MemFree == 0) && (line.find("MemFree:") > -1) )
            {
                line.replace(QString("MemFree:"), QString(" "));
                line.replace(QString("kB"), QString(" "));
                MemFree = line.stripWhiteSpace().toInt();
            }
            else if ( (MemBuffers == 0) && (line.find("Buffers:") > -1) )
            {
                line.replace(QString("Buffers:"), QString(" "));
                line.replace(QString("kB"), QString(" "));
                MemBuffers = line.stripWhiteSpace().toInt();
            }
            else if ( (MemCach == 0) && (line.find("Cached:") > -1) )
            {
                line.replace(QString("Cached:"), QString(" "));
                line.replace(QString("kB"), QString(" "));
                MemCach = line.stripWhiteSpace().toInt();
            }
            else if ( (MemTotal == 0) && (line.find("MemTotal:") > -1) )
            {
                line.replace(QString("MemTotal:"), QString(" "));
                line.replace(QString("kB"), QString(" "));
                MemTotal = line.stripWhiteSpace().toInt();
            }
            else if ( (SWAPFree == 0) && (line.find("SwapFree:") > -1) )
            {
                line.replace(QString("SwapFree:"), QString(" "));
                line.replace(QString("kB"), QString(" "));
                SWAPFree = line.stripWhiteSpace().toInt();
            }
            else if ( (SWAPTotal == 0) && (line.find("SwapTotal:") > -1) )
            {
                line.replace(QString("SwapTotal:"), QString(" "));
                line.replace(QString("kB"), QString(" "));
                SWAPTotal = line.stripWhiteSpace().toInt();
            }
        }
        file.close();
    }

    memTotal = QString("Total : ")+QString::number( ((double)MemTotal)/1024,'f',1 )+"M";
    memFree = QString("Free : ")+QString::number( ((double)MemFree)/1024,'f',1)+"M";
    memBuffers = QString("Buffers : ")+QString::number( ((double)MemBuffers)/1024,'f',1)+"M";
    memCached = QString("Cached : ")+QString::number( ((double)MemCach)/1024,'f',1)+"M";
    swapTotal = QString("Swap : ")+QString::number( ((double)SWAPTotal)/1024,'f',1)+"M";
    swapFree = QString("Free : ")+QString::number( ((double)SWAPFree)/1024,'f',1)+"M";

}


stateDlg :: stateDlg() :MyBaseDlg()
{
#ifdef SCREEN_H
    pixmap = new QPixmap(320, 240);
#else
    pixmap = new QPixmap(240, 320);
#endif
    this->installEventFilter( this );

    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), this, SLOT( slotTimerEvent()));
    timer->start(1000);

    timer2 = new QTimer(this);
    connect( timer2, SIGNAL(timeout()), this, SLOT( slotTimerEvent2()));
    timer2->start(1500);

    thread = new stateThread();
    thread->start();

    this->setFullScreenMode(true);
}

stateDlg::~stateDlg()
{
    if( timer->isActive() ) {
        timer->stop();
        delete timer;
        timer = NULL;
    }
    if( thread != NULL ) {
        thread->wait();
        delete thread;
        thread = NULL;
    }

    if( pixmap != NULL ) {
        delete pixmap;
        pixmap = NULL;
    }

    this->removeEventFilter( this );
}
void stateDlg::slotTimerEvent()
{
    thread->initState();
}
void stateDlg::slotTimerEvent2()
{
    update();
}
void stateDlg::paintEvent( QPaintEvent *pe)
{
#ifdef SCREEN_H
    const int pX1 = 5;
    const int pX2 = 165;
    const int pW = 320;
    const int pH = 240;
#else
    const int pX1 = 5;
    const int pX2 = 5;
    const int pW = 240;
    const int pH = 320;
#endif

    int pY = 25;

    pixmap->fill(Qt::black);

    QPainter p( pixmap);	//定义画笔
    p.begin( pixmap);

    QFont f( qApp->font() );
    QFontMetrics fm(f);
    f.setBold(true);
    f.setPointSize(18);

    p.setFont(f);
    p.setPen(QColor(10,255,10));

    QString str = QString("CPU STATE");

    p.drawText( (240/2-str.length())/2 , pY , str );

    f.setBold(false);
    f.setPointSize(13);
    p.setFont(f);

    pY += fm.lineSpacing();
    p.drawText( pX1 , pY, thread->cpuFrec);

    pY += fm.lineSpacing();
    p.drawText( pX1 , pY, thread->cpuPerc);

    pY += fm.lineSpacing();
    p.drawLine( pX1 , pY, (pW-pX1), pY);

    f.setBold(true);
    f.setPointSize(18);
    p.setFont(f);

    str = QString("MEM STATE");

    pY += fm.lineSpacing();
    p.drawText( (pW/2-str.length())/2 , pY , str );

    pY += fm.lineSpacing();
    f.setBold(false);
    f.setPointSize(13);
    p.setFont(f);

    p.drawText( pX1 , pY , thread->memTotal );
#ifndef SCREEN_H
    pY += fm.lineSpacing();
#endif
    p.drawText( pX2 , pY , thread->memFree );


    pY += fm.lineSpacing();
    p.drawText( pX1 , pY , thread->memBuffers );
#ifndef SCREEN_H
    pY += fm.lineSpacing();
#endif
    p.drawText( pX2 , pY , thread->memCached );


    pY += fm.lineSpacing();
    p.drawText( pX1 , pY , thread->swapTotal );
#ifndef SCREEN_H
    pY += fm.lineSpacing();
#endif
    p.drawText( pX2 , pY , thread->swapFree );

    p.end();

    bitBlt( this, 0, 0, pixmap, 0, 0, pW, pH);//将内存中画布复制到屏幕
    QWidget::paintEvent( pe);
}

bool stateDlg::eventFilter(QObject* o, QEvent* pEvent)
{
    if ( QEvent::KeyRelease == pEvent->type() )
    {
        QKeyEvent *pKey = (QKeyEvent*)pEvent;
        if ( pKey->key() == KEYCODE_LEFT_SK || pKey->key() == KEYCODE_RIGHT_SK )
        {
            accept();
            return true;
        }
    }
    return QWidget::eventFilter( o, pEvent );
}


/**********************************************************************************/
/**********************************************************************************/


procInfoDlg::procInfoDlg(int pid) :MyBaseDlg()
{
    //QWidget *this = new QWidget ( this );
    QWidget *myWidget = new QWidget ( this );

    setMainWidgetTitle("Process Info");
    procPid = pid;

    buildInfo();
    /*********************** tab1 **************************/
    infoForm = new ZFormContainer(this, 0, ZSkinService::clsZFormContainer);
    //infoForm->setVScrollBarMode(ZScrollPanel::AlwaysOff);
    //infoForm->setHScrollBarMode(ZScrollPanel::AlwaysOff);
    QFont font ( qApp->font() );
    font.setPointSize ( 20 );

    myLabel = new ZLabel ( procName, myWidget, "ZLabel" , 0, (ZSkinService::WidgetClsID)4 );
    myLabel->setAutoResize(true);
    myLabel->setFont(font);
    myLabel->setFixedWidth ( SCREEN_WHIDTH );
    myLabel->setAlignment(ZLabel::AlignHCenter);
    infoForm->addChild(myLabel );

    font.setPointSize ( 15 );
    //QString infoText = QString("Pid: %1\n%2\n%3\n%4\n%5\n%6\nCmd: %7\n").arg(pid).arg(procState).arg(uid).arg(procThreads).arg(VmSize).arg(VmRSS).arg(procCmd);
    myLabel = new ZLabel ( QString("Pid : %1").arg(pid), myWidget , "ZLabel" , 0, (ZSkinService::WidgetClsID)4 );
    myLabel->setAutoResize(true);
    myLabel->setFont(font);
    myLabel->setFixedWidth ( SCREEN_WHIDTH );
    myLabel->setAlignment( ZLabel::AlignLeft );
    infoForm->addChild(myLabel );

    myLabel = new ZLabel ( procState, myWidget , "ZLabel" , 0, (ZSkinService::WidgetClsID)4 );
    myLabel->setAutoResize(true);
    myLabel->setFont(font);
    myLabel->setFixedWidth ( SCREEN_WHIDTH );
    myLabel->setAlignment( ZLabel::AlignLeft );
    infoForm->addChild(myLabel );

    myLabel = new ZLabel ( uid, myWidget , "ZLabel" , 0, (ZSkinService::WidgetClsID)4 );
    myLabel->setAutoResize(true);
    myLabel->setFont(font);
    myLabel->setFixedWidth ( SCREEN_WHIDTH );
    myLabel->setAlignment( ZLabel::AlignLeft );
    infoForm->addChild(myLabel );

    myLabel = new ZLabel ( procThreads, myWidget , "ZLabel" , 0, (ZSkinService::WidgetClsID)4 );
    myLabel->setAutoResize(true);
    myLabel->setFont(font);
    myLabel->setFixedWidth ( SCREEN_WHIDTH );
    myLabel->setAlignment( ZLabel::AlignLeft );
    infoForm->addChild(myLabel );

    myLabel = new ZLabel ( VmSize, myWidget , "ZLabel" , 0, (ZSkinService::WidgetClsID)4 );
    myLabel->setAutoResize(true);
    myLabel->setFont(font);
    myLabel->setFixedWidth ( SCREEN_WHIDTH );
    myLabel->setAlignment( ZLabel::AlignLeft );
    infoForm->addChild(myLabel );

    myLabel = new ZLabel ( VmRSS, myWidget , "ZLabel" , 0, (ZSkinService::WidgetClsID)4 );
    myLabel->setAutoResize(true);
    myLabel->setFont(font);
    myLabel->setFixedWidth ( SCREEN_WHIDTH );
    myLabel->setAlignment( ZLabel::AlignLeft );
    infoForm->addChild(myLabel );

    myLabel = new ZLabel ( QString("Cmd : %1").arg(procCmd), myWidget , "ZLabel" , 0, (ZSkinService::WidgetClsID)4 );
    myLabel->setAutoResize(true);
    myLabel->setFont(font);
    myLabel->setFixedWidth ( SCREEN_WHIDTH );
    myLabel->setAlignment( ZLabel::AlignLeft );
    infoForm->addChild(myLabel );

    /*********************** tab2 **************************/

    libBox = new ZListBox ( QString ( "%I%M" ), this, 0);
    font.setPointSize ( 13 );
    //font.setBold ( true );
    libBox->setItemFont (ZListBox::LISTITEM_REGION_C, ZListBox::StStandard, font);
    libBox->setItemFont (ZListBox::LISTITEM_REGION_C, ZListBox::StHighlighted, font);
    //libBox->setItemFont (ZListBox::LISTITEM_REGION_A, font, false );

    buildLibList();

    /*********************** add to navtab **************************/

    QPixmap imgTab0, imgTab1;//, imgTab2, imgTab3, imgTab4;
    imgTab0.load(getAppDir()+"/img/infotab0.png");
    imgTab1.load(getAppDir()+"/img/infotab1.png");

    tabWidget = new ZNavTabWidget(this);
    this->setContentWidget(tabWidget);
    //tabWidget->stopNextWhenKeyRepeat(true);

    tabWidget->addTab(infoForm, QIconSet(imgTab0), "");
    tabWidget->addTab(libBox, QIconSet(imgTab1), "");

    /*********************** sotf key **************************/

    ZSoftKey *softKey = new ZSoftKey ( NULL, this, this );
    softKey->setText(ZSoftKey::LEFT,  lng->tr("FT_CANCEL"), (ZSoftKey::TEXT_PRIORITY)0);
    softKey->setClickedSlot ( ZSoftKey::LEFT, this, SLOT ( accept() ) );
    setCSTWidget ( softKey );

    //startTimer(500);
    this->installEventFilter( this );

    startTimer(500);
}
procInfoDlg::~procInfoDlg()
{
    this->removeEventFilter( this );
}
void procInfoDlg::buildLibList()
{
    QFile f("/proc/"+QString::number(procPid)+"/maps");
    QString s;
    QString htm;

    if ( f.open( IO_ReadOnly ) )
    {
        QTextStream t( &f );

        while ( !t.eof() )
        {
            s = t.readLine();
            if ( s.find("/SYSV")==-1 )
            {
                s.remove(0, s.find("/"));
                s.stripWhiteSpace();
                if ( htm.find(s)==-1)
                {
                    htm=htm+s+"|";
                    ZListBoxItem* listitem = new ZListBoxItem ( libBox );
                    listitem->appendSubItem ( 1, QString( s ) , true, NULL );
                    libBox->insertItem ( listitem, -1, true );
                }
            }
        }
        f.close();
    }
}

void procInfoDlg::buildInfo()
{
    qDebug("============ get proc info =============");

    QFile f("/proc/"+QString::number(procPid)+"/status");
    QString str;
    //QString name;
    //QString state;
    //QString VmSize;
    //int mark = 0;
    if (f.open(IO_ReadOnly))
    {
        QTextStream in(&f);

        while (!in.eof())
        {
            //if(mark == 3) break;

            str = in.readLine();
            if (str.find("Name")>-1)
            {
                while (str.find("  ")>-1)
                    str.remove(str.find("  ")+1, 1);
                procName = str.replace(QString("Name:"), QString("") );
                procName = procName.stripWhiteSpace();
                //procName = str.stripWhiteSpace();
                //mark ++;
            }
            else if (str.find("State:")>-1)
            {
                while (str.find("  ")>-1)
                    str.remove(str.find("  ")+1, 1);
                procState = str.stripWhiteSpace();
                //mark ++;
            }
            else if (str.find("VmSize:")>-1)
            {
                while (str.find("  ")>-1)
                    str.remove(str.find("  ")+1, 1);
                VmSize = str.stripWhiteSpace();
                //mark ++;
            }
            else if (str.find("VmRSS:")>-1)
            {
                while (str.find("  ")>-1)
                    str.remove(str.find("  ")+1, 1);
                VmRSS = str.stripWhiteSpace();
                //mark ++;
            }
            else if (str.find("Threads:")>-1)
            {
                while (str.find("  ")>-1)
                    str.remove(str.find("  ")+1, 1);
                procThreads = str.stripWhiteSpace();
                //mark ++;
            }
            else if (str.find("Uid:")>-1)
            {
                if(str.find("2000")>-1)
                    uid = QString("UID:  EZX");
                else
                    uid = QString("UID:  ROOT");
            }
        }
        f.close();
    }

    char *cmd;
    char buf[1024];
    cmd =0;
    FILE *fp;
    char *name;
    int n;

    fp = fopen(QCString("/proc/"+QString::number(procPid)+"/cmdline"), "r") ;
    if((n=fread(buf, 1, sizeof(buf)-1, fp)) > 0)
    {
        if(buf[n-1]=='\n')
            buf[--n] = 0;
        name = buf;
        while(n)
        {
            if(((unsigned char)*name) < ' ')
                *name = ' ';
            name++;
            n--;
        }
        *name = 0;
        if(buf[0])
            cmd = strdup(buf);
    }
    fclose(fp);
    procCmd = QString(cmd);
}

void procInfoDlg::timerEvent(QTimerEvent *e)
{
    int tabIndex = tabWidget->currentPageIndex();
    switch(tabIndex) {
    case 0:
        infoForm->setFocus();
        break;
    case 1:
        libBox->setFocus();
        break;
    default:
        break;
    }
}
bool procInfoDlg::eventFilter(QObject* o, QEvent* pEvent)
{
    if ( tabWidget->currentPageIndex() == 0 ) //For fix not scroll ZFormContainer on one tab
    {
        if ( QEvent::KeyRelease == pEvent->type() )
        {
            QKeyEvent *pKey = (QKeyEvent*)pEvent;
            if (pKey->key() == KEY_UP)
            {
                infoForm->scrollBy(0,-20);
            }
            if (pKey->key() == KEY_DOWN)
            {
                infoForm->scrollBy(0,20);
            }
        }
    }
    return QWidget::eventFilter( o, pEvent );
}


/*
int main(int argc, char** argv)
{
	ZApplication *app = new ZApplication(argc, argv);
	stateDlg *dlg = new stateDlg();
	app->setMainWidget(dlg);

	dlg->show();

	int ret=app->exec();
	delete dlg;
	delete app;
	return ret;



}
*/

