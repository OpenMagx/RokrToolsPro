#include "ZTaskItem.h"

ZTaskItem::ZTaskItem( ZListBox* container, const QString itemType ):
	ZSettingItem( container, itemType )
{
	type = ITEM_NATIV;
	pid = 0;
	isSDPath = false;
}

ZTaskItem::~ZTaskItem()
{
}

 ITEMTYPE ZTaskItem::getType()
{
	return type;
}

 void ZTaskItem::setType( ITEMTYPE t)
{
	type = t;
}

 uint ZTaskItem::getPid()
{
	return pid;
}

 void ZTaskItem::setPid( uint pid2)
{
	pid = pid2;
}

 QString ZTaskItem::getGname()
{
	return uid;		
}

 void ZTaskItem::setGname( QString UID)
{
	uid = UID;
}

 void ZTaskItem::setOriName(QString name)
{
    oriName = name;
}

 QString ZTaskItem::getOriName()
{
    return oriName;
}

 bool ZTaskItem::isSD()
{
    return isSDPath;
}

 void ZTaskItem::setPath(bool isSD)
{
    isSDPath = isSD;
}




