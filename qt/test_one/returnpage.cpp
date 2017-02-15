#include "returnpage.h"
#include "ui_returnpage.h"
#include <QSqlRelationalDelegate>

ReturnPage::ReturnPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReturnPage)
{
    ui->setupUi(this);
    T_table = &T_TABLE;

//      this->showFullScreen();
//      showMaximized();
//      setWindowFlags(Qt::FramelessWindowHint);

    query                = new QSqlQuery;
    T_model_Other_Execut = new QSqlTableModel;

    Operate_execut_Page  = new Execut_window;
    show_Info_Page       = new ShowAllInfo;
    delayTime = new DelayTime;


    RHeader = new ExecuteSheet_R(Qt::Horizontal,this);
    checkBoxDelegate = new CheckBoxDelegate(this);
    returnAgentiaChechBoxDelegate = new ReturnAgentiaCheckBoxDelegate(this);

    change_NewOperate = new ChangeNew(this);

    ui->tableView_showExecuteInfo->setHorizontalHeader(RHeader);
    ui->tableView_showExecuteInfo->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView_showExecuteInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);//窗口不可编辑

//头改子列
    connect(RHeader,SIGNAL(stateUP_R(int)),this,SLOT(onState(int)));

//子改头列
    connect(checkBoxDelegate,SIGNAL(headUP(int)),RHeader,SLOT(SetHeaderBox(int)));
    connect(returnAgentiaChechBoxDelegate,SIGNAL(headUP_R(int)),RHeader,SLOT(SetHeaderBox(int)));

//子改头列
    connect(this,SIGNAL(stateUp_ReturnPage(int)),RHeader,SLOT(SetHeaderBox(int)));

    connect(Operate_execut_Page,SIGNAL(upSheet_From_Execute()),this,SIGNAL(returnPage_To_MainUi()));//返回主界面

}



void ReturnPage::onState(int state)
{
    QString setInfo;

    if((state == 1) || (state == 0))
    {

        if(state == 1)
        {
            setInfo = "选择";
        }
        else
        {
            setInfo = "未选择";
        }

        writeStatus(1,1,setInfo);

    }

    else if((state == 2) || (state == 3))
    {

        if(state == 3)
        {
            setInfo = "选择";
        }
        else
        {
            setInfo = "未选择";
        }

        writeStatus(2,1,setInfo);

    }

    else if((state == 4)||(state == 5)||(state == 6)||(state == 7)|| \
             (state == 8)||(state == 9))
    {

        if(state == 4)
        {
            setInfo = "归还";
            writeStatus(2,6,setInfo);
        }

        else if(state == 5)
        {
            setInfo = "替换";
            writeStatus(2,7,setInfo);
        }

        else if(state == 6)
        {
            setInfo = "报废";
            writeStatus(2,8,setInfo);
        }

        else if(state == 7)
        {
            setInfo = "未";
            writeStatus(2,6,setInfo);
        }

        else if(state == 8)
        {
            setInfo = "未";
            writeStatus(2,7,setInfo);
        }

        else if(state == 9)
        {
            setInfo = "未";
            writeStatus(2,8,setInfo);
        }
    }
}

void ReturnPage::writeStatus(int order, int num,QString setInfo)//order 0:search 1:execute
{
    int countRow = 0;
    if(order == 2)
    {
        countRow = T_model_Other_Execut->rowCount();
        for(int i = 0; i < countRow; i++)
        {
            T_model_Other_Execut->setData(T_model_Other_Execut->index(i,num),QString("%1").arg(setInfo));
        }
    }
}

ReturnPage::~ReturnPage()
{
    delete ui;
}
/********************************/
void ReturnPage::on_Bt_backBefore_clicked()
{
    closePage();
}

void ReturnPage::on_Bt_execute_clicked()
{
    scanStatus();//扫描试剂操作

    Operate_execut_Page->show();
    Operate_execut_Page->executeInit(2);

}

/**************************************************/
void ReturnPage::ModelSelect(int num)//1:入柜 2：取 3：还
{
    switch (num) {
    case 3:{
        T_table->T_execut_RelationTable = "T_AgentiaWaitExecute";
        T_table->dialog_model = 3;
        ui->label_title->setText("还试剂");
        tableInit(3);
        break;
    }
    default:
        break;
    }

  updateNo();
}


void ReturnPage::tableInit(char modelOption)//1:入柜
{
    if(modelOption == 3)//还试剂
    {
        T_model_Other_Execut->setTable(QString("%1").arg(T_table->T_execut_RelationTable));
        T_model_Other_Execut->select();

        T_model_Other_Execut->setHeaderData(0,Qt::Horizontal,QObject::tr("序号"));
        T_model_Other_Execut->setHeaderData(1,Qt::Horizontal,QObject::tr("勾选"));
        T_model_Other_Execut->setHeaderData(2,Qt::Horizontal,QObject::tr("试剂名"));
        T_model_Other_Execut->setHeaderData(3,Qt::Horizontal,QObject::tr("额定容积"));
        T_model_Other_Execut->setHeaderData(4,Qt::Horizontal,QObject::tr("试剂容量"));
        T_model_Other_Execut->setHeaderData(5,Qt::Horizontal,QObject::tr("到期日期"));
        T_model_Other_Execut->setHeaderData(6,Qt::Horizontal,QObject::tr("归还"));
        T_model_Other_Execut->setHeaderData(7,Qt::Horizontal,QObject::tr("替换"));
        T_model_Other_Execut->setHeaderData(8,Qt::Horizontal,QObject::tr("报废"));
        T_model_Other_Execut->setHeaderData(9,Qt::Horizontal,QObject::tr("抽屉号"));
        T_model_Other_Execut->setHeaderData(10,Qt::Horizontal,QObject::tr("位置"));
        T_model_Other_Execut->setHeaderData(11,Qt::Horizontal,QObject::tr("试剂类别"));
        T_model_Other_Execut->setHeaderData(12,Qt::Horizontal,QObject::tr("试剂ID"));
        T_model_Other_Execut->setHeaderData(13,Qt::Horizontal,QObject::tr("位置ID"));
        T_model_Other_Execut->setHeaderData(14,Qt::Horizontal,QObject::tr("状态"));

        T_model_Other_Execut->setEditStrategy(QSqlTableModel::OnManualSubmit);//自动提交

        ui->tableView_showExecuteInfo->setModel(T_model_Other_Execut);
        ui->tableView_showExecuteInfo->setItemDelegateForColumn(1,checkBoxDelegate);
        ui->tableView_showExecuteInfo->setItemDelegate(returnAgentiaChechBoxDelegate);

    }

}

void ReturnPage::closePage()
{
    emit returnPage_To_MainUi();
    delayTime->waitTaskInfo(50);
    this->close();
}


void ReturnPage::scanStatus()
{//6,7,8

    QString returnA = "";
    QString changeA = "";
    QString desertA = "";
    QString error = "";
    QString checkStatus = "";

    int allrow = T_model_Other_Execut->rowCount();

    if(T_table->dialog_model == 3)
    {
        for(int i = 0; i <= allrow; i++)
        {
            checkStatus = T_model_Other_Execut->data(T_model_Other_Execut->index(i,1)).toString();

            if (checkStatus == "选择")
            {
                returnA = T_model_Other_Execut->data(T_model_Other_Execut->index(i,6)).toString();
                changeA = T_model_Other_Execut->data(T_model_Other_Execut->index(i,7)).toString();
                desertA = T_model_Other_Execut->data(T_model_Other_Execut->index(i,8)).toString();

                if(returnA == "归还")
                {
                    error = "归还";
                }

                else if(changeA == "替换")
                {
                    error = "替换";
                }

                else if(desertA == "报废")
                {
                    error = "报废";
                }

                else if((returnA == "未")||(changeA == "未")||(desertA == "未"))
                {
                    error = "归还";
                }

                T_model_Other_Execut->setData(T_model_Other_Execut->index(i,13),error);
                T_model_Other_Execut->submitAll();
            }
        }
    }
}

void ReturnPage::updateNo(void)//更新数据表格行列号
{
    int E_allRow = 0;//执行表格的行数

    E_allRow = T_model_Other_Execut->rowCount();
    for (int i = 0 ; i < E_allRow ; i++)
    {
        T_model_Other_Execut->setData(T_model_Other_Execut->index(i,0),i+1);
        T_model_Other_Execut->submitAll();
    }
}

