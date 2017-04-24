#ifndef DG_CHECKPAGE_H
#define DG_CHECKPAGE_H

#include "ui/task/dg_takeoutpage.h"

class Dg_CheckPage : public Dg_TakeOutPage
{
    Q_OBJECT
public:
    explicit Dg_CheckPage(QWidget *parent = 0);

signals:

public slots:
private slots:


private:

    void ShowExecuteWindow();


};

#endif // DG_CHECKPAGE_H
