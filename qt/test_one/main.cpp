#include "mainui.h"
#include "enterpage.h"
#include <QApplication>
#include "uart4stm.h"
#include "Sql_Setting.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(!createConnection())//创建数据库，注意测试阶段数据库一启动便会初始化具体Sql_Setting文件
        return 0;


//    if(!(createSerialPort(QString("%1").arg(SERIALPORT),38400)))//打开串口
//    {
//        QMessageBox qMbox;
//        qMbox.setText(QString("串口被%1无法打开").arg(SERIALPORT));
//        qMbox.exec();
//    }

//    MainUI w;
    EnterPage w;
    w.show();





    return a.exec();
}
