#ifndef APPLISTBOX_H
#define APPLISTBOX_H

#include <ZListBox.h>
#include <RES_ICON_Reader.h>
#include <qstring.h>
#ifdef DEBUG
#include <iostream>
#endif

#include "ZTaskItem.h"
#include "myDef.h"
#include "source.h"
#include "UtilsDBOperation.h"

class appListBox : public ZListBox
{
    Q_OBJECT

public :
    enum appListBoxType {
        ALB_FAV		= 0,	//当前是favtab
        ALB_MGX		= 1,	//当前是mgxtab
        ALB_MPKG	= 2,	//当前mpkgtab
        ALB_JAVA	= 3,	//当前javatab
        ALB_MENU			//当前是菜单
    };

    appListBox(appListBoxType t = ALB_FAV, const QString& type = QString::null, QWidget* parent = 0, WFlags f = 0);
    ~appListBox();
    void setDB(QString file);

    void bulidAppList();
    void openMenu(const QString &menuId);
    QString getCurMenuId();
    void cdBack(); //从菜单模式返回

    void selAll(); //全选
    void selNone(); //全不选

    QStringList getSelectedItemNameList(); //返回多选模式时选中的item的名字
    QStringList getItemNameList(); //返回所有的item的名字，不管是否是多选

    QString getSelectedItemNameStr( const QString& sep = "||"); //以sep分割的item的名字的QString
    QString getItemNameStr( const QString& sep = "||");

    QStringList getSelectedItemGNameList(); //返回多选模式时选中的item的gname
    QStringList getItemGNameList();

    QString getSelectedItemGNameStr( const QString& sep = "||");
    QString getItemGNameStr( const QString& sep = "||");

    appListBoxType getCurListType(); //得到当前的listbox是哪个类型的, fav , mgx, java
    ITEMMODE getMode(); //得到item类型，选择/非选择

    //void setItemMode(ITEMMODE mode = NORMAL);
public slots:
    void setMode(ITEMMODE mode = NORMAL); //设置多选模式

signals:
    void itemHighlighted(int index);
    void itemSelected(int index);

private slots:
    void slotHighlighted(int index);
    void slotSelected(int index);

private:
    void buildFavList();
#ifdef TAB3_JAVA
    void buildJavaList();
#endif
    //void buildMpkgList();
    void buildMenuList();
    void buildMgxList();
    //void ChkMgxList();

private:
    int m_nIndex;

    RES_ICON_Reader m_iconReader;
    appListBoxType m_eType;
    ITEMMODE m_eMode;

    SettingItemRegion m_eRegion;

    QString m_strMgxMenuListStr;
    QString m_strMenuId;
    QString m_strAppDir;
    QString m_strConfigFile;
    QString m_strExistsMgxListStr;

    bool m_bIsSD;
    bool m_bOpenMenuState;

    UtilsDBOperation *m_uDB;

    //ZConfig *m_zDBConfig;
    //ZConfig *mMenuConfig;
};

#endif //APPLISTBOX_H

