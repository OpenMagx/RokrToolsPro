
#include <stdio.h>
#include <stdlib.h>

#include "UtilsDBOperation.h"

UtilsDBOperation::UtilsDBOperation( const QString &file )
{
	m_bFlushed = false;
	m_sAppDir = getAppDir();
	
	QString f = file;
	
	//cout<<" ############## "<<file<<endl;
	
	m_zConfig = new ZConfig(f);
	
	//printf(" ####################### %p\n", m_zConfig);
	
	initObjectList();
}
UtilsDBOperation::~UtilsDBOperation()
{
	if( !m_bFlushed ) {
		if( m_zConfig != NULL ) {
			m_zConfig->flush();
			delete m_zConfig;
			m_zConfig = NULL;	
		}
	}	
}
QStringList UtilsDBOperation::getRootObjectList()
{
	//QString itmeStr = m_zConfig->readEntry( ROKR_ROOT, "Items", "" );
	return QStringList::split(";", getRootObjectStr());
}
QString UtilsDBOperation::getRootObjectStr()
{
	return m_zConfig->readEntry( QString(ROKR_ROOT), "Items", "" );
}
QStringList UtilsDBOperation::getAllGroupList()
{
	return m_lsMenuGroupList+m_lsMgxGroupList+m_lsJavaGroupList+m_lsNativeGroupList;
}
QStringList UtilsDBOperation::getMenuGroupList()
{
	return m_lsMenuGroupList;
}
QStringList UtilsDBOperation::getMgxGroupList()
{
	return m_lsMgxGroupList;
}
QStringList UtilsDBOperation::getJavaGroupList()
{
	return m_lsJavaGroupList;
}
QStringList UtilsDBOperation::getNativeGroupList()
{
	return m_lsNativeGroupList;
}	
QStringList UtilsDBOperation::getSubObjectGroupList( const QString& gnameParent)
{
	//QString itmeStr = m_zConfig->readEntry( gnameParent, "Items", "" );
	return QStringList::split(";", getSubObjectGroupStr(gnameParent) );
}
QString UtilsDBOperation::getSubObjectGroupStr( const QString& gnameParent)
{
	return m_zConfig->readEntry( gnameParent, "Items", "" );
}
QString UtilsDBOperation::getObjectName( const QString& gname)
{
	return m_zConfig->readEntry( gname, "Name", "" );
}
QString UtilsDBOperation::getObjectResourceIdName( const QString& gname)
{
	return QStringList::split(";", m_zConfig->readEntry( gname, "NameResourceId", "" ) )[0];
}
QString UtilsDBOperation::getObjectDir( const QString& gname)
{
	return m_zConfig->readEntry( gname, "Directory", "" );
}
QString UtilsDBOperation::getObjectExec( const QString& gname)
{
	QString exec = QString::null;
	exec = m_zConfig->readEntry( gname, "ExecId", "" );
	if( exec == "" ) {
		exec = m_zConfig->readEntry( gname, "Exec", "" );
		if( exec == "" ) {
			exec = m_zConfig->readEntry( gname, "JavaId", "" );
		}
	}
	return exec;
}
QString UtilsDBOperation::getObjectIconName( const QString& gname)
{
	QString icon = m_zConfig->readEntry( gname, "Icon", "" );
	if( icon == "" ) {
		icon = m_zConfig->readEntry(gname, "BigIcon", "");
		if( icon == "" ) {
			icon = m_zConfig->readEntry(gname, "AniIcon", "");
			if( icon == "" ) {
				icon = m_zConfig->readEntry(gname, "SVGIcon", "");	 
			} 
		}
	}
	return icon;
	//return m_zConfig->readEntry( gname, "Icon", "" );
}
QString UtilsDBOperation::getObjectVendor( const QString& gname)
{
	return m_zConfig->readEntry( gname, "Vendor", "" );
}
QString UtilsDBOperation::getObjectVersion( const QString& gname)
{
	return m_zConfig->readEntry( gname, "Version", "" );
}
QString UtilsDBOperation::getObjectType( const QString& gname)
{
	QString type = m_zConfig->readEntry( gname, "AppType", "" );
	if( type == "" ) {
		type = m_zConfig->readEntry( gname, "ExecId", "" );
		if( type == "" ) {
			type = m_zConfig->readEntry( gname, "Exec", "" );
			if( type == "" ) {
				type = m_zConfig->readEntry( gname, "JavaId", "" );
				if( type != "" ) {
					type = "1";
				} else {
					type = "";
				}
			} else {
				type = "0";
			}
		} else {
			type = "0";
		}
	} 
	return type;
}
QString UtilsDBOperation::getObjectComment( const QString& gname)
{
	return m_zConfig->readEntry( gname, "Comment", "" );
}
bool UtilsDBOperation::isSDObject( const QString& gname)
{
	return m_zConfig->readBoolEntry( gname, "Path", false );
}
QPixmap UtilsDBOperation::getObjectIconPixmap( const QString& gname, int w, int h)
{
	QPixmap pm;
	QString iconPath = m_sAppDir+"/img/"+getObjectIconName(gname);
	
/*	if( !QFile::exists(iconPath) ) 
		iconPath = getObjectIconName(gname);
	if( !QFile::exists(iconPath) ) 
		iconPath = getObjectIconName(gname);
	if( !QFile::exists(iconPath) ) 
		iconPath = getObjectDir(gname)+"/"+getObjectIconName(gname);
	if( !QFile::exists(iconPath) ) 
		iconPath = getObjectDir(gname)+getObjectIconName(gname);
	if( !QFile::exists(iconPath) ) 	
		iconPath = m_zConfig->readEntry(gname, "BigIcon", "");
	if( !QFile::exists(iconPath) ) 	
		iconPath = m_zConfig->readEntry(gname, "AniIcon", "");
	if( !QFile::exists(iconPath) ) 	
		iconPath = m_zConfig->readEntry(gname, "SVGIcon", "");	
	//if( !QFile::exists(iconPath) ) 	
	//	iconPath = 
	*/
	if( QFile::exists(iconPath) ) {
		pm = QPixmap(iconPath);
	} else {
		iconPath = getObjectIconName(gname);
		if( QFile::exists(iconPath) ) {
			pm = QPixmap(iconPath);		
		} else {
			iconPath = getObjectDir(gname)+"/"+getObjectIconName(gname);
			if( QFile::exists(iconPath) ) {
				pm = QPixmap(iconPath);		
			} else {
				iconPath = getObjectDir(gname)+getObjectIconName(gname);
				if( QFile::exists(iconPath) ) {
					pm = QPixmap(iconPath);		
				} else {
					iconPath = m_zConfig->readEntry(gname, "BigIcon", "");
					if( QFile::exists(iconPath) ) {
						pm = QPixmap(iconPath);	
					} else {	
						iconPath = m_zConfig->readEntry(gname, "AniIcon", "");
						if( QFile::exists(iconPath) ) {
							pm = QPixmap(iconPath);	
						} else {
							iconPath = m_zConfig->readEntry(gname, "SVGIcon", "");	
							if( QFile::exists(iconPath) ) {
								pm = QPixmap(iconPath);	
							} else {
								pm = m_iconReader.getIcon( getObjectIconName(gname) );
							}
						}		
					}	
				}
			}			
		}	
	}	
	QImage img = pm.convertToImage();
	img = img.smoothScale(w, h);
	pm.convertFromImage(img);
	return pm;
}
QString UtilsDBOperation::readEntry( const QString& gname, const QString& key, const QString& deflt )
{
	return m_zConfig->readEntry( gname, key, deflt );
}
bool UtilsDBOperation::writeEntry( const QString& gname, const QString& key, const QString& value)
{
	return m_zConfig->writeEntry( gname, key, value);
}
bool UtilsDBOperation::writeEntry( const QString& gname, const QString& key, bool value)
{
	return m_zConfig->writeEntry( gname, key, value);
}
bool UtilsDBOperation::add2Menu( const QString& gnameObject, const QString& gnameMenu)
{
	QString menuStr = getSubObjectGroupStr( gnameMenu );
	if( !menuStr.endsWith(";" ) ) { menuStr += ";"; }
	menuStr = menuStr+gnameObject+";";
	m_zConfig->writeEntry( gnameMenu, "Items", menuStr );
	
	QString rootStr = getRootObjectStr();
	if( !rootStr.endsWith(";" ) ) { rootStr += ";"; }
	rootStr = rootStr.replace( gnameObject+";", "");
	m_zConfig->writeEntry( QString(ROKR_ROOT), "Items", rootStr );
	
	return this->flush();	
}
bool UtilsDBOperation::add2Menu( const QStringList& gnameObjectList, const QString& gnameMenu)
{
	QString menuStr = getSubObjectGroupStr( gnameMenu );
	if( !menuStr.endsWith(";" ) ) { menuStr += ";"; }
	QString rootStr = getRootObjectStr();
	if( !rootStr.endsWith(";" ) ) { rootStr += ";"; }
	
	for ( m_it = gnameObjectList.begin(); m_it != gnameObjectList.end(); ++m_it ) {	
		menuStr = menuStr+*m_it+";";	
		rootStr = rootStr.replace( *m_it+";", "");
	}
	m_zConfig->writeEntry( gnameMenu, "Items", menuStr );	
	m_zConfig->writeEntry( QString(ROKR_ROOT), "Items", rootStr );
	
	return this->flush();
}
bool UtilsDBOperation::orderSubItemList( const QStringList& gnameObjectList,  const QString& gnameMenu)
{
	QString str = QString::null;	
	for ( m_it = gnameObjectList.begin(); m_it != gnameObjectList.end(); ++m_it ) {	
		str = str+*m_it+";";
	}
	m_zConfig->writeEntry( gnameMenu, "Items", str );
	return this->flush();
}

bool UtilsDBOperation::move2Root( const QString& gnameObject, const QString& gnameMenu)
{
	if( gnameMenu == QString(ROKR_ROOT) ) return false; 
	QString menuStr = getSubObjectGroupStr( gnameMenu );
	if( !menuStr.endsWith(";" ) ) { menuStr += ";"; }
	menuStr = menuStr.replace( gnameObject+";", "");
	m_zConfig->writeEntry( gnameMenu, "Items", menuStr );
	
	QString rootStr = getRootObjectStr();
	if( !rootStr.endsWith(";" ) ) { rootStr += ";"; }
	rootStr = rootStr+gnameObject+";";
	m_zConfig->writeEntry( QString(ROKR_ROOT), "Items", rootStr );
	
	return this->flush();
}
bool UtilsDBOperation::move2Root( const QStringList& gnameObjectList, const QString& gnameMenu)
{
	if( gnameMenu == QString(ROKR_ROOT) ) return false;
	QString menuStr = getSubObjectGroupStr( gnameMenu );
	if( !menuStr.endsWith(";" ) ) { menuStr += ";"; }
	QString rootStr = getRootObjectStr();
	if( !rootStr.endsWith(";" ) ) { rootStr += ";"; }
		
	for ( m_it = gnameObjectList.begin(); m_it != gnameObjectList.end(); ++m_it ) {	
		menuStr = menuStr.replace( *m_it+";", ""); 
		rootStr = rootStr+*m_it+";";
	}
	m_zConfig->writeEntry( gnameMenu, "Items", menuStr );	
	m_zConfig->writeEntry( QString(ROKR_ROOT), "Items", rootStr );
	
	return this->flush();
}
bool UtilsDBOperation::clearGroup( const QString& gname )
{
	#ifdef DEBUG
	cout<<" ==== ====== clearGroup "<<endl;
	#endif	
	QString rootStr = getRootObjectStr();
	if( !rootStr.endsWith(";" ) ) { rootStr += ";"; }
	
	QString subObjStr = getSubObjectGroupStr(gname);
	
	#ifdef DEBUG
	cout<<" ################## rootStr "<<rootStr<<endl;
	cout<<" ################## subObjStr "<<subObjStr<<endl;
	#endif		
	if( subObjStr != "" ) {
		if( subObjStr.startsWith(";") ) {
			subObjStr = subObjStr.right(subObjStr.length()-1);
		}
		rootStr = rootStr+subObjStr+";";
	} else {
		rootStr = rootStr.replace( gname+";", ""); 
	}
	#ifdef DEBUG
	cout<<" ################## rootStr "<<rootStr<<endl;
	#endif	
	m_zConfig->clearGroup( gname );	
	m_zConfig->writeEntry( QString(ROKR_ROOT), "Items", rootStr );	
	
	return this->flush();
}
bool UtilsDBOperation::addMenu( const QString& menuName, const QString& iconName )
{
	#ifdef DEBUG
	cout<<" ==== ====== add menu "<<endl;
	#endif	
	QString gname = getGname();
	#ifdef DEBUG
	cout<<" ==== ====== add menu start "<<endl;
	#endif		
	
	//printf(" ####################### %p\n", m_zConfig);
	
	m_zConfig->writeEntry(gname, "Name", menuName );
	cout<<" ==== ====== add menu aaaa "<<endl;
	m_zConfig->writeEntry(gname, "AppType", 50 );
	cout<<" ==== ====== add menu bbbb "<<endl;
	m_zConfig->writeEntry(gname, "Icon",  iconName );
	cout<<" ==== ====== add menu cccc "<<endl;
	#ifdef DEBUG
	cout<<" ==== ====== add menu after "<<endl;
	#endif		
	return this->flush();
	
}
bool UtilsDBOperation::flush()
{
	#ifdef DEBUG
	cout<<" ==== ====== flush() "<<endl;
	#endif		
	//bool state = false;
	bool state = m_zConfig->flush();
	initObjectList(); 
	m_bFlushed = true;
	return state;
}
void UtilsDBOperation::ChkMgxList()
{
//	return;
	
	m_strExistsMgxListStr = QString::null;	
	QString appDir = QString::null;
	QString tmpAppIds = QString::null;
	bool mark = false;
		
	QDir dir;
	for ( m_it = m_lsMgxGroupList.begin(); m_it != m_lsMgxGroupList.end(); ++m_it ) 
	{	
		#ifdef DEBUG
		cout<<"====== chk exists app list "<<*m_it<<endl;
		#endif	
		appDir = getObjectDir( *m_it );
		if( dir.exists( appDir ) ) {
			m_strExistsMgxListStr += appDir;
		} else {
			m_zConfig->clearGroup( *m_it );		
			mark = true;
			//m_zConfig->flush();
		}	
	}	
	
	/// make mgx db 
	QStringList appList;
 // cfg file name => must be full path 
	char linestr[512]; // for each line
	int count = 0; // count for getGname step
	int cfgMark = 0;
	QString fileName = QString::null;
	QString name = QString::null;
	QString path = QString::null;
	QString icon = QString::null;
	QString exec = QString::null;
	QString ver = QString::null;
	QString author = QString::null;
	QString comment = QString::null;	
	QString uid = QString::null;
	bool isSD;
	
	dir.setPath(PHONE_MPKG_PATH);	
	if ( dir.exists() )
	{	
		dir.setFilter(QDir::Dirs);	        
		appList = dir.entryList();
		if( !appList.isEmpty() ) {
			isSD = false;
			mark = true;
			for ( m_it = appList.begin(); m_it != appList.end(); ++m_it ) 
			{		
				cfgMark = 0;	
				
				path = QString("%1/%2").arg(PHONE_MPKG_PATH).arg(*m_it); 
				if( m_strExistsMgxListStr.find(path) > -1 ) {
					#ifdef DEBUG
					std::cout<<"cur mgx exists "<<endl;
					#endif					
					continue;			
				}
				
				fileName = QString("%1/%2/%3.cfg").arg(PHONE_MPKG_PATH).arg(*m_it).arg(*m_it);
				QFile cfgFile(fileName);
				if ( !cfgFile.exists() )	continue;
				if (cfgFile.open(IO_ReadOnly)) // cfg file exists
				{ 
					#ifdef DEBUG
					std::cout<<"cur mgx cfg file path is "<<fileName<<std::endl;
					#endif	
					while (cfgFile.readLine(linestr, 512) != -1) 
					{
						linestr[strlen(linestr)-1]='\0';									
						if (strstr(linestr, "Name = ")==linestr) {
							cfgMark++;
							name = QString(linestr+7);
						} 
						else if (strstr(linestr, "Exec = ")==linestr) {
							cfgMark++;
							exec = QString(linestr+7);
						} 
						else if (strstr(linestr, "Icon = ")==linestr) {
							cfgMark++;
							icon = QString(linestr+7);
						} 
						else if (strstr(linestr, "Author = ")==linestr) 
							author = QString(linestr+9);
						else if (strstr(linestr, "Version = ")==linestr) 
							ver = QString(linestr+10);
						else if (strstr(linestr, "Comment = ")==linestr)
							comment = QString(linestr+10);	
							
						uid = getGname(count+255);					
					}					
					//if (cfgMark==3) 
					if( name!="" && exec!="" && icon!="" )
					{					
						tmpAppIds = tmpAppIds+uid+";";
						m_zConfig->writeEntry(uid, "Name" , ctr(name));
						m_zConfig->writeEntry(uid, "Directory", path);
						m_zConfig->writeEntry(uid, "Icon", icon);
						m_zConfig->writeEntry(uid, "ExecId", exec);
						m_zConfig->writeEntry(uid, "Version", ctr(ver));
						m_zConfig->writeEntry(uid, "Vendor", ctr(author));	
						m_zConfig->writeEntry(uid, "Comment", ctr(comment));
						m_zConfig->writeEntry(uid, "Path", isSD);
						m_zConfig->writeEntry(uid, "AppType", "100");		
						//m_zConfig->writeEntry(uid, "Menu", "");
						//m_zConfig->flush();
					}
					count++;
					cfgFile.close();
				}			
			}
		}		
	}
	if( !appList.isEmpty() ) {appList.clear(); }
    dir.setPath(SD_MPKG_PATH);
	if ( dir.exists() )
	{	
		dir.setFilter(QDir::Dirs);	        
		appList = dir.entryList();
		if( !appList.isEmpty() ) {
			isSD = true;
			mark = true;
			//appList.sort();
			for (m_it = appList.begin(); m_it != appList.end(); ++m_it ) 
			{					   
				cfgMark = 0;
		
				path = QString("%1/%2").arg(SD_MPKG_PATH).arg(*m_it);
				if(m_strExistsMgxListStr.find(path) > -1 ) {
					#ifdef DEBUG
					std::cout<<"cur mgx exists "<<endl;
					#endif				
					continue;			
				}		
				
				fileName = QString("%1/%2/%3.cfg").arg(SD_MPKG_PATH).arg(*m_it).arg(*m_it);

				QFile cfgFile(fileName);
				if ( !cfgFile.exists() )	continue;
				if (cfgFile.open(IO_ReadOnly)) // cfg file exists
				{ 
					#ifdef DEBUG
					std::cout<<"cur mgx cfg file path is "<<fileName<<std::endl;
					#endif					
					while (cfgFile.readLine(linestr, 512) != -1) 
					{
						linestr[strlen(linestr)-1]='\0';									
						if (strstr(linestr, "Name = ")==linestr) {
							cfgMark++;
							name = QString(linestr+7);
						} 
						else if (strstr(linestr, "Exec = ")==linestr) {
							cfgMark++;
							exec = QString(linestr+7);
						} 
						else if (strstr(linestr, "Icon = ")==linestr) {
							cfgMark++;
							icon = QString(linestr+7);
						} 
						else if (strstr(linestr, "Author = ")==linestr) 
							author = QString(linestr+9);
						else if (strstr(linestr, "Version = ")==linestr) 
							ver = QString(linestr+10);
						else if (strstr(linestr, "Comment = ")==linestr)
							comment = QString(linestr+10);	
							
						uid = getGname(count+255);
					}					
					//if (cfgMark==3) 
					if( name!="" && exec!="" && icon!="" )
					{
						tmpAppIds = tmpAppIds+uid+";";
						m_zConfig->writeEntry(uid, "Name" , ctr(name));
						m_zConfig->writeEntry(uid, "Directory", path);
						m_zConfig->writeEntry(uid, "Icon", icon);
						m_zConfig->writeEntry(uid, "ExecId", exec);
						m_zConfig->writeEntry(uid, "Version", ctr(ver));
						m_zConfig->writeEntry(uid, "Vendor", ctr(author));	
						m_zConfig->writeEntry(uid, "Comment", ctr(comment));
						m_zConfig->writeEntry(uid, "Path", isSD);
						m_zConfig->writeEntry(uid, "AppType", "100");		
						//m_zConfig->writeEntry(uid, "Menu", "");	
						//m_zConfig->flush();
					}
					count++;
					cfgFile.close();
				}
			}	
		}	
	}	
	//m_zConfig->flush();
	if( mark ) {
		#ifdef DEBUG
		cout<<"====== last flash  mgx cfg file path is "<<endl;
		#endif	
		tmpAppIds = getRootObjectStr()+tmpAppIds;
		m_zConfig->writeEntry( QString(ROKR_ROOT), "Items", tmpAppIds);		
		m_zConfig->flush();
		initObjectList();
	}	
}
void UtilsDBOperation::initObjectList()
{
	#ifdef DEBUG
	cout<<"====== initObjectList() "<<endl;
	#endif	
	if ( !m_lsMenuGroupList.isEmpty() ) 	{ m_lsMenuGroupList.clear(); }
	if ( !m_lsMgxGroupList.isEmpty() )  	{ m_lsMgxGroupList.clear(); }
	if ( !m_lsJavaGroupList.isEmpty() )		{ m_lsJavaGroupList.clear();}
	if ( !m_lsNativeGroupList.isEmpty() ) 	{ m_lsNativeGroupList.clear(); }
	
	#define MENU "50"
	#define MGX "100"
	#define JAVA "1"
	//#define JAVA2 "3"
	#define NATIVE "0"

	QString type = QString::null;
	QStringList tmpList;
	
	//printf(" ####################### %p\n\n", m_zConfig);
	
	m_zConfig->getGroupsKeyList(tmpList);
	
		//printf(" ####################### %p\n", m_zConfig);
		
	if( tmpList.isEmpty() ) { return;}
	for ( m_it = tmpList.begin(); m_it != tmpList.end(); ++m_it ) 
	{	
		#ifdef DEBUG
		cout<<"====== chk "<<*m_it<<endl;
		#endif
		type = m_zConfig->readEntry( *m_it, "AppType", "");
		if( type != "" ) {
			if(  type  == MENU ) {	
				m_lsMenuGroupList += *m_it;
			} else
			if( type == MGX ) {
				m_lsMgxGroupList += *m_it;
			} else
			if( type == JAVA ) {
				m_lsJavaGroupList += *m_it;
			} else
			/*if( type == JAVA2 ) {
				m_lsJavaGroupList += *m_it;
			} else	*/
			if( type == NATIVE ) {
				m_lsNativeGroupList += *m_it;
			} else {
				continue ;
			}
		} else {
			#ifdef DEBUG
			cout<<"====== type is NULL "<<endl;
			#endif
			type = m_zConfig->readEntry( *m_it, "Exec", "");
			if( type == "" ) {
				type = m_zConfig->readEntry( *m_it, "JavaId", "");
				if( type != "" ) {				
					m_lsJavaGroupList += *m_it;
				} else {
					continue;
				}		
			} else 	{
				m_lsNativeGroupList += *m_it;						
			}		
		
		}
	}
}




