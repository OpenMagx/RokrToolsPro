
#include <qfile.h>
#include "deathDlg.h"
#include "myDef.h"

deathDlg::deathDlg(QString &title, QString &text, const char* cmd) : ZPopup((ZPopup::POPUP_TYPE_E)1)
{
    ret=256;
    setTitle(title);
    setInstructText(text);
    bzero(command, 1024);
    if (cmd!=NULL) strcpy(command, cmd);
    else return;
    setTitleIcon("info_pop.bmp");
    hide();
    do_exec();
}

deathDlg::~deathDlg()
{
    bzero(command, 1024);
}
void deathDlg::do_exec()
{
    show();
    QTimer::singleShot(10, this, SLOT(system_exec()));
}
void deathDlg::system_exec()
{
    if (command[0]=='\0') return;
    printf("Debug: system(%s)\n", command);
    strcat(command, " > /tmp/exec_log.txt 2>&1");
    ret=system(command);
    ret>>=8;
    hide();
    if (ret!=0)
    {
        char title[32];
        sprintf(title, "Error: %d", ret);
        QString text="";
        FILE *fp=fopen("/tmp/exec_log.txt", "rb");
        int flen=0;
        if (fp!=NULL)
        {
            fseek(fp, 0, SEEK_END);
            flen=ftell(fp);
            if (flen>0)
            {
                char buffer[flen+1];
                memset(buffer, '\n', flen);
                rewind(fp);
                fread(buffer, 1, flen, fp);
                text+=ctr(buffer);
            }
            fclose(fp);
        }
        ZMessageDlg *errDlg = new ZMessageDlg(title, text, (ZMessageDlg::MessageDlgType)2);
        errDlg->setTitleIcon("error_pop.bmp");
        errDlg->exec();
        delete errDlg;
    }
    QFile::remove("rm -f /tmp/exec_log.txt");
}
