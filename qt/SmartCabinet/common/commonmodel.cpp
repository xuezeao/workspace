#include "commonmodel.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>

CommonModel::CommonModel(QObject *parent) :
    QObject(parent)
{

}

QList <int > CommonModel::RequestRowId(QItemSelectionModel *selections)
{
        QModelIndexList selected = selections->selectedIndexes();
        QMap <int, int> rowMap;

        foreach (QModelIndex index, selected)
        {
            rowMap.insert(index.row(), 0);
        }
#ifdef TEST
        qDebug() << "print : " <<rowMap;
        qDebug() << "acount : " <<rowMap.count() ;
        qDebug() <<"i = 0 : "<<rowMap.keys();
#endif

        return rowMap.keys();
}


void CommonModel::Del(QTableWidget *source)
{
    QList<QTableWidgetItem *> array = source->selectedItems();
    int count = array.count();
    int num = 0;
    QList<int> rowNum;
    rowNum.append(-1);

    for (int i = 0; i < count; i++)
    {
        int row = source->row(array.at(i));
        if (rowNum[num] != row)
        {
            rowNum.append(row);
            num++;
        }
    }

    if (rowNum.length() > 1)
    {
        source->removeRow(rowNum[1]);//删除第一条
    }
    for (int i = 2; i < rowNum.length(); i++)
    {
        source->removeRow(rowNum[i] - (i - 1));
    }
}

void CommonModel::Add(QTableWidget *goal, QTableWidget *source)
{
    QList<QTableWidgetItem *> array = source->selectedItems();
    int count = array.count();
    int old = -1;
    int current = -1;
    int nowCount = -1;
    int rowCount = -1;

    for (int i = 0; i < count; i++)
    {

        int row = source->row(array.at(i));
        QTableWidgetItem *item = array.at(i);
        QString name = item->text();

        current = row;
        if (source->item(current, 2)->text() == "在位")
        {
            if (current != old)//当前行号不等于之前存储行号说明需要新增一行
            {
                rowCount = goal->rowCount();
                goal->setRowCount(rowCount + 1);
                old = current;//存储当前行号
                nowCount = 0;//重置列数 应对：上一行数据不全导致列数少实际总的需要列数

            }
            else
            {
                nowCount++;//需要新增一列
                int columnCount = goal->columnCount();//当前表格列数
                if (columnCount <= nowCount)//需求大于实际表格具备的列数则新增一列
                {
                    goal->setColumnCount(columnCount + 1);
                }
            }

            if (0 == nowCount)//第一列作为排序，需要插入整型变量
            {
                QTableWidgetItem *test = new QTableWidgetItem();//插入整型
                int integer = name.toInt();
                qDebug()<<integer;
                test->setData(Qt::DisplayRole, integer);
                goal->setItem(rowCount,nowCount,test);
            }
            else
            {
                goal->setItem((rowCount),(nowCount),new QTableWidgetItem(name));
            }
        }
    }
}

void CommonModel::ScreenInfo(QTableWidget *source, QString &text)
{
    int rC = source->rowCount();//获得行号

    if ("" == text)//判断输入是否为空
    {
        for (int i = 0; i < rC; i++)
        {
            source->setRowHidden(i, false);//显示所有行
        }
    }
    else
    {
        //获取符合条件的cell索引
        QList <QTableWidgetItem *> item = source->findItems(text, Qt::MatchContains);

        for (int i = 0; i < rC; i++)
        {
            source->setRowHidden(i, true);//隐藏所有行
        }

        if (!item.isEmpty())//不为空
        {
            for (int i = 0; i < item.count(); i++)
            {
                source->setRowHidden(item.at(i)->row(),false);//item.at(i).row()输出行号
            }
        }
    }
}


void CommonModel::LoadSQL(QTableWidget *source, QString name, QStringList &hearders)
{
    QSqlQuery query;
    int nColumn = 0;
    int nRow = 0;

    query.exec(QString("select * from %1").arg(name));
    query.last();
    nRow = query.at() + 1;

    QSqlRecord rec = query.record();
    nColumn = rec.count();

    source->setRowCount(nRow);
    source->setColumnCount(nColumn);
    source->setHorizontalHeaderLabels(hearders);

    query.first();
    for (int i = 0; i < nRow; i++)
    {
        for (int j = 0; j < nColumn; j++)
        {
            if (0 == j)
            {
                QTableWidgetItem *test = new QTableWidgetItem();//插入整型
                int integer = query.value(j).toInt();
                test->setData(Qt::DisplayRole, integer);
                source->setItem(i,j,test);
            }
            else
            {
                source->setItem(i,j,new QTableWidgetItem(query.value(j).toString()));
            }
        }

        query.next();
    }
}























