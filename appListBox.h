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
        ALB_FAV		= 0,	//��ǰ��favtab
        ALB_MGX		= 1,	//��ǰ��mgxtab
        ALB_MPKG	= 2,	//��ǰmpkgtab
        ALB_JAVA	= 3,	//��ǰjavatab
        ALB_MENU			//��ǰ�ǲ˵�
    };

    appListBox(appListBoxType t = ALB_FAV, const QString& type = QString::null, QWidget* parent = 0, WFlags f = 0);
    ~appListBox();
    void setDB(QString file);

    void bulidAppList();
    void openMenu(const QString &menuId);
    QString getCurMenuId();
    void cdBack(); //�Ӳ˵�ģʽ����

    void selAll(); //ȫѡ
    void selNone(); //ȫ��ѡ

    QStringList getSelectedItemNameList(); //���ض�ѡģʽʱѡ�е�item������
    QStringList getItemNameList(); //�������е�item�����֣������Ƿ��Ƕ�ѡ

    QString getSelectedItemNameStr( const QString& sep = "||"); //��sep�ָ��item�����ֵ�QString
    QString getItemNameStr( const QString& sep = "||");

    QStringList getSelectedItemGNameList(); //���ض�ѡģʽʱѡ�е�item��gname
    QStringList getItemGNameList();

    QString getSelectedItemGNameStr( const QString& sep = "||");
    QString getItemGNameStr( const QString& sep = "||");

    appListBoxType getCurListType(); //�õ���ǰ��listbox���ĸ����͵�, fav , mgx, java
    ITEMMODE getMode(); //�õ�item���ͣ�ѡ��/��ѡ��

    //void setItemMode(ITEMMODE mode = NORMAL);
public slots:
    void setMode(ITEMMODE mode = NORMAL); //���ö�ѡģʽ

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

