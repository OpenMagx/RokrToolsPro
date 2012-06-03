
#include <ZConfig.h>
#include <ZFormContainer.h>

#include <qfile.h>
#include <qdir.h>
#include <qtimer.h>

#include "appAddDlg.h"
#include "lng.h"
#include "source.h"
#include "UtilsDBOperation.h"

#include <iostream>

extern Lng *lng;
extern UtilsDBOperation *mgxDB;
extern UtilsDBOperation *favDB;

appAddDlg :: appAddDlg(const QString &title, appAddDlgType type,  bool isMult) :ZPopup()
{
#ifdef DEBUG
    cout<<" =========== appAddDlg :: appAddDlg ==========="<<endl;
#endif

    m_bIsMult = isMult;
    m_eDlgType = type;

    appDir = QString::null;
    appExec = QString::null;
    appGname = QString::null;
    appName = QString::null;


    this->setTitle(title);

    ZFormContainer *form = new ZFormContainer(this);
    this->insertChild(form);

    m_zListBox = new ZListBox ( QString ( "%I%M" ), this, 0);
    form->addChild(m_zListBox);

    if( m_bIsMult ) {
        m_nIndex = 1;
        m_eRegion = SETTINGITEM_REGION_B;
        m_zListBox->setItemType("%C%I%M");
        m_strItemType = QString("%C%I%M");
    } else {
        m_nIndex = 0;
        m_eRegion = SETTINGITEM_REGION_A;
        m_zListBox->setItemType("%I%M");
        m_strItemType = QString("%I%M");
    }

    ZSoftKey *softKey = this->getSoftKey();
    softKey->setText(ZSoftKey::LEFT,  lng->tr("FT_SELECT"));
    softKey->setText(ZSoftKey::RIGHT, lng->tr("FT_CANCEL"));
    softKey->setClickedSlot(ZSoftKey::RIGHT, this, SLOT( slotCancel() ));
    softKey->setClickedSlot(ZSoftKey::LEFT, this, SLOT ( slotOK() ));

    if( !m_bIsMult ) {
        connect( m_zListBox, SIGNAL( selected(int) ), this, SLOT( obj_click(int) ) );
    }
    QTimer::singleShot(100, this, SLOT( initList() ) );
}
appAddDlg::~appAddDlg()
{
    m_lsAppList.setAutoDelete(true);
    m_lsAppList.clear();
    //m_lsAppList = NULL;
}

void appAddDlg::initList()
{
#ifdef DEBUG
    cout<<" =========== initList() ==========="<<endl;
#endif
    buildAppList();
//	showAppList();
    m_zListBox->setCurrentItem(0);
}

void appAddDlg::buildAppList()
{
#ifdef DEBUG
    cout<<" =========== buildAppList() ==========="<<endl;
#endif

    if( !m_lsAppList.isEmpty() ) {
        m_lsAppList.setAutoDelete(true);
        m_lsAppList.clear();
    }
    switch(m_eDlgType) {
    case DT_addFavApp :
        creatPHApp();
        creatSDApp();
        creatSYSApp();
        break;
    case DT_getAppInfo :
#ifdef VE66
        creatPHApp();
        creatSDApp();
#endif
        creatSYSApp();
        break;
    case DT_showFavMenuList :
    case DT_showMgxMenuList :
        creatMgxMenuList();
        break;
    default:
        break;
    }
}
/*
void appAddDlg::showAppList()
{
	appListbox->clear();
	append2_app_ui(appListbox,appList);
}
*/
void appAddDlg::creatMgxMenuList()
{
#ifdef DEBUG
    cout<<" =========== creatMgxMenuList() ==========="<<endl;
#endif
    UtilsDBOperation *m_uDB = NULL;

    if( m_eDlgType == DT_showFavMenuList ) {
        m_uDB = new UtilsDBOperation( QString(FAV_OBJ_PATH) );
    } else if( m_eDlgType == DT_showMgxMenuList ) {
        m_uDB = new UtilsDBOperation( QString(MGX_OBJ_PATH) );
    } else {
        return;
    }

    struct appObject *obj = NULL;
    QStringList grouplist;
    QStringList::ConstIterator p;

    grouplist = m_uDB->getMenuGroupList();
    for (p = grouplist.begin(); p != grouplist.end(); ++p )
    {
        obj = new appObject();
        obj->uid = *p;
        obj->name = m_uDB->getObjectName(*p);
        obj->type = m_uDB->getObjectType(*p);
        obj->icon = m_uDB->getObjectIconName(*p);
        m_lsAppList.append(obj);

        m_zTaskItem = new ZTaskItem ( m_zListBox, m_strItemType );
        m_zTaskItem->appendSubItem(m_nIndex+1, obj->name, false, m_eRegion);
        m_zTaskItem->setPixmap(m_nIndex,  m_uDB->getObjectIconPixmap(*p, 20, 20) );

        m_zListBox->insertItem(m_zTaskItem);
    }
    delete m_uDB;
    m_uDB = NULL;
}
void appAddDlg::creatPHApp()
{
#ifdef DEBUG
    cout<<" =========== creatPHApp() ==========="<<endl;
#endif
    UtilsDBOperation m_uDB( QString(PHONE_INSTALL_DB) );

    int index = -1;
    QStringList grouplist;
    QStringList::ConstIterator p;

    grouplist = m_uDB.getJavaGroupList();
    grouplist += m_uDB.getNativeGroupList();

    struct appObject *obj = NULL;
    for (p = grouplist.begin(); p != grouplist.end(); ++p )
    {
        obj = new appObject();
        obj->uid = *p;
        obj->name = m_uDB.getObjectName(*p);
        obj->type = m_uDB.getObjectType(*p);
        obj->path = m_uDB.getObjectDir(*p);
        obj->icon = m_uDB.getObjectIconName(*p);
        obj->exec = m_uDB.getObjectExec(*p);
        if( obj->path == "" || obj->name == "" || obj->exec == "" ) {
            delete obj;
            obj = NULL;
            continue;
        }
        m_lsAppList.append(obj);

        m_zTaskItem = new ZTaskItem ( m_zListBox, m_strItemType );
        m_zTaskItem->appendSubItem(m_nIndex+1, obj->name, false, m_eRegion);
        m_zTaskItem->setPixmap(m_nIndex,  m_uDB.getObjectIconPixmap(*p, 20, 20) );

        m_zListBox->insertItem(m_zTaskItem);
    }
}

void appAddDlg::creatSDApp()
{
#ifdef DEBUG
    cout<<" =========== :creatSDApp() =========="<<endl;
#endif
    UtilsDBOperation m_uDB( QString(SD_INSTALL_DB) );

    int index = -1;
    QStringList grouplist;
    QStringList::ConstIterator p;

    grouplist = m_uDB.getJavaGroupList();
    grouplist += m_uDB.getNativeGroupList();

    struct appObject *obj = NULL;

    for (p = grouplist.begin(); p != grouplist.end(); ++p )
    {
        obj = new appObject();
        obj->uid = *p;
        obj->name = m_uDB.getObjectName(*p);
        obj->type = m_uDB.getObjectType(*p);
        obj->path = m_uDB.getObjectDir(*p);
        obj->icon = m_uDB.getObjectIconName(*p);
        obj->exec = m_uDB.getObjectExec(*p);
        if( obj->path == "" || obj->name == "" || obj->exec == "" ) {
            delete obj;
            obj = NULL;
            continue;
        }
        m_lsAppList.append(obj);

        m_zTaskItem = new ZTaskItem ( m_zListBox, m_strItemType );
        m_zTaskItem->appendSubItem(m_nIndex+1, obj->name, false, m_eRegion);
        m_zTaskItem->setPixmap(m_nIndex,  m_uDB.getObjectIconPixmap(*p, 20, 20) );;

        m_zListBox->insertItem(m_zTaskItem);
    }
}

void appAddDlg::creatSYSApp()
{
#ifdef DEBUG
    cout<<" =========== creatSYSApp() =========="<<endl;
#endif

    int index = -1;
    QStringList uidList;
    QStringList::ConstIterator p;

    UtilsDBOperation m_uDB( QString(SYS_REGISTRY) );

    if( m_eDlgType == DT_addFavApp ) {
        QString menuListStr = QString::null;
        QStringList grouplist;
        ZConfig cfg( QString(MENU_TREE) );
        cfg.getGroupsKeyList(grouplist);
        for (p = grouplist.begin(); p != grouplist.end(); ++p ) {
            menuListStr = cfg.readEntry(*p, "Items", "");
            uidList += QStringList::split(";", menuListStr);
        }
    } else {
        uidList = m_uDB.getNativeGroupList();
    }

    QString name = QString::null;
    struct appObject *obj = NULL;

    for (p = uidList.begin(); p != uidList.end(); ++p )
    {
        obj = new appObject();
        obj->uid = *p;
        obj->name = tr(m_uDB.getObjectResourceIdName(*p));
        obj->type = m_uDB.getObjectType(*p);
        obj->path = m_uDB.getObjectDir(*p);
        obj->icon = m_uDB.getObjectIconName(*p);
        obj->exec = m_uDB.getObjectExec(*p);
        if( obj->path == "" || obj->name == "" || obj->exec == "" ) {
            delete obj;
            obj = NULL;
            continue;
        }
        m_lsAppList.append(obj);

        m_zTaskItem = new ZTaskItem ( m_zListBox, m_strItemType );
        m_zTaskItem->appendSubItem(m_nIndex+1, obj->name, false, m_eRegion);
        m_zTaskItem->setPixmap(m_nIndex,  m_uDB.getObjectIconPixmap(*p, 20, 20) );

        m_zListBox->insertItem(m_zTaskItem);
    }
}

void appAddDlg::slotOK()
{
#ifdef DEBUG
    cout<<" ==========  slotOK()=========="<<endl;
#endif
    struct appObject *obj = NULL;
    switch(m_eDlgType) {
    case DT_addFavApp :
    {
        //UtilsDBOperation m_uDB( QString(FAV_OBJ_PATH) );
        UtilsDBOperation *m_uDB = favDB;
        if(m_bIsMult) {
            //int count = m_zListBox->count();
            for (int i=0 ; i < m_zListBox->count(); i++) {
                if (m_zListBox->itemChecked(i)) {
                    obj = m_lsAppList.at(i);
                    if(obj == NULL) continue;
                    m_uDB->writeEntry(obj->uid, "Name" , obj->name);
                    m_uDB->writeEntry(obj->uid, "Directory", obj->path);
                    m_uDB->writeEntry(obj->uid, "Icon", obj->icon);
                    m_uDB->writeEntry(obj->uid, "ExecId", obj->exec);
                    m_uDB->writeEntry(obj->uid, "AppType", obj->type);
                }
            }
        } else
        {
            obj = m_lsAppList.at( m_zListBox->currentItem() );
            if(obj == NULL) return;
            m_uDB->writeEntry(obj->uid, "Name" , obj->name);
            m_uDB->writeEntry(obj->uid, "Directory", obj->path);
            m_uDB->writeEntry(obj->uid, "Icon", obj->icon);
            m_uDB->writeEntry(obj->uid, "ExecId", obj->exec);
            m_uDB->writeEntry(obj->uid, "AppType", obj->type);
        }
        m_uDB->flush();
    }
    break;
    case DT_getAppInfo:
    {
        obj = m_lsAppList.at( m_zListBox->currentItem() );
        if(obj == NULL) return;
        appDir = obj->path;
        appExec = obj->exec;
        appGname = obj->uid;
        appName = obj->name;
    }
    break;
    case DT_showFavMenuList:
    case DT_showMgxMenuList:
    {
        obj = m_lsAppList.at( m_zListBox->currentItem() );
        if(obj == NULL) return;
        appGname = obj->uid;
        appName = obj->name;
    }
    break;
    default :
        break;
    }
    isCancel = false;
    accept();
}

void appAddDlg::slotCancel()
{
    isCancel = true;
    reject();
}
void appAddDlg::obj_click(int index)
{
    int i = -1;
    i = index;
    if(i == -1 )
        return;
    struct appObject *obj = NULL;
    obj = m_lsAppList.at(i);
    if(obj == NULL)
        return;
    switch(m_eDlgType) {
    case DT_addFavApp :
    {
        UtilsDBOperation m_uDB( QString(FAV_OBJ_PATH) );

        obj = m_lsAppList.at( m_zListBox->currentItem() );
        if(obj == NULL) return;
        m_uDB.writeEntry(obj->uid, "Name" , obj->name);
        m_uDB.writeEntry(obj->uid, "Directory", obj->path);
        m_uDB.writeEntry(obj->uid, "Icon", obj->icon);
        m_uDB.writeEntry(obj->uid, "ExecId", obj->exec);
        m_uDB.writeEntry(obj->uid, "AppType", obj->type);
        m_uDB.flush();
    }
    break;
    case DT_getAppInfo:
    {
        obj = m_lsAppList.at( m_zListBox->currentItem() );
        if(obj == NULL) return;
        appDir = obj->path;
        appExec = obj->exec;
        appGname = obj->uid;
        appName = obj->name;
    }
    break;
    case DT_showFavMenuList:
    case DT_showMgxMenuList:
    {
        obj = m_lsAppList.at( m_zListBox->currentItem() );
        if(obj == NULL) return;
        appGname = obj->uid;
        appName = obj->name;
    }
    break;
    default :
        break;
    }
    isCancel = false;
    accept();
}



