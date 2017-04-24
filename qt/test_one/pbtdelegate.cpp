#include "pbtdelegate.h"
#include <QApplication>
#include <QMouseEvent>
#include <QDebug>

AddNew::AddNew(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

void AddNew::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionButton button;
    QRect r = option.rect;/* + r.width() - 30*/;//the X coordinate
    int x,y,w,h;
    x = r.left();
    y = r.top();
    w = r.width();
    h = r.height();

    button.rect = QRect(x,y,w,h);
    button.text = "+";
    button.state = QStyle::State_Enabled;

    QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
}

bool AddNew::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if(event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *e = (QMouseEvent *)event;
        int clickX = e->x();
        int clickY = e->y();

        QRect r = option.rect;
        int x,y,w,h;
        x = r.left();
        y = r.top();
        w = r.width();
        h = r.height();

        if(clickX > x && clickX < x+w )
        {
            if(clickY > y && clickY < y+h )
            {

                emit sendNewFromPBT(1);
            }
        }
    }
}
/***************************************************************/

DelNew::DelNew(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

void DelNew::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionButton button;
    QRect r = option.rect;/* + r.width() - 30*/;//the X coordinate
    int x,y,w,h;
    x = r.left();
    y = r.top();
    w = r.width();
    h = r.height();

    button.rect = QRect(x,y,w,h);
    button.text = "-";
    button.state = QStyle::State_Enabled;

    QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
}

bool DelNew::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if(event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *e = (QMouseEvent *)event;
        int clickX = e->x();
        int clickY = e->y();

        QRect r = option.rect;
        int x,y,w,h;
        x = r.left();
        y = r.top();
        w = r.width();
        h = r.height();

        if(clickX > x && clickX < x+w )
        {
            if(clickY > y && clickY < y+h )
            {

                emit sendNewFromPBT(2);
            }
        }
    }
}

/********************************************************************/
ChangeNew::ChangeNew(QObject *parent) :
    QStyledItemDelegate(parent)
{

}
void ChangeNew::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionButton button;
    QRect r = option.rect;/* + r.width() - 30*/;//the X coordinate
    int x,y,w,h;
    x = r.left();
    y = r.top();
    w = r.width();
    h = r.height();

    button.rect = QRect(x,y,w,h);
    button.text = "修改";
    button.state = QStyle::State_Enabled;

    QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
}

bool ChangeNew::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if(event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *e = (QMouseEvent *)event;
        int clickX = e->x();
        int clickY = e->y();

        QRect r = option.rect;
        int x,y,w,h;
        x = r.left();
        y = r.top();
        w = r.width();
        h = r.height();

        if(clickX > x && clickX < x+w )
        {
            if(clickY > y && clickY < y+h )
            {

                emit sendNewFromPBT(3);
            }
        }
    }
}


/************************************************************/
BlakeDelegate::BlakeDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}







