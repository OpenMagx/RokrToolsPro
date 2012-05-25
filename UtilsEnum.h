#ifndef UTILSENUM_H
#define UTILSENUM_H

enum ITEMTYPE  // for app list box
{
	ITEM_NATIV,
	ITEM_JAVA,
	ITEM_SPECIAL,  //"phone" "flowplayer" "xp" "messaging" "RokrToolsPro"
	//ITEM_OTHE
	ITEM_FAV_OBJ,
	ITEM_MGX_OBJ,
	ITEM_MPKG_OBJ,
	ITEM_JAVA_OBJ,
	ITEM_MENU_OBJ
};

enum FILTERTYPE // for file browser
{
    NO_SHOW_HIDE,
	SHOW_ALL
};

enum ITEMMODE // for ztask item
{
    NORMAL,
	MULTI,	
};

enum FM_MOD // for megabox >> file manager part mod //��ǰϵͳ�ļ���������״̬���������ļ�����ʱ����ǣ�����һ�����У����ǵ�������
{
	FM_MOD_CUT,
	FM_MOD_COPY,
	FM_MOD_NORMAL
};

enum FILE_MOD_TYPE // �ļ����������ͣ��Ǹ���ճ��/���У������ƶ�
{
	FILE_PASTE,
	FILE_DELETE,
	FILE_MOVE,
	FILE_OTHER
};

#endif //UTILSENUM_H

