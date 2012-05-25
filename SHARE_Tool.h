

// Writen by Ant-ON, 2010
//
// This class in lib: libezxshare.so
//
// Example: Send file by BT
//
// QString file = "/mmc/mmc1/test.png";
//
// SHARE_Tool::OBJECT_INFO info;
// info.n = file.right( file.findRev("/")!=-1?(file.length()-file.findRev("/")):file.length() );
// info.t = "media/"+file.right( 3 );
// if ( !SHARE_Tool::(info, file) )
//    qDebug("Error send file!");

#ifndef SHARE_TOOL_H
#define SHARE_TOOL_H

#include <qstringlist.h>
#include <qarray.h>
#include <quuid.h>

class QUnknownInterface;
class SDData;

class ActionInterface
{
    public:	
		ActionInterface(); //
    	~ActionInterface();
    
    	void queryInterface( const QUuid &, QUnknownInterface ** );
    	void getReturnValue();
    	void addRef();   	
};

class SHARE_ActionImpl: public ActionInterface
{
    public:	
    	SHARE_ActionImpl();
    	~SHARE_ActionImpl();
    	
    	bool invokeAction(QString const&, SDData const&);
    	QUuid getUuid();
};

class SHARE_Tool
{
    public:
		SHARE_Tool();//
		~SHARE_Tool();//
        enum SHARE_METHOD
        {
        };

        struct OBJECT_INFO
        {
            QString  n;
            QString  t;
        };

        struct SHARE_OBJECT
        {
            OBJECT_INFO info;
            QString     file;
            bool        removed;
        };
         
		static bool shareByBt(const QValueList<OBJECT_INFO>& info, const QArray<char>& array, const QValueList<int>& sizes);      
		static bool shareByBt(const QValueList<SHARE_OBJECT> & o);
		static bool shareByBt(const OBJECT_INFO& info, const QString& file, bool removed = false);
		static bool shareByBt(const OBJECT_INFO& info, const QArray<char>& o);

    protected:
		static QString genTmpFileName(SHARE_METHOD method, const QString& name, const QString& type);
        static bool saveObject(const QString& file, const QArray<char>& o, int, int);
        static bool shareByBtIrda(SHARE_METHOD method, const QValueList<SHARE_OBJECT>& o);

  //  private:
    //    SHARE_Tool();
};


#endif
