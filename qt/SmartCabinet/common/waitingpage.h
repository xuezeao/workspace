#ifndef WAITINGPAGE_H
#define WAITINGPAGE_H

#include <QDialog>
#include <QFont>

class WaitingPage : public QDialog
{
    Q_OBJECT

public:
    explicit WaitingPage(QWidget *parent = 0);
    ~WaitingPage();

private:

};

#endif // WAITINGPAGE_H
