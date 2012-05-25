
#include "lng.h"
#include <qtextcodec.h>
#include <qtextstream.h>

Lng::Lng(QString &lngFile)
{
	#ifdef DEBUG
	qDebug("====== creat lng \n");
	#endif	
	lngCfg = new ZConfig(lngFile);
}

Lng::Lng(const char *lngFile)
{
	#ifdef DEBUG
	qDebug("====== creat lng \n");
	#endif	
	lngCfg = new ZConfig(QString(lngFile));
}

Lng::~Lng()
{
 	#ifdef DEBUG
	qDebug("====== delete lng cfg \n");
	#endif   
	delete lngCfg;
	#ifdef DEBUG
	qDebug("====== delete lng \n");
	#endif
}
void Lng::changeLngFile(QString &lngFile)
{
	if( NULL != lngCfg ) {
		delete lngCfg;
		lngCfg = new ZConfig(lngFile);
	}	
}

QString Lng::tr( const char * sourceText)
{
    QString tmp = lngCfg->readEntry("LNG", QString(sourceText), "");
	if(tmp == "") {
	    tmp = QString(sourceText);
		printf("%s = \n", sourceText); 
	}	
	return tmp; 
}


