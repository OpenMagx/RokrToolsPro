
#include <ZApplication.h>
#include <ZConfig.h>
#include <qstring.h>
#include <qfile.h>
#include <qdir.h>
#include <qtextcodec.h>
#include <qtextstream.h>

#include <time.h>

#include <dirent.h>
#include <sys/stat.h>
#include <iostream>

#include "UtilsTools.h"



QString readFileLine(const QString & fileName)
{
    QFile f(fileName);
    QString s;
    if ( f.open( IO_ReadOnly ) )
    {
        QTextStream t( &f );
        if ( !t.eof() )
            s = t.readLine();
        f.close();
    }
    return s;
}
//////////////////////////////////////////////////////////////////////////////////////

bool readLineConfig(QString &file, QString &key, QString &type, QString &dest)
{
	if( !QFile::exists(file) )
		return false;
		
	QFile f(file);
	QString str;
    if ( f.open( IO_ReadOnly ) ) 
    {
		while ( !f.atEnd() )
		{
			f.readLine(str, 512);
			//cout<<str<<endl;
			if(str.find( key+type ) > -1) {
				dest = str.replace(key+type, "");
				f.close();
				return true;
			}		
		}
		f.close();
		return false;
	}
	return false;
}

bool setLineConfig(QString &file, QString &key, QString &type, QString &value)
{
	if( !QFile::exists(file) )
		return false;
	
	bool status = false;
	QString str;
	QString dest;
	QFile f(file);
    if ( f.open( IO_ReadOnly | IO_WriteOnly ) ) 
    {
		while ( !f.atEnd() )
		{
			f.readLine(str, 512);
			if(str.find(key+type) > -1) {
				dest += key+type+value;
				status = true;				
			} else {
				dest += str;
			}
		}
		
		if( status == false )
			dest += key+type+value;
			
		f.close();

		if ( f.open( IO_WriteOnly | IO_Truncate ) )
		{
			f.writeBlock( dest, dest.length() );
			f.close();
			return true;
		}
		return false;	
	}
	return false;
}
bool deleteKeyLine(QString &file, QString &deleteMark)
{
	if( !QFile::exists(file) )
		return false;

	QString str;
	QString dest;
	QFile f(file);
    if ( f.open( IO_ReadOnly | IO_WriteOnly ) ) 
    {
		while ( !f.atEnd() )
		{
			f.readLine(str, 512);
			if(str.find(deleteMark) > -1)
				continue;
			else
				dest += str;		
		}
		f.close();
		
		if ( f.open( IO_WriteOnly | IO_Truncate ) )
		{
			f.writeBlock( dest, dest.length() );
			f.close();
			return true;
		}
		return false;	
	}
	return false;
}
/*
QString ctr(const char*ChineseString)
{
	QTextCodec* gbk_codec = QTextCodec::codecForName("UTF-8");
	return gbk_codec->toUnicode(ChineseString);
}
*/

QString getAppDir()
{
    QString tmp = QString(qApp->argv()[0]);
    int i = tmp.findRev("/");
    tmp.remove(i+1,tmp.length()-1);
    return tmp;
}

QString getIMEI()
{
	QString result = QString::null;

	unsigned int res;
	TAPI_IMEI_NUMBER_A imei;

	TAPI_CLIENT_Init ( NULL, 0 );
	res = TAPI_ACCE_GetImei ( imei );
	if ( res == 0 ) 
	{
		QString pImei = QString::fromLatin1 ( reinterpret_cast<char*> ( imei ) );
		result = pImei;
	}
	TAPI_CLIENT_Fini();
	
	return result;
}

QString getIMSI()
{
	QString result = QString::null;

	unsigned int res;
	TAPI_IMSI_NUMBER_A imsi;

	TAPI_CLIENT_Init ( NULL, 0 );
	res = TAPI_ACCE_GetImsi ( imsi );
	if ( res == 0 ) 
	{
		QString pImsi = QString::fromLatin1 ( reinterpret_cast<char*> ( imsi ) );
		result = pImsi;
	}
	TAPI_CLIENT_Fini();
	
	return result;    
}

QString getGname(int step)
{
	time_t t;
	srand( (unsigned)time(&t) );
	
    int data[8];
    QString uuid;

    for (int i = 0; i < 8; ++i) {
        data[i] = rand() % step;//255;
		if (data[i] < 16)
			uuid += "0";
		 uuid += QString::number(data[i],16);
		if( i==4 || i==6 || i==8 ) 
			uuid += "-";
	}
	return uuid;
}
int showQ(const QString &title, const QString &text, int type)
{
	ZMessageDlg* dlg = new ZMessageDlg(title, text, (ZMessageDlg::MessageDlgType)type);
	int ret = dlg->exec();
	delete dlg;
	dlg = NULL;
	return ret;
}

void showNotify(const QString &title, const QString &text, int type, int time)
{
	ZNoticeDlg *dlg = new ZNoticeDlg((ZNoticeDlg::Type)type, text, "");
	if(type==2) dlg->setTitleIcon("error_pop.bmp");
	dlg->setTitle(title);
	dlg->setAutoDismissTime(time*1000);
	dlg->exec();
	delete dlg;
	dlg = NULL;
}

bool isValidFileName(QString &fileName)
{
	if(fileName == "") return true;
	char resultName[128];
	sprintf(resultName, fileName.local8Bit());
	int i=0, j=strlen(resultName);
	for(i=0; i<j; i++)
	{
		switch(resultName[i])
		{
			case '*':
			case '/':
			case '\\':
			case '<':
			case '>':
			case ':':
			case '|':
			case '?':
			case '"':
				return true;
				break;
			default:
				break;
		}
	}
	return false;
}

unsigned int getDirSize(const char* path)
{
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	unsigned int size=0;
	char newDir[256];
	if ((dp=opendir(path))==NULL) {
		return 0;
	}
	chdir(path);
	while ((entry=readdir(dp))!=NULL)
	{
		if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
		{
			stat(entry->d_name, &statbuf);
			if ((statbuf.st_mode&S_IFMT)==S_IFDIR) 	{
				sprintf(newDir, "%s/%s", path, entry->d_name);
				size+=getDirSize(newDir);
			}
			else if ((statbuf.st_mode&S_IFMT)==S_IFREG) {
				size+=statbuf.st_size;
			}
		}
	}
	return size;
}

QString size2string(unsigned long size)
{
    if (size < 1024) 
        return QString("%1 b").arg(size);
    else if (size < 1024 * 1024)
        return QString("%1 Kb").arg((double) size / 1024);
    else 
        return QString("%1 Mb").arg((double) size / 1024 / 1024);
	return "";
}




