
#include <ZConfig.h>
#include <qstringlist.h>
#include <qdir.h>
#include <qfile.h>

#include "appListBox.h"
#include "cfg.h"
#include "lng.h"
#include "source.h"
#include "codeComp.h"

extern Settings *cfg;
extern Lng *lng;
extern CDCP *codeComp;
extern UtilsDBOperation *mgxDB;
extern UtilsDBOperation *favDB;

appListBox::appListBox(appListBoxType t, const QString& type, QWidget* parent, WFlags f)
    : ZListBox(type, parent, 0)
{
    m_eType = t;
    m_eMode = NORMAL;
    m_bOpenMenuState = false;
    m_bIsSD = false;

    m_nIndex = 0;
    m_eRegion = SETTINGITEM_REGION_A;

    m_strMgxMenuListStr = QString::null;
    //m_strMenuId = QString::null;
    m_strMenuId = QString(ROKR_ROOT);
    m_strExistsMgxListStr = QString::null;
    m_strConfigFile	= QString::null;

    m_strAppDir = getAppDir();

    switch( m_eType ) {
    case ALB_FAV:
        m_uDB = favDB;
        break;
    case ALB_MGX :
        m_uDB = mgxDB;
        break;
    default:
        m_uDB = NULL;
        break;
    }

    //#ifdef ZN5
    QFont font ( qApp->font() );
    font.setPointSize ( 22 );
    this->setItemFont( ZListBox::LISTITEM_REGION_A, ZListBox::StStandard, font );
    font.setPointSize ( 24 );
    this->setItemFont( ZListBox::LISTITEM_REGION_A, ZListBox::StHighlighted, font );
    this->setItemFont( ZListBox::LISTITEM_REGION_A, ZListBox::StSelected, font );
    this->setItemFont( ZListBox::LISTITEM_REGION_A, ZListBox::StHighlightSelected, font );
    //#endif

    connect ( this, SIGNAL ( highlighted ( int ) ), this, SLOT ( slotHighlighted ( int ) ) );
    connect ( this, SIGNAL ( selected ( int ) ), this, SLOT ( slotSelected ( int ) ) );
}

appListBox::~appListBox()
{
#ifdef DEBUG
    qDebug("===== delete list box \n");
#endif
    if( (m_eType != ALB_FAV) && (m_eType != ALB_MGX) ) {
#ifdef DEBUG
        qDebug("===== delete fav/mgx box \n");
#endif
        if( m_uDB != NULL ) {
            delete m_uDB;
            m_uDB = NULL;
        }
    }
}

void appListBox::setDB(QString file)
{
#ifdef TAB3_JAVA
    if( m_eType == ALB_JAVA )
#else
    if( m_eType == ALB_MPKG )
#endif
    {
        if( file == QString(PHONE_INSTALL_DB) ) {
            m_bIsSD = false;
        } else {
            m_bIsSD = true;
        }
        if( m_uDB == NULL ) {
            m_uDB = new UtilsDBOperation(file);
        } else {
            delete m_uDB;
            m_uDB = new UtilsDBOperation(file);
        }
    } else {
        return;
    }
}
void appListBox::bulidAppList()
{
    switch( m_eType )
    {
    case ALB_FAV :
        buildFavList();
        break;
        //case ITEM_MPKG_OBJ :
        //	buildMpkgList();
        //	break;
#ifdef TAB3_JAVA
    case ALB_JAVA :
        buildJavaList();
        break;
#endif
    case ALB_MGX :
        //ChkMgxList();
        buildMenuList();
        buildMgxList();
        break;
    default :
        break;
    }
}
void appListBox::openMenu(const QString &menuId)
{
    m_bOpenMenuState = true;
    m_strMenuId = menuId;

    this->clear();

    switch( m_eType )
    {
    case ALB_FAV :
        buildFavList();
        break;
        //case ITEM_MPKG_OBJ :
        //	buildMpkgList();
        //	break;
#ifdef TAB3_JAVA
    case ALB_JAVA :
        buildJavaList();
        break;
#endif
    case ALB_MGX :
        //ChkMgxList();
        //buildMenuList();
        buildMgxList();
        break;
    default :
        break;
    }
    this->setCurrentItem(0);
}
QString appListBox::getCurMenuId()
{
    return m_strMenuId;
}
void appListBox::cdBack()
{
    m_bOpenMenuState = false;
    //m_strMenuId = QString::null;
    m_strMenuId = QString(ROKR_ROOT);

    this->clear();

    switch( m_eType )
    {
    case ALB_FAV :
        buildFavList();
        break;
        //case ITEM_MPKG_OBJ :
        //	buildMpkgList();
        //	break;
#ifdef TAB3_JAVA
    case ALB_JAVA :
        buildJavaList();
        break;
#endif
    case ALB_MGX :
        buildMenuList();
        buildMgxList();
        break;
    default :
        break;
    }

    this->setCurrentItem(0);
}
void appListBox::setMode(ITEMMODE mode)
{
    m_eMode = mode;
    if( m_eMode == MULTI ) {
        m_nIndex = 1;
        m_eRegion = SETTINGITEM_REGION_B;
        //this->setItemType("%C%I%M%I");
#ifdef SCREEN_V
        this->setItemType("%C%I%M120%I40");
#else
        this->setItemType("%C%I%M200%I40");
#endif
    } else {
        m_nIndex = 0;
        m_eRegion = SETTINGITEM_REGION_A;
        this->setItemType("%I%M%I");
#ifdef SCREEN_V
        this->setItemType("%I%M160%I");
#else
        this->setItemType("%I%M240%I");
#endif
    }
}
void appListBox::selAll()
{
    if(m_eMode == NORMAL)
        return;
    for(unsigned int i=0; i<count(); i++)
        this->checkItem(i, true);
}
void appListBox::selNone()
{
    if( m_eMode == NORMAL)
        return;
    for(unsigned int i=0; i<count(); i++)
        this->checkItem(i, false);
}
QStringList appListBox::getSelectedItemNameList()
{
    QStringList list;
    for (unsigned int i=0 ; i<this->count(); i++) {
        if ( this->itemChecked(i) ) {
            list += ( (ZTaskItem*)item(i) )->getSubItemText(m_nIndex+1, 0);
        }
    }
    return 	list;
}
QStringList appListBox::getItemNameList()
{
    QStringList list;
    for (unsigned int i=0 ; i<this->count(); i++) {
        list += ( (ZTaskItem*)item(i) )->getSubItemText(m_nIndex+1, 0);
    }
    return 	list;
}
QString appListBox::getSelectedItemNameStr( const QString& sep)
{
    QString str;
    for (unsigned int i=0 ; i<this->count(); i++) {
        if ( this->itemChecked(i) ) {
            str += ( (ZTaskItem*)item(i) )->getSubItemText(m_nIndex+1, 0);
            str += sep;
        }
    }
    return str;
}
QString appListBox::getItemNameStr( const QString& sep )
{
    QString str;
    for (unsigned int i=0 ; i<this->count(); i++) {
        str += ( (ZTaskItem*)item(i) )->getSubItemText(m_nIndex+1, 0);
        str += sep;
    }
    return str;
}
QStringList appListBox::getSelectedItemGNameList()
{
    QStringList list;
    for (unsigned int i=0 ; i<this->count(); i++) {
        if ( this->itemChecked(i) ) {
            list += ( (ZTaskItem*)item(i) )->getGname();
        }
    }
    return 	list;
}
QStringList appListBox::getItemGNameList()
{
    QStringList list;
    for (unsigned int i=0 ; i<this->count(); i++) {
        list += ( (ZTaskItem*)item(i) )->getGname();
    }
    return 	list;
}
QString appListBox::getSelectedItemGNameStr( const QString& sep )
{
    QString str;
    for (unsigned int i=0 ; i<this->count(); i++) {
        if ( this->itemChecked(i) ) {
            str += ( (ZTaskItem*)item(i) )->getGname();
            str += sep;
        }
    }
    return 	str;
}
QString appListBox::getItemGNameStr( const QString& sep )
{
    QString str;
    for (unsigned int i=0 ; i<this->count(); i++) {
        str += ( (ZTaskItem*)item(i) )->getGname();
        str += sep;
    }
    return 	str;
}
ITEMMODE appListBox::getMode()
{
    return m_eMode;
}

appListBox::appListBoxType appListBox::getCurListType()
{
    return  m_eType;
}
void appListBox::slotHighlighted(int index)
{
    if( index == 0 && m_bOpenMenuState )
        return;
    if(m_eMode == MULTI)
        return;
    emit itemHighlighted(index);
}
void appListBox::slotSelected(int index)
{
    if( index == 0 && m_bOpenMenuState )   {
        cdBack();
    } else {
        if(m_eMode == MULTI)
            return;
        else
            emit itemSelected(index);
    }
}
void appListBox::buildFavList()
{
#ifdef DEBUG
    printf("***********buildFavList()\n");
#endif
    if( m_uDB == NULL )
        return;

    QString apt = QString("-1");
    QStringList grouplist;
    QStringList::ConstIterator p;
    ZTaskItem* pItem = NULL;

    grouplist = m_uDB->getAllGroupList();

    for (p = grouplist.begin(); p != grouplist.end(); ++p )
    {
#ifdef SCREEN_V
        pItem = new ZTaskItem ( this, QString ("%I%M160%I") );
#else
        pItem = new ZTaskItem( this, QString("%I%M240%I"));
#endif
        pItem->setType(ITEM_FAV_OBJ);
        pItem->setGname(*p);
        pItem->appendSubItem(m_nIndex+1, m_uDB->getObjectName(*p), false, m_eRegion);
        pItem->setPixmap(m_nIndex, m_uDB->getObjectIconPixmap(*p, ICON_SMALL));

        apt = m_uDB->getObjectType( *p );
        if( apt == "0" ) {
            pItem->setPixmap( m_nIndex+2, QPixmap(m_strAppDir+"/img/native_s.png") );
        } else if( apt == "1" ) {
            pItem->setPixmap( m_nIndex+2, QPixmap(m_strAppDir+"/img/java_s.png") );
        } else if( apt == "2" ) {
            pItem->setPixmap( m_nIndex+2, QPixmap(m_strAppDir+"/img/native_s.png") );
        } else if( apt == "3" ) {
            pItem->setPixmap( m_nIndex+2, QPixmap(m_strAppDir+"/img/java_s.png") );
        } else {
            pItem->setPixmap( m_nIndex+2, QPixmap(m_strAppDir+"/img/app_s.png") );
        }
        this->insertItem(pItem);
    }
}
#ifdef TAB3_JAVA
void appListBox::buildJavaList()
{
    if( m_uDB == NULL )
        return;
#ifdef DEBUG
    cout<<"***** ***** buildJavaList() "<<endl;
#endif
    QString text2 = QString::null;
    QStringList grouplist;
    QStringList::ConstIterator p;
    ZTaskItem* pItem = NULL;

    grouplist = m_uDB->getJavaGroupList();

    for (p = grouplist.begin(); p != grouplist.end(); ++p )
    {
        if(cfg->cfg_showMpkgBigIcon) {
            pItem = new ZTaskItem ( this, QString ( "%I%M" ) );
        } else
        {
#ifdef SCREEN_V
            pItem = new ZTaskItem ( this, QString ("%I%M160%I") );
#else
            pItem = new ZTaskItem(this, QString("%I%M240%I"));
#endif
        }
        pItem->setType(ITEM_JAVA_OBJ);
        pItem->setGname( *p);
        pItem->setPath( m_bIsSD );
#ifdef DEBUG
        cout<<"***** ***** buildJavaList() add "<<m_uDB->getObjectName(*p)<<endl;
#endif
        pItem->appendSubItem(m_nIndex+1, m_uDB->getObjectName(*p), false, m_eRegion);
        if(cfg->cfg_showMpkgBigIcon) {
            pItem->setPixmap(m_nIndex, m_uDB->getObjectIconPixmap(*p, ICON_BIG));
            if( m_bIsSD )
                text2 = lng->tr("FT_MMCA1");
            else
                text2 = lng->tr("FT_PHONE");

            text2 += ctr("，")+m_uDB->getObjectVendor(*p)+ctr("，")+m_uDB->getObjectVersion(*p);
            pItem->appendSubItem(m_nIndex+1, text2, false, m_eRegion);
        } else {
            if( m_bIsSD ) {
                pItem->setPixmap(m_nIndex+2, QPixmap( m_strAppDir+"/img/sd_s.png") );
            } else {
                pItem->setPixmap(m_nIndex+2, QPixmap( m_strAppDir+"/img/phone_s.png") );
            }
            pItem->setPixmap(m_nIndex, m_uDB->getObjectIconPixmap(*p, ICON_SMALL) );
        }
        this->insertItem(pItem);
    }
}
#endif
/*
void appListBox::buildMpkgList()
{
 	if( m_uDB == NULL )
		return;

	QString uid = QString::null;
	QString name = QString::null;
	QString iconPath = QString::null;
	bool isSD =  mIsSD;


	QStringList grouplist;
	QStringList::ConstIterator p;

	//ZConfig mDBConfig(mConfigFile);
	mDBConfig->getGroupsKeyList(grouplist);

	for (p = grouplist.begin(); p != grouplist.end(); ++p )
	{
		uid = *p;
		#ifdef VE66
		if( (mDBConfig->readEntry(uid, "AppType", "")).toInt() != 3) continue;
		#else
		if( (mDBConfig->readEntry(uid, "AppType", "")).toInt() != 0) continue;
		#endif

		ZTaskItem* pItem = NULL;
		QString dir;

		name = mDBConfig->readEntry(uid, "Name", "");
		dir = mDBConfig->readEntry(uid, "Directory", "");
		iconPath = mDBConfig->readEntry(uid, "Icon", "");

		if(cfg->cfg_showMpkgBigIcon)
			pItem = new ZTaskItem ( this, QString ( "%I%M" ) );
		else
		{
			#ifdef SCREEN_V
			pItem = new ZTaskItem ( this, QString ("%I%M160%I") );
			#else
			pItem = new ZTaskItem(this, QString("%I%M240%I"));
			#endif
		}
		pItem->setType(ITEM_MPKG_OBJ);
		pItem->setGname(uid);
		pItem->setPath(isSD);

		QPixmap icon;
		if ( QFile::exists( iconPath ) )  {
			icon = QPixmap(iconPath);
		} else {
			if  ( QFile::exists( QString("%1/%2").arg(dir).arg(iconPath) ) )
				icon = QPixmap( QString("%1/%2").arg(dir).arg(iconPath) );
			else
				icon = iconReader.getIcon(iconPath);
		}
		//if(QFile::exists(iconPath))
		//	icon = QPixmap(iconPath);
		//else
		//	icon = iconReader.getIcon(iconPath);
		QImage img = icon.convertToImage();
		if(cfg->cfg_showMpkgBigIcon) 	img = img.smoothScale(ICON_BIG );
		else 	img = img.smoothScale(ICON_SMALL);
		icon.convertFromImage(img);
		pItem->setPixmap(0, icon);
		if(cfg->cfg_showMpkgBigIcon == false)
		{
			if(isSD == true) icon = QPixmap(mAppDir+"/img/sd_s.png");
			else icon = QPixmap(mAppDir+"/img/phone_s.png");

			pItem->setPixmap(2, icon);
		}
		QString text2 = QString::null;
		if(cfg->cfg_showMpkgBigIcon)
		{
			if (isSD == true) text2 = lng->tr("PROP_PATH")+lng->tr("FT_MMCA1");
			else text2 = lng->tr("PROP_PATH")+lng->tr("FT_PHONE");
		}
		pItem->appendSubItem(1, name, false, SETTINGITEM_REGION_A);
		if(cfg->cfg_showMpkgBigIcon) pItem->appendSubItem(1, text2, false, SETTINGITEM_REGION_B);
		this->insertItem(pItem);
	}
	//this->sort();
}*/
void appListBox::buildMenuList()
{
    if( m_bOpenMenuState )
        return;
    if( m_uDB == NULL)
        return;

#ifdef DEBUG
    cout<<"***** ***** buildMenuList() "<<endl;
#endif

    ZTaskItem* pItem = NULL;

    QStringList grouplist;
    QStringList::ConstIterator p;

    grouplist = m_uDB->getMenuGroupList();

    for (p = grouplist.begin(); p != grouplist.end(); ++p )
    {
        pItem = new ZTaskItem ( this, QString ( "%I%M" ) );
        pItem->setType(ITEM_MENU_OBJ);
        pItem->setGname(*p);
        pItem->appendSubItem(m_nIndex+1, m_uDB->getObjectName(*p), false, m_eRegion);
        if(cfg->cfg_showMgxBigIcon) {
            pItem->setPixmap(m_nIndex, m_uDB->getObjectIconPixmap(*p, ICON_BIG));
            pItem->appendSubItem(m_nIndex+1, QString("Menu Units"), false, m_eRegion);
        } else {
            pItem->setPixmap(m_nIndex, m_uDB->getObjectIconPixmap(*p, ICON_SMALL));
        }
        this->insertItem(pItem, 0);
    }
}
void appListBox::buildMgxList()
{
    if( m_uDB == NULL )
        return;
#ifdef DEBUG
    cout<<"***** ***** ***** ***** *****  buildMgxList() "<<endl;
#endif
    ZTaskItem* pItem = NULL;
    bool isSD = false;
    QString text2 = QString::null;
    QStringList grouplist;
    QStringList::ConstIterator p;

    if( m_bOpenMenuState ) {
        pItem = new ZTaskItem(this, QString("%I%M"));
        pItem->setType(ITEM_MENU_OBJ);
        pItem->setGname(m_strMenuId);
        pItem->setPixmap(0, m_iconReader.getIcon("gen_back_to_arrw_small.bmp"));
        pItem->appendSubItem(1, QString("Back"));
        this->insertItem(pItem, 0);//, true);

        grouplist = m_uDB->getSubObjectGroupList(m_strMenuId);
    } else {
        grouplist = m_uDB->getRootObjectList();
    }

    for (p = grouplist.begin(); p != grouplist.end(); ++p )
    {
        if(cfg->cfg_showMgxBigIcon)
        {
            if( m_eMode == NORMAL )  {
                pItem = new ZTaskItem ( this, QString ( "%I%M" ) );
            } else
            {
#ifdef SCREEN_V
                pItem = new ZTaskItem(this, QString("%C%I%M120%I40"));
#else
                pItem = new ZTaskItem(this, QString("%%C%I%M200%I40"));
#endif
            }
        }
        else
        {
            if( m_eMode == NORMAL )
            {
#ifdef SCREEN_V
                pItem = new ZTaskItem ( this, QString ("%I%M160%I") );
#else
                pItem = new ZTaskItem(this, QString("%I%M240%I"));
#endif
            } else
            {
#ifdef SCREEN_V
                pItem = new ZTaskItem(this, QString("%C%I%M120%I40"));
#else
                pItem = new ZTaskItem(this, QString("%%C%I%M200%I40"));
#endif
            }
        }
        isSD = m_uDB->isSDObject(*p);

        pItem->setType(ITEM_MGX_OBJ);
        pItem->setGname( *p );
        pItem->setPath( isSD );
        //pItem->appendSubItem(1, m_uDB->getObjectName(*p), true, SETTINGITEM_REGION_A);
        pItem->appendSubItem(m_nIndex+1, m_uDB->getObjectName(*p), false, m_eRegion);
        if(cfg->cfg_showMgxBigIcon)
        {
            pItem->setPixmap(m_nIndex, m_uDB->getObjectIconPixmap(*p, ICON_BIG) );

            if ( isSD )
                text2 = lng->tr("FT_MMCA1")+","+m_uDB->getObjectComment(*p);
            else
                text2 = lng->tr("FT_PHONE")+","+m_uDB->getObjectComment(*p);

            pItem->appendSubItem(m_nIndex+1, text2, false,  m_eRegion);
        } else
        {
            pItem->setPixmap(m_nIndex, m_uDB->getObjectIconPixmap(*p, ICON_SMALL) );

            if( isSD )
                pItem->setPixmap(m_nIndex+2, QPixmap(m_strAppDir+"/img/sd_s.png") );
            else
                pItem->setPixmap(m_nIndex+2, QPixmap(m_strAppDir+"/img/phone_s.png") );
        }
        this->insertItem(pItem);
    }
}
