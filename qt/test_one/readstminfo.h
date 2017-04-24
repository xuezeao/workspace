#ifndef READSTMINFO_H
#define READSTMINFO_H
#include <QSqlQuery>

class ReadSTMInfo
{
public:
    ReadSTMInfo();
    void searchError();//写入状态

    void readFile();

private:

    int moveOperate();//移动记录
    int ErrorOperate();//错误操作检测
    int notDoOperate();//未操作记录
    void writeFile(int status , int i);//写文件
//0:写入用户名等信息 1 2 ：写入错误 3 4 ：写入移动 5 6：写入未操作


};

#endif // READSTMINFO_H
