#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <RES_ICON_Reader.h>
#include <ZListBox.h>

#include "myDef.h"
#include "source.h"

class FileBrowser : public ZListBox
{
    Q_OBJECT
public :
    enum FILE_TYPE
    {
        FILE_PACKAGE, //.
        FILE_FONT, //
        FILE_SCRIPT, //
        FILE_THEME, //
        FILE_KM,//
        FILE_QKT,//
        FILE_IMG, //
        FILE_MEDIA,
        FILE_TEXT,
        FILE_DIR,
        FILE_SYMLINK,
        FILE_OTHER
    };
public :
    FileBrowser(const QString& dir, const QString& type = QString::null, QWidget* parent = 0, WFlags f = 0);
    ~FileBrowser();
    void init();  // append dirs/files to listbox
    void setDir(QString &pdir, ITEMMODE mode = NORMAL);//, const QString &sFilter = QString::null, const QString &sSort = QString::null);
    void setFilter(FILTERTYPE t = SHOW_ALL);
    void setMode(ITEMMODE mode = NORMAL);
    void enableMimeType(bool enable = true);
    bool isEnableMimeType();
    //void getChkItemList(QList <ZListBoxItem> &destList); //not work ..... --!// get the selected item lists
    QStringList getCheckedItemList();
    void selAll();
    void selNone();
    void cdBack();
    void setCursor(const QString &keyItem);
    ITEMMODE getMode();
    //void setSort(QString &sSort);
    QString getCurDir();
    QString getFileName(); // the file name
    QString getFilePath(); // full path of file
    int fileCount;
    int dirCount;
    bool isItemInfo; //当前项是否是列表头的当前列表信息项，true，是信息项，false，是实际文件/文件夹

signals:
    void fileHighlighted(int index);

private slots:
    void itemHighlighted(int index);
    void itemSelected(int index);

private :
    FILE_TYPE getFileType( const QString &fullPath );
    QPixmap getItemIcon( const QString &fullPath );
    QString adjustFile( const QString &file );
private :
    bool m_bMimeTypeState;
    QString m_strCurDir;
    QString m_strFileName;
    QString m_strFilePath;
    QString m_strAppDir;

    FILTERTYPE m_eFilterType;
    ITEMMODE m_eMode;

    RES_ICON_Reader m_iconReader;


};


#endif //FILEBROWSER_H

