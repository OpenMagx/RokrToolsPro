
#include <qdir.h>

#include "saveDlg.h"
#include "lng.h"
#include "source.h"

extern Lng *lng;

saveDlg :: saveDlg(QString title, QString name, int type) : ZPopup()
{
	#if defined(E8) || defined(EM30)  
	oldMode = getMorphMode();
	setMorphMode(3);
	#endif
	
	CardExist = false;
	checkedIndex = -1;	
	mDlgType = type;	
	QDir dir;
	if ( dir.exists( QString(SD_MPKG_PATH) ) )
		CardExist = true;	
		
	if( mDlgType == 0)
	{
		if( !dir.exists( QString(SD_MGX_FILE) ) )
			dir.mkdir( QString(SD_MGX_FILE) );
		if( !dir.exists( QString(PH_MGX_FILE) ) )	
			dir.mkdir( QString(PH_MGX_FILE) );	
	}
	
	this->setTitle(title);
	this->setInstructText(name);
	this->setTitleIcon("action_required_pop.bmp");

	if( mDlgType == 0)	
	{
		editGroupBox = new ZGroupBox(lng->tr("FT_SAVENAME"), this);

		lineEdit = new ZLineEdit(name, editGroupBox, 0, (ZSkinService::WidgetClsID)39);
		lineEdit->selectAll();
		lineEdit->resize(300, 40);

		this->insertChild(editGroupBox);
	}

	pathGroupBox = new ZGroupBox(lng->tr("FT_SAVEPATH"), this);

	pathBox = new ZComboBox(pathGroupBox);
	pathBox->insertItem(lng->tr("FT_PHONE"), 0);
	if (CardExist) pathBox->insertItem(lng->tr("FT_MMCA1"), 1);
	pathBox->resize(300, 40);

	this->insertChild(pathGroupBox);

	ZSoftKey *softKey = this->getSoftKey();
	softKey->setText(ZSoftKey::LEFT,  tr("TXT_RID_SOFTKEY_OK", "Ok"));
	softKey->setText(ZSoftKey::RIGHT, tr("TXT_RID_SOFTKEY_CANCEL", "Cancel"));
	softKey->setClickedSlot(ZSoftKey::LEFT, this, SLOT(ok()));
	softKey->setClickedSlot(ZSoftKey::RIGHT, this, SLOT(cancel()));
}
saveDlg::saveDlg(QString title, QStringList nameList, int type) : ZPopup()
{
	#if defined(E8) || defined(EM30) 
	oldMode=getMorphMode();
	setMorphMode(3);
	#endif
	CardExist = false;
	checkedIndex = -1;	
	mDlgType = type;
	QDir dir;
	if ( dir.exists( QString(SD_MPKG_PATH) ) )
		CardExist = true;	
		
	if( mDlgType == 0)
	{
		if( !dir.exists( QString(SD_MGX_FILE) ) )
			dir.mkdir( QString(SD_MGX_FILE) );
		if( !dir.exists( QString(PH_MGX_FILE) ) )	
			dir.mkdir( QString(PH_MGX_FILE) );	
	}
	QString oldName = QString("multi_mgx_installation");
	//enableAia(true);
	this->setTitle(title);
	this->setTitleIcon("action_required_pop.bmp");

	if( mDlgType == 0)	
	{
		editGroupBox = new ZGroupBox(lng->tr("FT_SAVENAME"), this);

		lineEdit = new ZLineEdit(oldName, editGroupBox, 0, (ZSkinService::WidgetClsID)39);
		lineEdit->selectAll();
		lineEdit->resize(300, 40);

		this->insertChild(editGroupBox);
	}
	QString NameListStr = QString::null;
	QStringList::ConstIterator p;   
	for (p=nameList.begin(); p!=nameList.end(); p++)
	{
		NameListStr += QString("%1\n").arg(*p);	
	}
	this->setInstructText(NameListStr);

	pathGroupBox = new ZGroupBox(lng->tr("FT_SAVEPATH"), this);

	pathBox = new ZComboBox(pathGroupBox);
	pathBox->insertItem(lng->tr("FT_PHONE"), 0);
	if (CardExist) pathBox->insertItem(lng->tr("FT_MMCA1"), 1);
	pathBox->resize(300, 40);

	this->insertChild(pathGroupBox);

	ZSoftKey *softKey = this->getSoftKey();
	softKey->setText(ZSoftKey::LEFT,  tr("TXT_RID_SOFTKEY_OK", "Ok"));
	softKey->setText(ZSoftKey::RIGHT, tr("TXT_RID_SOFTKEY_CANCEL", "Cancel"));
	softKey->setClickedSlot(ZSoftKey::LEFT, this, SLOT(ok()));
	softKey->setClickedSlot(ZSoftKey::RIGHT, this, SLOT(cancel()));
}
saveDlg::~saveDlg()
{
	#if defined(E8) || defined(EM30)  
	setMorphMode(oldMode);
	#endif
}
void saveDlg::ok()
{
	checkedIndex = pathBox->currentItem();
	if( mDlgType == 0)	
	{
		newName = lineEdit->text();
		newName = newName.simplifyWhiteSpace();
		if (newName == "") checkedIndex=-1;
	}
	else
	{
		newName == "";
	}
	reject();
}
void saveDlg::cancel()
{
	checkedIndex=-1;
	newName="";
	reject();
}

////////////////////////////////////////////////////

compressDlg::compressDlg(QString title, QString name ) : ZPopup()
{
	#if defined(E8) || defined(EM30)  
	oldMode = getMorphMode();
	setMorphMode(3);
	#endif
	
	CardExist = false;
	pathIndex = -1;
	typeIndex = -1;
	
	QDir dir;
	if ( dir.exists( QString(SD_MPKG_PATH) ) )
		CardExist = true;	
	
	this->setTitle(title);
	this->setInstructText(name);
	this->setTitleIcon("action_required_pop.bmp");

	editGroupBox = new ZGroupBox(lng->tr("FT_SAVENAME"), this);
	lineEdit = new ZLineEdit(name, editGroupBox, 0, (ZSkinService::WidgetClsID)39);
	lineEdit->selectAll();
	lineEdit->resize(300, 40);
	this->insertChild(editGroupBox);

	pathGroupBox = new ZGroupBox(lng->tr("FT_SAVEPATH"), this);
	pathBox = new ZComboBox(pathGroupBox);
	pathBox->insertItem(lng->tr("FT_PHONE"), 0);
	pathBox->insertItem(lng->tr("FT_CURPATH"), 1);
	if (CardExist) pathBox->insertItem(lng->tr("FT_MMCA1"), 2);
	pathBox->resize(300, 40);
	this->insertChild(pathGroupBox);

	typeGroupBox = new ZGroupBox(lng->tr("PROP_TYPE"), this);
	typeBox = new ZComboBox(typeGroupBox);
	typeBox->insertItem( QString("7z"), 0);
	typeBox->insertItem( QString("tar"), 1);
	typeBox->insertItem( QString("tar.gz"), 2);
	typeBox->insertItem( QString("tar.bz2"), 3);
	typeBox->resize(300, 40);
	this->insertChild(typeGroupBox);	
	
	ZSoftKey *softKey = this->getSoftKey();
	softKey->setText(ZSoftKey::LEFT,  tr("TXT_RID_SOFTKEY_OK", "Ok"));
	softKey->setText(ZSoftKey::RIGHT, tr("TXT_RID_SOFTKEY_CANCEL", "Cancel"));
	softKey->setClickedSlot(ZSoftKey::LEFT, this, SLOT(ok()));
	softKey->setClickedSlot(ZSoftKey::RIGHT, this, SLOT(cancel()));
}

compressDlg::~compressDlg()
{
	#if defined(E8) || defined(EM30)  
	setMorphMode(oldMode);
	#endif
}
void compressDlg::ok()
{
	pathIndex = pathBox->currentItem();	
	typeIndex = typeBox->currentItem();	
	newName = (lineEdit->text() ).simplifyWhiteSpace();
	accept();
}
void compressDlg::cancel()
{
	reject();
}










