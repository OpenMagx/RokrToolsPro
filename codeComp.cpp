
#include "codeComp.h"
#include "source.h"
#include "myDef.h"
#include "cfg.h"

extern Settings *cfg;

CDCP::CDCP()
{
	//imei = getIMEI();
	//imsi = getIMSI();
	
	//sprintf(keyList, "%s", KEY_LIST);
}

bool CDCP::cpcp()
{
	#ifdef DEVELOP_NO_REG
		return true;
	#endif
		return false;	
}



