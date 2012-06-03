#ifndef APPADDDLG_H
#define APPADDDLG_H

#include <ZPopup.h>
#include <ZSoftKey.h>
#include <ZListBox.h>

#include <RES_ICON_Reader.h>

#include "source.h"
#include "myDef.h"
#include "UtilsDBOperation.h"
#include "ZTaskItem.h"

class appAddDlg : public ZPopup
{
    Q_OBJECT

public:
    enum appAddDlgType {
        DT_addFavApp			= 0,  	//��ӿ�ݷ�ʽ��favtab
        DT_getAppInfo			= 1,	//��ʾһ�������б��õ�ѡ�еĳ�����Ϣ������fmtab�Ĵ򿪷�ʽ����
        DT_showFavMenuList		= 2,	//��ʾfav����mgx��ǩ�Ĳ˵��б����ڹ���˵�����Ϣ
        DT_showMgxMenuList,
    };
    appAddDlg(const QString &title = QString::null, appAddDlgType type = DT_addFavApp, bool isMult = false);  //0 for app add in the fav tab
    //1 for get app property(show a menu and choose the app item)
    // 2 for manager menu in mgx
    ~appAddDlg();//{};

    bool isCancel;
    QString appDir;
    QString appExec;
    QString appGname;
    QString appName;

public:
    struct appObject
    {
        QString uid;
        QString name;
        QString type; //1 -> java, 0->native
        QString path;
        QString icon;
        QString exec;
    };
private:
    void buildAppList();
    void showAppList();
    void creatPHApp();
    void creatSDApp();
    void creatMgxMenuList();
    void creatSYSApp();

public slots:
    void initList();

private slots:
    void slotOK();
    void slotCancel();
    void obj_click(int index);

private :
    int m_nIndex;
    SettingItemRegion m_eRegion;
    QString m_strItemType;

    bool m_bIsMult;
    appAddDlgType m_eDlgType;
    QList<struct appObject> m_lsAppList;

    ZListBox *m_zListBox;
    ZTaskItem *m_zTaskItem;
};



#endif
