#ifndef UTILSDBOPERATION_H
#define UTILSDBOPERATION_H

#include <ZConfig.h>
#include <RES_ICON_Reader.h>

#include <qstringlist.h>
#include <qpixmap.h>
#include <qfile.h>
#include <qdir.h>

#include "source.h"
#include "myDef.h"

class UtilsDBOperation
{
public :	
	UtilsDBOperation( const QString &file );
	~UtilsDBOperation();
	
	QStringList getRootObjectList();
	QString getRootObjectStr();
	QStringList getAllGroupList();
	QStringList getMenuGroupList();
	QStringList getMgxGroupList();
	QStringList getJavaGroupList();
	QStringList getNativeGroupList();
	
	QStringList getSubObjectGroupList( const QString& gnameParent);
	QString getSubObjectGroupStr( const QString& gnameParent);
		
	QString getObjectName( const QString& gname);
	QString getObjectResourceIdName( const QString& gname);
	QString getObjectDir( const QString& gname);
	QString getObjectExec( const QString& gname);
	QString getObjectIconName( const QString& gname);
	QString getObjectVendor( const QString& gname);
	QString getObjectVersion( const QString& gname);
	QString getObjectType( const QString& gname);
	QString getObjectComment( const QString& gname);
	bool isSDObject( const QString& gname);
	QPixmap getObjectIconPixmap( const QString& gname, int w = 40, int h = 40);
	
	QString readEntry( const QString& gname, const QString& key, const QString& deflt = QString::null );
	//QString readBoolEntry( const QString& gname, const QString& key, bool deflt = false );
	bool writeEntry( const QString& gname, const QString& key, const QString& value );
	bool writeEntry( const QString& gname, const QString& key, bool value = false );
	

	bool add2Menu( const QString& gnameObject, const QString& gnameMenu);
	bool add2Menu( const QStringList& gnameObjectList, const QString& gnameMenu);
	bool orderSubItemList( const QStringList& gnameObjectList,  const QString& gnameMenu);
	bool move2Root( const QString& gnameObject, const QString& gnameMenu);
	bool move2Root( const QStringList& gnameObjectList, const QString& gnameMenu);
	//bool deleteMenu( const QString& gnameMenu );
//	bool move2Root( const QString& gnameObject );
//	bool move2Root( const QStringList& gnameObjectList );
	
	bool clearGroup( const QString& gname);
	bool addMenu( const QString& menuName, const QString& iconName = "menuIcon.png");

	bool flush();
	
	void ChkMgxList();
	
protected :
	void initObjectList();

private :
	ZConfig *m_zConfig;
	
	bool m_bFlushed;
	
	QString m_sAppDir;
	QString m_strExistsMgxListStr;
	//QString m_strRootAppIds;
	
	QStringList m_lsMenuGroupList;
	QStringList m_lsMgxGroupList;
	QStringList m_lsJavaGroupList;
	QStringList m_lsNativeGroupList;
	
	QValueListConstIterator<QString> m_it;
	
	RES_ICON_Reader m_iconReader;

};
#endif 
