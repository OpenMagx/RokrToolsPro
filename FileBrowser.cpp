

#include <AM_LauncherClient.h>

#include <qdir.h>
#include <qfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "myDef.h"
#include "FileBrowser.h"
#include "source.h"

//#ifdef ZN5
//#define ZListBoxItem ZSettingItem
//#endif

FileBrowser::FileBrowser(const QString &dir, const QString& type, QWidget* parent, WFlags f)
    : ZListBox(type, parent, 0)
{
    //nameFilter = QString("QDir::Dirs | QDir::Files| QDir::Hidden | QDir::System");
    //dirSort = QString("QDir::DirsFirst | QDir::Name | QDir::IgnoreCase");
    m_strCurDir = dir;
    m_eFilterType = SHOW_ALL;
    m_eMode = NORMAL;
    fileCount = 0;
    dirCount = 0;
    isItemInfo = false;
    m_bMimeTypeState = false;
    m_strAppDir = getAppDir();

    //#ifdef ZN5
    QFont font ( qApp->font() );
    font.setPointSize ( 16 );
    this->setItemFont( ZListBox::LISTITEM_REGION_A, ZListBox::StStandard, font );
    font.setPointSize ( 18 );
    this->setItemFont( ZListBox::LISTITEM_REGION_A, ZListBox::StHighlighted, font );
    this->setItemFont( ZListBox::LISTITEM_REGION_A, ZListBox::StSelected, font );
    this->setItemFont( ZListBox::LISTITEM_REGION_A, ZListBox::StHighlightSelected, font );
    //#endif

    connect ( this, SIGNAL ( highlighted ( int ) ), this, SLOT ( itemHighlighted ( int ) ) );
    connect ( this, SIGNAL ( selected ( int ) ), this, SLOT ( itemSelected ( int ) ) );
}

FileBrowser::~FileBrowser()
{
#ifdef DEBUG
    qDebug("===== delete file browser \n");
#endif
}

void FileBrowser::init()
{
    //mMode = NORMAL;
    setDir(m_strCurDir, m_eMode);//, nameFilter);//, dirSort);
    //setCurrentItem(0);
}

void FileBrowser::setDir(QString &pdir, ITEMMODE mode)//, const QString &sFilter, const QString &sSort)
{
    //clear();
#ifdef DEBUGFM
    std::cout<<"cur dir is "<<pdir<<std::endl;
#endif
    fileCount = 0;
    dirCount = 0;

    m_strCurDir = pdir;
    m_eMode = mode;
    this->clear();

    QDir dir(pdir);
#ifndef ZN5
    dir.setMatchAllDirs (true);
#endif
    if ( !dir.isReadable() ) {
        return;
    }

    int index = 0;
    QString itemType = QString("%I%M");
    SettingItemRegion eRegion = SETTINGITEM_REGION_A;
    if( mode == MULTI ) {
        index ++;
        itemType = QString("%C%I%M");
        eRegion = SETTINGITEM_REGION_B;
    }

    QStringList entries;
    QStringList::ConstIterator p;
    ZSettingItem* pItem = NULL;
    QPixmap icon;

    if(m_eFilterType == SHOW_ALL) {
        entries = dir.entryList( QDir::Dirs | QDir::Hidden );
    } else {
        entries = dir.entryList( QDir::Dirs );
    }
    entries.sort();
    for (p=entries.begin(); p!=entries.end(); p++)
    {
        if ( *p == "." || *p == ".." ) continue;

        icon = getItemIcon( adjustFile(QString(*p)) );

        pItem = new ZSettingItem(this, itemType);
        pItem->appendSubItem(index+1, *p, false, eRegion);
        pItem->setPixmap(index, icon);
        this->insertItem(pItem);
        dirCount++;
    }

    if(m_eFilterType == SHOW_ALL) {
        entries = dir.entryList( QDir::Files | QDir::Hidden );//| QDir::System);
    } else {
        entries = dir.entryList( QDir::Files );//| QDir::System);
    }
    entries.sort();
    for (p=entries.begin(); p!=entries.end(); p++)
    {
        if ( *p == "." || *p == ".." ) continue;

        icon = getItemIcon( adjustFile(QString(*p)) );

        pItem = new ZSettingItem(this, itemType);
        pItem->appendSubItem(index+1, *p, false, eRegion);
        pItem->setPixmap(index, icon);
        this->insertItem(pItem);
        fileCount++;
    }

#ifdef SCREEN_V
    pItem = new ZSettingItem(this, QString("%I%M100%M"));
#else
    pItem = new ZSettingItem(this, QString("%I%M140%M"));
#endif
    pItem->appendSubItem(1, pdir);
    pItem->appendSubItem(2, QString( " %1 Dirs %2 Files").arg(dirCount).arg(fileCount) );
    this->insertItem(pItem, 0);//, false);
    this->setCurrentItem(0);
}

void FileBrowser::itemHighlighted(int index)
{
    if( index == 0 )  {
        m_strFilePath = m_strCurDir;
        m_strFileName = m_strCurDir;
        isItemInfo = true;
        return;
    }

    if( m_eMode == MULTI) {
        return;
    }
    ZSettingItem *pItem = (ZSettingItem *)item(index);
    m_strFileName = pItem->getSubItemText(1, 0);//, false);
    m_strFilePath = adjustFile(m_strFileName);
    isItemInfo = false;

    emit fileHighlighted(index);
}

void FileBrowser::itemSelected(int index)
{
    if( index == 0 )  {
        isItemInfo = true;
        if( m_eMode != MULTI) {
            cdBack();
        }
        return;
    }
    isItemInfo = false;
    QString sName = QString::null;
    QString sPath  = QString::null;

    ZSettingItem *pItem =(ZSettingItem *)item(index);

    sName = pItem->getSubItemText(1, 0);
    sPath = adjustFile(sName);

#ifdef DEBUGFM
    std::cout<<"path is "<<sPath<<std::endl;
#endif

    if( m_eMode == NORMAL ) {
        if( QFileInfo(sPath).isDir() ) 	{
            m_strCurDir = sPath;
            setDir(m_strCurDir);
        } else {
            if( QFileInfo(sPath).isFile() )
            {
#ifdef INSTALL_MGX_IN_FM
                if( sPath.endsWith( QString(".mgx") ) )
                {
                    QString path = getAppDir();
                    system(QString("exec '%1/mgxInstaller' -d '%2' &").arg(path).arg(sPath));
                }
                else
#endif
                    AM_LauncherClient::openDocument(sPath);
            }
        }
    }
}
void FileBrowser::setMode(ITEMMODE mode)
{
    m_eMode = mode;
}
QStringList FileBrowser::getCheckedItemList()
{
    QStringList list;// = "";
    for (unsigned int i=0 ; i<this->count(); i++) {
        if ( this->itemChecked(i) ) {
            list += ( (ZSettingItem*)item(i) )->getSubItemText(2, 0);
        }
    }
    return 	list;
}
void FileBrowser::cdBack()
{
    QString newDir;
    int ret = m_strCurDir.findRev('/');
    if (ret <= 0) newDir = "/";
    else newDir = m_strCurDir.left(ret);
    m_strCurDir = newDir;
#ifdef DEBUGFM
    std::cout<<"cur dir is "<<newDir<<std::endl;
#endif
    setDir(m_strCurDir, NORMAL);
}
void FileBrowser::setCursor(const QString &keyItem)
{
    if(m_eMode == MULTI) return;

    for (unsigned int i=0 ; i<this->count(); i++) {
        if( ( ((ZSettingItem*)item(i))->getSubItemText(1, 0) ).find(keyItem) > -1 ) {
            this->setCurrentItem(i);
            break;
        }
    }
}
FileBrowser::FILE_TYPE FileBrowser::getFileType( const QString &fullPath )
{
    if( QFileInfo(fullPath).isDir() ) {
        return FILE_DIR ;
    }
    if( QFileInfo(fullPath).isSymLink() ) {
        return FILE_SYMLINK ;
    }

    QString filePath = fullPath;

    filePath = filePath.remove( 0, filePath.findRev("/")+1 );
    filePath = filePath.remove( 0, filePath.findRev(".")+1 );

    filePath = ( QString(".%1").arg(filePath) ).lower();

    if (  filePath == ".7z" || filePath == ".zip" || filePath == ".tar" ||
            filePath == ".gz" || filePath == ".bz2" || filePath == ".rar" ||
            filePath == ".cab" || filePath == ".iso" || filePath == ".arg" ||
            filePath == ".z" )
        return FILE_PACKAGE;

    if ( filePath == ".lin" || filePath == ".sh" )
        return FILE_SCRIPT;

    if ( filePath == ".mgs" )
        return FILE_THEME;

    if ( filePath == ".ttf" || filePath == ".ttc" )
        return FILE_FONT;

    if ( filePath == ".km" )
        return FILE_KM;

    if ( filePath == ".qkt" )
        return FILE_QKT;

    if ( filePath == ".bmp" || filePath == ".png" || filePath == ".jpg" || filePath == ".jpeg" || filePath == ".gif")
        return FILE_IMG;

    if ( filePath == ".flv" || filePath == ".avi" || filePath == ".flac" || filePath == ".vob"|| filePath == ".rm" ||
            filePath == ".rmvb" || filePath == ".3gp")
        return FILE_MEDIA;

    if ( filePath == ".txt" || filePath == ".cfg" || filePath == ".chm" ||
            filePath == ".ini" || filePath == ".conf" || filePath == ".c" ||
            filePath == ".cc" || filePath == ".cpp" || filePath == ".h" ||
            filePath == ".hh" || filePath == ".java" || filePath == ".lng" ||
            filePath == ".desktop" || filePath == ".log" || filePath == ".pro")
        return FILE_TEXT;

    return FILE_OTHER;
}
QPixmap FileBrowser::getItemIcon( const QString &fullPath )
{
    if( !m_bMimeTypeState ) {
        return m_iconReader.getIcon( (QFileInfo(fullPath).isFile()) ? "fm_insert_file_small.bmp" : "fm_folder_small.bmp" );
    }

    QString iconFile;
    FILE_TYPE type = getFileType( fullPath );
    switch(type) {
    case FILE_PACKAGE :
        iconFile = m_strAppDir+"/img/img_pkg.png";
        break;
    case FILE_FONT :
        iconFile = m_strAppDir+"/img/img_font.png";
        break;
    case FILE_SCRIPT :
        iconFile = m_strAppDir+"/img/img_script.png";
        break;
    case FILE_THEME :
        iconFile = m_strAppDir+"/img/img_theme.png";
        break;
    case FILE_KM :
    case FILE_QKT :
        iconFile = m_strAppDir+"/img/img_km.png";
        break;
    case FILE_IMG :
        iconFile = m_strAppDir+"/img/img_picture.png";
        break;
    case FILE_MEDIA:
        iconFile = m_strAppDir+"/img/img_media.png";
        break;
    case FILE_TEXT :
        iconFile = m_strAppDir+"/img/img_text.png";
        break;
    case FILE_DIR :
        iconFile = m_strAppDir+"/img/img_dir.png";
        break;
    case FILE_SYMLINK:
        iconFile = m_strAppDir+"/img/img_symlink.png";
        break;
    case FILE_OTHER:
    default:
        iconFile = m_strAppDir+"/img/img_file.png";
        break;
    }
    return QPixmap(iconFile);
}
QString FileBrowser::adjustFile(const QString &file)
{
    if(m_strCurDir == "/")
        return QString("/%1").arg(file);
    else
        return QString("%1/%2").arg(m_strCurDir).arg(file);
}
void FileBrowser::selAll()
{
    if(m_eMode == NORMAL) return;
    for(unsigned int i=0; i<count(); i++)
        checkItem(i, true);
}

void FileBrowser::selNone()
{
    if(m_eMode == NORMAL) return;
    for(unsigned int i=0; i<count(); i++)
        checkItem(i, false);
}
void FileBrowser::enableMimeType(bool enable)
{
    m_bMimeTypeState = enable;
}
bool FileBrowser::isEnableMimeType()
{
    return m_bMimeTypeState;
}
void FileBrowser::setFilter(FILTERTYPE t)
{
    m_eFilterType = t;
}

ITEMMODE FileBrowser::getMode()
{
    return m_eMode;
}

QString FileBrowser::getCurDir()
{
    return m_strCurDir;
}

QString FileBrowser::getFileName()
{
    return m_strFileName;
}

QString FileBrowser::getFilePath()
{
    return m_strFilePath;
}


