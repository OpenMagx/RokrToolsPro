#ifndef ZTASKITEM_H
#define ZTASKITEM_H

#include <ZSettingItem.h>
#include "source.h"


class Q_EXPORT ZTaskItem : public ZSettingItem
{
public: 
	ZTaskItem( ZListBox* container = 0, const QString itemType = NULL );
	~ZTaskItem();

	 void setOriName(QString name); //full cmd for each process in tasklist
	 QString getOriName();
	
	 ITEMTYPE getType();
	 void setType( ITEMTYPE type);		
	
	 uint getPid();
	 void setPid( uint pid);
	
	 QString getGname();
	 void setGname( QString UID);	
	
	 bool isSD();
	 void setPath(bool isSD);
	
private:
	ITEMTYPE type;
	uint pid;
	bool isSDPath;
	//int data;
	QString oriName;
	QString uid;
};

#endif //ZTASKITEM_H
