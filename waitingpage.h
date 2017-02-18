#ifndef WAITINGPAGE_H
#define WAITINGPAGE_H

#include <QDialog>

namespace Ui {
class WaitingPage;
}

class WaitingPage : public QDialog
{
    Q_OBJECT

public:
    explicit WaitingPage(QWidget *parent = 0);
    ~WaitingPage();

private:
    Ui::WaitingPage *ui;
};

#endif // WAITINGPAGE_H
