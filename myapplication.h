
#ifndef MYAPPLICATION_H_
#define MYAPPLICATION_H_

#include <ZApplication.h>

class MyApplication : public ZApplication
{
    Q_OBJECT
    public:
        MyApplication(int argc,char *argv[]);
    protected slots:
        virtual void slotShutdown();
        virtual void slotQuickQuit();
};
#endif //MYAPPLICATION_H_


