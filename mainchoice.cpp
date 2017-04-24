#include "mainchoice.h"
#include "ui_mainchoice.h"
#include <QPushButton>
#include <QString>
#include <QLabel>
#include <QLineEdit>
#include <QBoxLayout>
#include <QTableView>

MainChoice::MainChoice(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainChoice)
{
    ui->setupUi(this);

    QPushButton *choichPage = new QPushButton(this);
    QPushButton *pubilcPage = new QPushButton(this);
    QPushButton *searchPage = new QPushButton(this);
    m_stackedLayout = new QStackedLayout();
    QVBoxLayout *frist_layout = new QVBoxLayout();
    QHBoxLayout *H_pushBotton = new QHBoxLayout();

    H_pushBotton->addWidget(choichPage);
    H_pushBotton->addWidget(pubilcPage);
    H_pushBotton->addWidget(searchPage);
    H_pushBotton->setSpacing(10);

    choichPage->setText(QStringLiteral("选择功能"));
    choichPage->setFixedSize(130, 30);
    pubilcPage->setText(QStringLiteral("公告栏"));
    pubilcPage->setFixedSize(130, 30);
    searchPage->setText(QStringLiteral("搜索试剂"));
    searchPage->setFixedSize(130, 30);

    QLabel *publicName = new QLabel(this);
    QLabel *publicName1 = new QLabel(this);
    QLabel *publicName2 = new QLabel(this);

//    QLineEdit *publicInfo = new QLineEdit(this);

    publicName->setText(QStringLiteral("aaa"));
    publicName1->setText(QStringLiteral("ddd"));
    publicName2->setText(QStringLiteral("ccc"));
    m_stackedLayout->addWidget(publicName);
    m_stackedLayout->addWidget(publicName1);
    m_stackedLayout->addWidget(publicName2);

    frist_layout->addLayout(H_pushBotton, Qt::AlignCenter);
    frist_layout->addLayout(m_stackedLayout);
    frist_layout->setSpacing(10);
    frist_layout->setContentsMargins(10, 10, 10,10);

    ui->main_widget->setLayout(frist_layout);


    connect(choichPage, &QPushButton::clicked, this, &MainChoice::SwitchPage);
    connect(pubilcPage, &QPushButton::clicked, this, &MainChoice::SwitchPage1);
    connect(searchPage, &QPushButton::clicked, this, &MainChoice::SwitchPage2);

}

MainChoice::~MainChoice()
{
    delete ui;
}

void MainChoice::SwitchPage()
{
    m_stackedLayout->setCurrentIndex(0);
}
void MainChoice::SwitchPage1()
{
    m_stackedLayout->setCurrentIndex(1);
}
void MainChoice::SwitchPage2()
{
    m_stackedLayout->setCurrentIndex(2);
}


