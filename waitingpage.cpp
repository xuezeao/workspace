#include "waitingpage.h"
#include "ui_waitingpage.h"
#include <QMovie>
#include <QGridLayout>
#include <QLabel>

WaitingPage::WaitingPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WaitingPage)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);//隐藏标题栏

    QLabel *gif_label = new QLabel();
    QLabel *prompt_label = new QLabel();

    QGridLayout *grid_layout = new QGridLayout;
    grid_layout->addWidget(gif_label, 1, 1, Qt::AlignCenter);
    grid_layout->addWidget(prompt_label, 2, 1, Qt::AlignCenter);

    QMovie *movie = new QMovie("C:/workspace/qt/SmartCabinetV0_0_1/resource/loading.gif");
    gif_label->setMovie(movie);
    gif_label->setFixedSize(400, 240);
    prompt_label->setText("数据加载中，请稍等....");
    prompt_label->setFixedSize(130, 20);
    movie->start();

    setLayout(grid_layout);

}

WaitingPage::~WaitingPage()
{
    delete ui;
}
