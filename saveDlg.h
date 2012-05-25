#ifndef SAVEDLG_H
#define SAVEDLG_H


#include <ZPopup.h>
#include <ZLineEdit.h>
#include <ZGroupBox.h>
#include <ZComboBox.h>
#include <ZSoftKey.h>

#include <qstring.h>
#include <qstringlist.h>

#include "myDef.h"

class saveDlg : public ZPopup
{
	Q_OBJECT
public:
	saveDlg(QString title = QString::null, QString name = QString::null, int type = 0 );
	saveDlg(QString title = QString::null, QStringList nameList = "", int type = 0); //0 save mgxes
																				// 1 just choose path	
	~saveDlg();
	int checkedIndex; // -1->reject, 0->phone, 1->sd card 2->curPath
	QString newName; 
	
private:
	#if defined(E8) || defined(EM30)  
	int oldMode;
	#endif
	int mDlgType;
	bool CardExist;
	bool m_curPath_flag;
	ZGroupBox *editGroupBox;
	ZLineEdit *lineEdit;
	ZGroupBox *pathGroupBox;
	ZComboBox *pathBox;

private slots:
	void ok();
	void cancel();
};

class compressDlg: public ZPopup
{
	Q_OBJECT
public:
	compressDlg(QString title = QString::null, QString name = QString::null );
	~compressDlg();
	int pathIndex; // -1->reject, 0->phone, 1->curPath, 2->sd card
	int typeIndex; // -1/0 ->7z, 1->tar, 2->tar.gz, 3->tar.bz2
	QString newName; 
	
private:
	#if defined(E8) || defined(EM30)  
	int oldMode;
	#endif

	bool CardExist;

	ZGroupBox *editGroupBox;
	ZLineEdit *lineEdit;
	ZGroupBox *pathGroupBox;
	ZComboBox *pathBox;
	
	ZGroupBox *typeGroupBox;
	ZComboBox *typeBox;

private slots:
	void ok();
	void cancel();
};
#endif
