#include "dialog_controlthread.h"
#include "ui_dialog_controlthread.h"
#include "wthread.h"
#include <QVBoxLayout>

Dialog_ControlThread::Dialog_ControlThread(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_ControlThread)
{
    ui->setupUi(this);

    QPushButton *A = new QPushButton(tr("A测试"));
    QPushButton *B = new QPushButton(tr("B测试"));
    QPushButton *C = new QPushButton(tr("C测试"));
    QPushButton *D = new QPushButton(tr("D test"));

    QVBoxLayout *vBL = new QVBoxLayout;
    vBL->addWidget(A);
    vBL->addSpacerItem(new QSpacerItem(20, 20));
    vBL->addWidget(B);
    vBL->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
    vBL->addWidget(C);
    vBL->addSpacing(0);
    vBL->setMargin(0);

    QHBoxLayout *hBL = new QHBoxLayout;



    hBL->addWidget(D);






    setLayout(vBL);

    connect(A, SIGNAL(clicked()), this, SLOT(A_pushButton()));
    connect(B, SIGNAL(clicked()), this, SLOT(B_pushButton()));
    connect(C, SIGNAL(clicked()), this, SLOT(C_pushButton()));

    timer = new QTimer;
//    connect(timer, SIGNAL(timeout()), this, SLOT(A_pushButton()));
    timer->start(1000);

    thread_http = new QThread;
    thread_query = new QThread;
    thread_httpTest = new QThread;
    httpA = new HttpGP;
    httpB = new HttpTest;
    seriportQuery = new SeriportQuery;

    httpA->moveToThread(thread_http);
//    httpA->accessManager->moveToThread(thread_http);
    seriportQuery->moveToThread(thread_query);

    thread_http->start();
    thread_query->start();
    connect(this, SIGNAL(startLoop()), seriportQuery, SLOT(requestSTM()));
    connect(this, SIGNAL(http(int)), httpA, SLOT(GetHttp(int)));
    connect(timer, SIGNAL(timeout()), httpB, SLOT(GetHttpa()));

//    WThread hread;
//    hread.start();

}

Dialog_ControlThread::~Dialog_ControlThread()
{
    delete ui;
}

void Dialog_ControlThread::A_pushButton()
{
    emit http(2);
}
void Dialog_ControlThread::B_pushButton()
{
    emit http(3);
}
void Dialog_ControlThread::C_pushButton()
{
    emit startLoop();
}
