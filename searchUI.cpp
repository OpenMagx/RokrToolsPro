


#include <ZFormContainer.h>

#include <qtimer.h>
#include <qfile.h>
#include <qdir.h>

#include "searchUI.h"
#include "lng.h"

extern Lng *lng;

using namespace std;

searchThread::searchThread(const QString &path, const QString &key, bool isSearchFile, int limitFolderDepth)
{
    m_strPath = path;
    m_strKey = key;

    m_nFolderDepth = limitFolderDepth;
    m_nCurFolderDepth = -1;

    m_bSearchStopState = false;
    m_bFinishState = false;
    m_bFileSearchState = isSearchFile;

    m_nParentFolderDepthNum = 0;

    QString a = m_strPath;
    while ( a.find("/") > -1 )
    {
        m_nParentFolderDepthNum++;
        a.remove(0, a.find("/")+1 );
    }
}
searchThread::~searchThread()
{
}
QString searchThread::getCurPath()
{
    return m_strCurPath;
}
void searchThread::stopSearch()
{
    m_bSearchStopState = true;
}
bool searchThread::isFinish()
{
    return m_bFinishState;
}
QStringList searchThread::getResultList()
{
    return m_lsResultList;
}
void searchThread::searchFile( const QString &path)
{
    if( m_bSearchStopState )  return;

    m_strCurPath = path;

    QDir dir(path);
    dir.setFilter(QDir::Dirs | QDir::Files| QDir::Hidden | QDir::System | QDir::NoSymLinks );
    dir.setMatchAllDirs(true);

    dir = dir.canonicalPath(); //ÐÞÕýÈíÁ´½Ó

    QString curPath;
    QStringList entries;
    QStringList::ConstIterator p;

    entries = dir.entryList( );

    for (p=entries.begin(); p!=entries.end(); p++)
    {
        if( m_bSearchStopState )  {
            break;
        }
        if ( (*p == ".") || (*p == "..") || (*p == QString::null) )  {
            continue;
        }

        if( path == "/") curPath = path+*p;
        else curPath = path+"/"+*p;

        if( (*p).contains(m_strKey, false) > 0 ) {
            m_lsResultList += curPath ;
        }

        if( QFileInfo(curPath).isDir() ) {
            m_nCurFolderDepth = getFolderDepth( curPath );
            if (  m_nCurFolderDepth > m_nFolderDepth ) {
                continue;
            }

            searchFile( curPath );
        } else {
            if(m_bFileSearchState) {
                if( QFileInfo(curPath).isFile() ) {
                    if( isTextFile( curPath ) ) {
                        if( fileSearched( curPath ) ) {
                            m_lsResultList += curPath ;
                        }
                    }
                }
            }
        }
    }
}
void searchThread::run()
{
    searchFile( m_strPath );
    m_bFinishState = true;
}
int searchThread::getFolderDepth( const QString &path )
{
    int i=0;
    QString a = path;
    while ( a.find("/") > -1 ) {
        i++;
        a.remove(0, a.find("/")+1 );
    }
    return i-m_nParentFolderDepthNum;
}
bool searchThread::fileSearched(const QString &file)
{
    FILE *p = popen( (const char*)(QString("grep %1 %2").arg(m_strKey).arg(file)).local8Bit(), "r");
    if( p == NULL )
        return false;

    bool state = false;
    if( fgetc(p) == EOF )  	state =  false;
    else  state =  true;
    pclose(p);
    return state;
}
bool searchThread::isTextFile(const QString &file)
{
    QString a = file;
    a = a.remove( 0, a.findRev("/")+1 );
    a = a.remove( 0, a.findRev(".")+1 );
    a = ( QString(".%1").arg(a) ).lower();

    if ( a == ".txt" || a == ".cfg" || a == ".ini" || a == ".conf" || a == ".c" ||
            a == ".cc" || a == ".cpp" || a == ".h" ||  a == ".hh" ||  a == ".lng" || a == ".log" )
        return true;
    else
        return false;
}

///////////////////////////////////////////////////////////////////

searchDlg::searchDlg( const QString &path, const QString &key, bool isSearchFile ): ZPopup((ZPopup::POPUP_TYPE_E)0)
{
    m_strPath = path;
    m_strKey = key;
    m_nStep = 0;

    m_bFileSearchState = isSearchFile;

    this->setTitle(QString("Search Agent"));
    this->setTitleIcon("info_pop.gif");

    m_zMeterModule = new ZMeterModule(8, this);
    m_zMeterModule->setAlignment(ZMeterModule::TOP);
    m_zMeter = m_zMeterModule->getMeter();
    m_zMeter->setAutoResize(true);

    this->insertChild( (ZWidget*)m_zMeterModule );

    ZSoftKey *softKey = this->getSoftKey();
    softKey->setText(ZSoftKey::LEFT, tr("TXT_RID_SOFTKEY_HIDE", "Hide"));
    softKey->setText(ZSoftKey::RIGHT, lng->tr("FT_CANCEL"));
    softKey->setClickedSlot( ZSoftKey::LEFT, this, SLOT ( slotHide() ) );
    softKey->setClickedSlot( ZSoftKey::RIGHT, this, SLOT ( slotQuit() ) );

    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), this, SLOT( slotTimerEvent()));

    m_thread = new searchThread(m_strPath, m_strKey, m_bFileSearchState);

    timer->start(500);
    m_thread->start();
}
searchDlg::~searchDlg()
{
    if( m_thread != NULL ) {
        m_thread->stopSearch();
        m_thread->wait();
        delete m_thread;
        m_thread = NULL;
    }
    if( timer->isActive() ) {
        timer->stop();
        delete timer;
        timer = NULL;
    }
}
QStringList searchDlg::getResultList()
{
    return m_lsResultList;
}
void searchDlg::slotHide()
{
    qApp->processEvents();
    this->hide();
    qApp->processEvents();
}
void searchDlg::slotTimerEvent()
{
    if( !m_thread->isFinish() )
    {
        qApp->processEvents();
        m_zMeterModule->setLabelText( QString("%1:%2").arg(lng->tr("FT_SEARCH")).arg(m_thread->getCurPath()) );
        qApp->processEvents();

        if( m_nStep <= 8 ) {
            m_zMeter->setProgress(m_nStep);
        } else {
            m_nStep = 0;
            m_zMeter->setProgress(m_nStep);
        }
        m_nStep++;
        qApp->processEvents();
    } else {
        slotQuit();
    }
}
void searchDlg::slotQuit()
{
    if( timer->isActive() ) {
        timer->stop();
    }
    while( !m_thread->isFinish() ) 	{
        m_thread->stopSearch();
        qApp->processEvents();
    }

    m_lsResultList = m_thread->getResultList();

    m_thread->wait();
    delete m_thread;
    m_thread = NULL;

    this->accept();
}

///////////////////////////////////////////////////////////////

searchUI::searchUI(const QStringList &itemList):ZPopup((ZPopup::POPUP_TYPE_E)0)
{
    m_bClickDirState = false;
    m_strClickedFile = "";
    m_lsItemList = itemList;

    m_zListBox = new ZListBox("%M", this);
    m_zListBox->enableMarquee(ZListBox::LISTITEM_REGION_A, true);
    m_zListBox->enableMarquee(ZListBox::LISTITEM_REGION_B, true);
    QFont font ( qApp->font() );
    font.setPointSize ( 14 );
    m_zListBox->setItemFont( ZListBox::LISTITEM_REGION_A, ZListBox::StStandard, font );
    m_zListBox->setItemFont( ZListBox::LISTITEM_REGION_A, ZListBox::StHighlighted, font );
    m_zListBox->setItemFont( ZListBox::LISTITEM_REGION_B, ZListBox::StStandard, font );
    m_zListBox->setItemFont( ZListBox::LISTITEM_REGION_B, ZListBox::StHighlighted, font );
    m_zListBox->setItemType("%M");

    ZFormContainer *form = new ZFormContainer(this);
    this->insertChild(form);
    form->addChild(m_zListBox);

    softKey = this->getSoftKey();
    softKey->disableClickedSlot( ZSoftKey::LEFT);
    softKey->setText( ZSoftKey::RIGHT,  tr("TXT_RID_SOFTKEY_BACK", "Back") );
    softKey->setClickedSlot(ZSoftKey::RIGHT, this, SLOT( reject() ));

    connect(m_zListBox, SIGNAL(selected(int)), this, SLOT(obj_selected(int)));

    QTimer::singleShot(100, this, SLOT( showItemList() ));
}
searchUI::~searchUI()
{
}
QString searchUI::getClickedObjName()
{
    QString tmp = m_strClickedFile;
    return tmp.remove( 0, tmp.findRev("/")+1 );
}
QString searchUI::getClickedObjDirname()
{
    if( m_bClickDirState ) {
        return m_strClickedFile;
    } else {
        QString tmp = m_strClickedFile;
        return tmp.remove( tmp.findRev("/")+1, tmp.length()-1 );
    }
}
bool searchUI::isClickedDir()
{
    return m_bClickDirState;
}
void searchUI::obj_selected(int i)
{
    ZSettingItem *pItem = (ZSettingItem *)(m_zListBox->item(i));

    m_strClickedFile = pItem->getSubItemText(0,0);

    if(QFileInfo(m_strClickedFile).isFile()) {
        m_bClickDirState = false;
    } else {
        m_bClickDirState = true;
    }
    accept();
}
void searchUI::showItemList()
{
    QStringList::ConstIterator p;
    ZSettingItem *pItem = NULL;
    for (p = m_lsItemList.begin(); p != m_lsItemList.end(); ++p )
    {
        pItem = new ZSettingItem(m_zListBox, QString("%M"));
        pItem->appendSubItem(0, *p, false, SETTINGITEM_REGION_A);
        m_zListBox->insertItem(pItem);
    }
}
