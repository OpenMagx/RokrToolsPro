//
// C++ Interface: BaseDlg
//
// Description: 
//
//
// Author: root <root@andLinux>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BASEDLG_H
#define BASEDLG_H

#include <ZKbMainWidget.h>

class MyBaseDlg : public ZKbMainWidget {
  Q_OBJECT

  public:
    MyBaseDlg(); 
    ~MyBaseDlg();
    
    int exec();
    int result() const { return rescode; }
    
  public slots:
    virtual void done(int);
    virtual void accept();
    virtual void reject();
        
  private:
    bool myInLoop;
    int rescode;
        
  protected:
    enum DialogCode { Rejected, Accepted };
    void setResult(int r) { rescode = r; }    
};

#endif
