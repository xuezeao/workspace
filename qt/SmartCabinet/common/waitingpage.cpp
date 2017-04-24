#include "waitingpage.h"
#include <QMovie>
#include <QGridLayout>
#include <QLabel>

WaitingPage::WaitingPage(QWidget *parent) :
    QDialog(parent)
{
    QFont ft_usuallySize;
    ft_usuallySize.setPointSize(20);

    setWindowFlags(Qt::FramelessWindowHint);//隐藏标题栏

    QLabel *gif_label = new QLabel();
    QLabel *prompt_label = new QLabel(QStringLiteral("数据加载中，请稍等...."));

    QGridLayout *grid_layout = new QGridLayout;
    grid_layout->addWidget(gif_label, 1, 1, Qt::AlignCenter);
    grid_layout->addWidget(prompt_label, 2, 1, Qt::AlignCenter);

    QMovie *movie = new QMovie(":/resource/image/loading.gif");
    gif_label->setMovie(movie);
    gif_label->setFixedSize(400, 240);

    prompt_label->setFont(ft_usuallySize);

    movie->start();

    setLayout(grid_layout);

}

WaitingPage::~WaitingPage()
{

}
