#ifndef MAINCHOICE_H
#define MAINCHOICE_H

#include <QMainWindow>
#include <QStackedLayout>

namespace Ui {
class MainChoice;
}

class MainChoice : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainChoice(QWidget *parent = 0);
    ~MainChoice();

private slots:
    void SwitchPage(void);
    void SwitchPage1(void);
    void SwitchPage2(void);

private:
    Ui::MainChoice *ui;
    QStackedLayout *m_stackedLayout;
};

#endif // MAINCHOICE_H
