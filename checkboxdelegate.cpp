 #include "checkboxdelegate.h"

#include <QCheckBox>
#include <QApplication>
#include <QMouseEvent>
#include <QDebug>
#include <QSqlQuery>

CheckBoxDelegate::CheckBoxDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}


void CheckBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    QStyleOptionViewItem viewOption(option);
    initStyleOption(&viewOption , index);
    if(option.state.testFlag(QStyle::State_HasFocus))
        viewOption.state = viewOption.state ^ QStyle::State_HasFocus;

    QStyledItemDelegate::paint(painter, viewOption, index);
    QStyleOptionButton checkBoxStyle;

    QString data1 = index.model()->data(index).toString();

    if(data1 == "未选择")
    {
        checkBoxStyle.state =  QStyle::State_Off;
    }
    else if(data1 == "选择"){
        checkBoxStyle.state =  QStyle::State_On;
    }

    checkBoxStyle.state |= QStyle::State_Enabled;
    checkBoxStyle.iconSize = QSize(30,30);
    checkBoxStyle.rect = option.rect;

    QCheckBox checkBox;
    QApplication::style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &checkBoxStyle, painter, &checkBox);

}

bool CheckBoxDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QRect decorationRect = option.rect;

    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

    if(event->type() == QEvent::MouseButtonPress && decorationRect.contains(mouseEvent->pos()))
    {
        if(index.column() == CHECKBOX_COLUMN_1)
       {

            QString data1 = index.model()->data(index).toString();
            if(data1 == "未选择")
            {
                model->setData(index, "选择");
            }
            else if(data1 == "选择")
            {
                model->setData(index, "未选择");
            }

            emit headUP(1);//更改表头状态
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}



MySqlTableModel::MySqlTableModel(QObject * parent, QSqlDatabase db) : QSqlTableModel(parent,db)
{

}


QVariant MySqlTableModel::data(const QModelIndex &index, int role) const
{
    QVariant value = QSqlQueryModel::data(index, role);
//        if (role == Qt::TextColorRole && index.column() == 0)
//            return qVariantFromValue(QColor(Qt::red)); //第一个属性的字体颜色为红色

        if (role == Qt::TextAlignmentRole && index.column() == 1)
        {
            value = (Qt::AlignVCenter + Qt::AlignHCenter); // 居中
        }



        return value;
}

/*
bool MySqlTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
//    if (index.column() < 1 || index.column() > 2)
//          return false;

//      QModelIndex primaryKeyIndex = QSqlQueryModel::index(index.row(), 0);
//      int id = data(primaryKeyIndex).toInt(); //获取id号

//      clear();

//      bool ok;
//      if (index.column() == 1) //第二个属性可更改
//          ok = setName(id, value.toString());

//      refresh();
//      return ok;
}

Qt::ItemFlags MySqlTableModel::flags(const QModelIndex &index) const
{
//    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
//      if (index.column() == 1) //第二个属性可更改
//          flags |= Qt::ItemIsEditable;
//      return flags;
}

void MySqlTableModel::refresh() //更新显示
{
//    setQuery("select * from student");
//    setHeaderData(0, Qt::Horizontal, QObject::tr("id"));
//    setHeaderData(1, Qt::Horizontal, QObject::tr("name"));
}

bool MySqlTableModel::setName(int studentId, const QString &name) //添加name属性的值
{
//    QSqlQuery query;
//    query.prepare("update student set name = ? where id = ?");
//    query.addBindValue(name);
//    query.addBindValue(studentId);
//    return query.exec();
}
*/

ReturnAgentiaCheckBoxDelegate::ReturnAgentiaCheckBoxDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}


void ReturnAgentiaCheckBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    QStyleOptionViewItem viewOption(option);
    initStyleOption(&viewOption , index);
    if(option.state.testFlag(QStyle::State_HasFocus))
        viewOption.state = viewOption.state ^ QStyle::State_HasFocus;

    QStyledItemDelegate::paint(painter, viewOption, index);

    if((index.column() == CHECKBOX_COLUMN_6) || (index.column() == CHECKBOX_COLUMN_7) || (index.column() == CHECKBOX_COLUMN_8))
    {
        QStyleOptionButton checkBoxStyle;
        QString data1 ;
        QString data2 ;
        QString data3 ;
        if(index.column() == CHECKBOX_COLUMN_6)
        {
            data1 = index.model()->data(index).toString();
        }
        if(index.column() == CHECKBOX_COLUMN_7)
        {
            data2 = index.model()->data(index).toString();
        }
        if(index.column() == CHECKBOX_COLUMN_8)
        {
            data3 = index.model()->data(index).toString();
        }


        if(data1 == "未")
        {
            checkBoxStyle.state =  QStyle::State_Off;
        }
        else if(data1 == "归还"){
            checkBoxStyle.state =  QStyle::State_On;
        }
        if(data2 == "未")
        {
            checkBoxStyle.state =  QStyle::State_Off;
        }
        else if(data2 == "替换"){
            checkBoxStyle.state =  QStyle::State_On;
        }
        if(data3 == "未")
        {
            checkBoxStyle.state =  QStyle::State_Off;
        }
        else if(data3 == "报废"){
            checkBoxStyle.state =  QStyle::State_On;
        }



//        checkBoxStyle.state = data ? QStyle::State_On : QStyle::State_Off;
        checkBoxStyle.state |= QStyle::State_Enabled;
        checkBoxStyle.iconSize = QSize(30,30);
        checkBoxStyle.rect = option.rect;



        QCheckBox checkBox;
        QApplication::style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &checkBoxStyle, painter, &checkBox);
   }
}

bool ReturnAgentiaCheckBoxDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QRect decorationRect = option.rect;

    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

    if(event->type() == QEvent::MouseButtonPress && decorationRect.contains(mouseEvent->pos()))
    {
        if((index.column() == CHECKBOX_COLUMN_6) || (index.column() == CHECKBOX_COLUMN_7) || (index.column() == CHECKBOX_COLUMN_8))
       {
            if(index.column() == CHECKBOX_COLUMN_6)
            {

                QString data1 = index.model()->data(index).toString();
                if(data1 == "未")
                {
                    model->setData(index, "归还");
                    model->setData(model->index(index.row(),index.column()+1),"未");
                    model->setData(model->index(index.row(),index.column()+2),"未");

                }

                else if(data1 == "归还")
                {
                    model->setData(index, "未");


                }
                emit headUP_R(2);//更改表头状态
            }
            if(index.column() == CHECKBOX_COLUMN_7)
            {

                QString data1 = index.model()->data(index).toString();
                if(data1 == "未")
                {
                    model->setData(index, "替换");
                    model->setData(model->index(index.row(),index.column()-1),"未");
                    model->setData(model->index(index.row(),index.column()+1),"未");
                }
                else if(data1 == "替换")
                {
                    model->setData(index, "未");


                }
                 emit headUP_R(2);//更改表头状态
            }
            if(index.column() == CHECKBOX_COLUMN_8)
            {

                QString data1 = index.model()->data(index).toString();
                if(data1 == "未")
                {
                    model->setData(index, "报废");
                    model->setData(model->index(index.row(),index.column()-2),"未");
                    model->setData(model->index(index.row(),index.column()-1),"未");
                }
                else if(data1 == "报废")
                {    model->setData(index, "未");


                }
                emit headUP_R(2);//更改表头状态
            }
       }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}


