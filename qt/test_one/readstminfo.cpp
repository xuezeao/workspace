#include "readstminfo.h"
#include <QFile>
#include <QMessageBox>
#include "uart4stm.h"
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

extern int Alarm_No[3][32];

ReadSTMInfo::ReadSTMInfo()
{

}

void ReadSTMInfo::searchError()//总的记录
{
    writeFile(0,0);
    ErrorOperate();
    moveOperate();
    notDoOperate();
}

int ReadSTMInfo::ErrorOperate()//错误操作检测
{

    writeFile(1,0);
    for(int i = 0; i < 32; i++)
    {
        if(Alarm_No[0][i])
        {
            writeFile(2,Alarm_No[0][i]);
        }
        else
        {
            qDebug()<<"错误操作：共有 "<<i;
            break;
        }
    }

}

int ReadSTMInfo::moveOperate()//移动记录
{
    writeFile(3,0);
    for(int i = 0; i < 32; i++)
    {
        if(Alarm_No[1][i])
        {
            writeFile(4,Alarm_No[1][i]);
        }
        else
        {
            qDebug()<<"移动操作：共有 "<<i;
            break;
        }
    }
}

int ReadSTMInfo::notDoOperate()//未操作记录
{
    writeFile(5,0);
    for(int i = 0; i < 32; i++)
    {
        if(Alarm_No[2][i])
        {
            writeFile(6,Alarm_No[2][i]);
        }
        else
        {
            qDebug()<<"未操作：共有 "<<i;
            break;
        }
    }
}


void ReadSTMInfo::readFile()//读文件
{

}

void ReadSTMInfo::writeFile(int status,int i)//写文件
//0:写入用户名等信息 1 2 ：写入错误 3 4 ：写入移动 5 6：写入未操作
{
    QFile data("log.txt");
    if(data.open(QIODevice::Append | QIODevice::Text | QIODevice::WriteOnly));
    else
    {
        QMessageBox *qMesBox;
        qMesBox->setText("无法打开日志文件");
        qMesBox->exec();
    }
    QTextStream out(&data);

    if(status == 0)//写入用户名等信息
    {
        QSqlQuery query;
        query.exec(QString("select * from T_UserInfo"));
        query.seek(0);
        QString str = QString("usrId: ")+query.value(0).toString()+\
                QString("  ，role： ")+query.value(1).toString()+QString("  ;");

        QDateTime time = QDateTime::currentDateTime();
        QString strTime = time.toString("yyyy-MM-dd hh:mm:ss ddd");

        out<<endl<<QObject::tr("系统时间： ")<<strTime<<endl;
        out<<QObject::tr("用户信息：")<<endl;
        out<<str;


    }
    else if(status == 1 || status == 2)//写入错误信息
    {
       if(status == 1)
       {
           out<<endl<<QObject::tr("错误操作： ");

       }
       else if(status == 2)
       {
           out<<QObject::tr(" , ")<<i;
       }
    }
    else if(status == 3 || status == 4)//写入移动信息
    {
        if(status == 3)
        {
            out<<endl<<QObject::tr("移动操作： ");

        }
        else if(status == 4)
        {
            out<<QObject::tr(" , ")<<i;
        }
    }
    else if(status == 5 || status == 6)//写入未操作
    {
        if(status == 5)
        {
            out<<endl<<QObject::tr("未操作： ");
        }
        else if(status == 6)
        {
            out<<QObject::tr(" , ")<<i;
        }
    }

    data.close();

}
