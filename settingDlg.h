
#ifndef SETTINGDLG_H
#define SETTINGDLG_H

#include <ZApplication.h>
#include <ZComboBox.h>
#include <ZCheckBox.h>
#include <ZFormContainer.h>
#include <ZNavTabWidget.h>
#include <ZLineEdit.h>
#include <ZLabel.h>
#include <ZPressButton.h>
#include <ZNoticeDlg.h>
#include <ZListBox.h>
#include <ZListBoxItem.h>

#include <qstring.h>
#include <qobject.h>
#include <qpainter.h>

#include "myDef.h"
#include "BaseDlg.h"
#include "cfg.h"
#include "source.h"


class SettingDlg: public MyBaseDlg
{
    Q_OBJECT

public :

    SettingDlg();
    ~SettingDlg();
    bool isCancel;
    bool isFullScreenMode;

    //void paintEvent( QPaintEvent * pe);
protected:
    void buildTab0();
    void buildTab1();
    void buildTab2();
    void buildTab3();
    void buildTab4();
    void timerEvent(QTimerEvent *e);

    void getLngList();
    void setOcList();

private slots:
    void accept();
    void reject();
    void buttonClicked();
    void slotMkey(int index);

    //void drawAbout(QPainter & p);

private :
    ZNavTabWidget *tabWidget;
    ZFormContainer *form0;
    //ZListBox *listBox0;
    ZFormContainer *form1;
    ZFormContainer *form2;
    ZFormContainer *form3;
    ZFormContainer *form4;

    ZLabel *tab2Label;
    ZLabel *tab3Label;
    ZLabel *tab4Label;
    //ZLabel *tab3Label;
    //QVBoxLayout *form3;

    // Global
    ZCheckBox *refreshWhenTabChg;
    ZCheckBox *cKeyToKill;
    ZCheckBox *shortMKeyToOpen;
    ZCheckBox *fullScreenStatus;
    ZCheckBox *timeHeader;
    ZCheckBox *hideAfterRunApp;

    // task
    ZCheckBox *procFilter;
    ZCheckBox *showNativeIcon;
    ZCheckBox *showJavaIcon;
    ZCheckBox *showJavaName;
    ZCheckBox *sendReturnToIdle;
    ZCheckBox *showNativeMgxInfo;
#ifndef NEW_JAVA_LIST
    ZCheckBox *markItem;
#endif

    // app
    //bool cfg_showFavBigIcon;
    ZCheckBox *showMgxBigIcon;
    ZCheckBox *showMpkgBigIcon;
#ifndef NEW_JAVA_LIST
    ZCheckBox *runJavaInMult;
#endif

    //fm
    ZCheckBox *clearBufEveryMultModeInCut;
    ZCheckBox *clearBufEveryMultModeInCopy;
    ZCheckBox *showHideObj;
    ZCheckBox *showMimeType;
    ZCheckBox *showMultiWindow;

    ZComboBox *lngcb;
    ZComboBox *regcb;
    ZComboBox *occb;
    ZComboBox *mKeycb;

    //ZLineEdit *lineedit;

    ZPressButton *button;

    QString imei;
    QString imsi;
    QString regCode;

};

#endif // SETTINGDLG_H

