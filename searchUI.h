

#ifndef _SEARCHUI_H_
#define _SEARCHUI_H_

#define QT_THREAD_SUPPORT

#include <ZPopup.h>
#include <ZMeter.h>
#include <ZSoftKey.h>
#include <ZLabel.h>
#include <ZListBox.h>

#include <qthread.h>
#include <qtimer.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qscrollview.h>

#include <stdio.h>
#include <iostream>

#include "myDef.h"

#if defined(E8) || defined(EM30)  || defined(VE66)
#include <ZMeterModule.h>
#endif

class searchThread : public QThread
{
public :
    searchThread(const QString &path, const QString &key, bool isSearchFile = false, int limitFolderDepth = 5);
    ~searchThread();

    bool isFinish();
    QString getCurPath();
    QStringList getResultList();
    void stopSearch();

protected :
    void searchFile( const QString &path);
    virtual void run();
    int getFolderDepth( const QString &path);
    bool fileSearched(const QString &file);
    bool isTextFile(const QString &file);

private :
    int m_nFolderDepth;
    int m_nCurFolderDepth;
    int m_nParentFolderDepthNum;
    bool m_bFinishState;
    bool m_bSearchStopState;
    bool m_bFileSearchState;
    QString m_strPath;
    QString m_strKey;
    QString m_strCurPath;
    QStringList m_lsResultList;
};


class searchDlg : public ZPopup
{
    Q_OBJECT

public :
    searchDlg( const QString &path, const QString &key , bool isSearchFile = false);
    ~searchDlg();
    QStringList getResultList();

private slots:
    void slotTimerEvent();
    void slotQuit();
    void slotHide();

private:
    searchThread *m_thread;
    QTimer *timer;
    ZMeterModule *m_zMeterModule;
    ZMeter *m_zMeter;

    int m_nStep;
    bool m_bFileSearchState;
    QString m_strPath;
    QString m_strKey;
    QStringList m_lsResultList;
};

class searchUI :public ZPopup
{
    Q_OBJECT

public:
    searchUI(const QStringList &itemList);
    ~searchUI();

    QString getClickedObjName(); //  /ezxlocal/download/test.sh >> return test.sh
    QString getClickedObjDirname(); //  /ezxlocal/download/test.sh >> return /ezxlocal/download
    //  /ezxlocal/download >> return /ezxlocal/download
    bool isClickedDir();

public slots:
    void obj_selected(int);
    void showItemList();

private:
    ZListBox *m_zListBox;
    ZSoftKey *softKey;

    bool m_bClickDirState;
    QString m_strClickedFile;
    QStringList m_lsItemList;
};

#endif
