
#ifndef LNG_H
#define LNG_H


#include <ZConfig.h>
#include <qstring.h>

#include <iostream>

#include "myDef.h"

class Lng 
{

public:
	Lng(QString &lngFile);
	Lng(const char *lngFile);
	~Lng();
    QString tr( const char * sourceText);
	void changeLngFile(QString &lngFile);

private:
	ZConfig *lngCfg;
};

#endif //LNG_H


