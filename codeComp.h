#ifndef CODECOMP_H
#define CODECOMP_H

#include <qdatetime.h>
#include <iostream>


class CDCP 
{
public: 
	CDCP();
	~CDCP() {};
	bool cpcp();
	
private:
	QString imei;
	QString imsi;
	QString code;
	int step;//= 0;
	int n;
	char codeStr[16];
	char keyStr[16];
	QString key_chk;
	
    char keyList[128];	
};

#endif //CODECOMP_H
