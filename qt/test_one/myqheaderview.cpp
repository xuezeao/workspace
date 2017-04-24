#include "myqheaderview.h"

#include <QPainter>
#include <QRect>
#include <QStyleOptionButton>
#include <QStyle>
#include <QCheckBox>
#include <QSize>
#include <QMouseEvent>
#include <QDebug>

MyQHeaderView::MyQHeaderView(Qt::Orientation orientation,QWidget *parent) :
    QHeaderView(orientation,parent)
{

    setHighlightSections(false);
    setMouseTracking(true);
    setSectionsClickable(true);//响应鼠标


    test_1.isOn = false;

}

void MyQHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();
    if( (logicalIndex == CHECKBOX_COLUMN_1))
    {
        QStyleOptionButton option;
        option.initFrom(this);

        if (test_1.isOn)
        {
            option.state = QStyle::State_On;

        }else{

            option.state = QStyle::State_Off;

        }

        QCheckBox   checkBox;
        option.iconSize = QSize(30,30);
        option.rect = rect;
        style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter, &checkBox);

    }
}
void MyQHeaderView::mousePressEvent(QMouseEvent *event)
{
    if (visualIndexAt(event->pos().x()) == CHECKBOX_COLUMN_1)
    {
        if (test_1.isOn)
        {
            test_1.isOn = false;
            emit stateUP(0);
        }
        else
        {
            test_1.isOn = true;
            emit stateUP(1);
        }
        update();
    }

    QHeaderView::mousePressEvent(event);
}

void MyQHeaderView::SetHeaderBox(int state)
{
    if(state ==1)
    {
        test_1.isOn = false;
        update();
    }
}
/**********************************************************************/

ExecuteSheet::ExecuteSheet(Qt::Orientation orientation,QWidget *parent) :
    QHeaderView(orientation,parent)
{
    setHighlightSections(false);
    setMouseTracking(true);
    setSectionsClickable(true);//响应鼠标


    test_1.isOn = false;

}

void ExecuteSheet::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();
    if( (logicalIndex == CHECKBOX_COLUMN_1))
    {
        QStyleOptionButton option;
        option.initFrom(this);

        if (test_1.isOn)
        {
            option.state = QStyle::State_On;

        }else{

            option.state = QStyle::State_Off;

        }

        QCheckBox   checkBox;
        option.iconSize = QSize(30,30);
        option.rect = rect;
        style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter, &checkBox);

    }
}
void ExecuteSheet::mousePressEvent(QMouseEvent *event)
{
    if (visualIndexAt(event->pos().x()) == CHECKBOX_COLUMN_1)
    {
        if (test_1.isOn)
        {
            test_1.isOn = false;
            emit stateUP_ES(2);
        }
        else
        {
            test_1.isOn = true;
            emit stateUP_ES(3);
        }
        update();
    }

    QHeaderView::mousePressEvent(event);
}

void ExecuteSheet::SetHeaderBox(int state)
{
    if(state ==1)
    {
        test_1.isOn = false;
        update();
    }
}

/************************************************************************/
ExecuteSheet_R::ExecuteSheet_R(Qt::Orientation orientation,QWidget *parent) :
    QHeaderView(orientation,parent)
{
    setHighlightSections(false);
    setMouseTracking(true);
    setSectionsClickable(true);//响应鼠标


    test_1.isOn  = false;
    test_1.isOn1 = false;
    test_1.isOn2 = false;
    test_1.isOn3 = false;

}

void ExecuteSheet_R::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();

    if( (logicalIndex == CHECKBOX_COLUMN_1))
    {
        QStyleOptionButton option;
        option.initFrom(this);

        if (test_1.isOn)
        {
            option.state = QStyle::State_On;

        }else{

            option.state = QStyle::State_Off;

        }


        QCheckBox   checkBox;
        option.iconSize = QSize(30,30);
        option.rect = rect;
        style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter, &checkBox);

    }
    if( (logicalIndex == CHECKBOX_COLUMN_6))
    {
        QStyleOptionButton option;
        option.initFrom(this);

        if (test_1.isOn1)
        {
            option.state = QStyle::State_On;

        }else{

            option.state = QStyle::State_Off;

        }


        QCheckBox   checkBox;
        option.iconSize = QSize(30,30);
        option.rect = rect;
        style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter, &checkBox);

    }
    if( (logicalIndex == CHECKBOX_COLUMN_7))
    {
        QStyleOptionButton option;
        option.initFrom(this);

        if (test_1.isOn2)
        {
            option.state = QStyle::State_On;

        }else{

            option.state = QStyle::State_Off;

        }


        QCheckBox   checkBox;
        option.iconSize = QSize(30,30);
        option.rect = rect;
        style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter, &checkBox);

    }
    if( (logicalIndex == CHECKBOX_COLUMN_8))
    {
        QStyleOptionButton option;
        option.initFrom(this);

        if (test_1.isOn3)
        {
            option.state = QStyle::State_On;

        }else{

            option.state = QStyle::State_Off;

        }


        QCheckBox   checkBox;
        option.iconSize = QSize(30,30);
        option.rect = rect;
        style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter, &checkBox);

    }
}
void ExecuteSheet_R::mousePressEvent(QMouseEvent *event)
{
    if (visualIndexAt(event->pos().x()) == CHECKBOX_COLUMN_1)
    {
        if (test_1.isOn)
        {
            test_1.isOn = false;
            emit stateUP_R(2);
        }
        else
        {
            test_1.isOn = true;
            emit stateUP_R(3);
        }
        update();
    }
    if (visualIndexAt(event->pos().x()) == CHECKBOX_COLUMN_6)
    {
        if (test_1.isOn1)
        {
            test_1.isOn1 = false;
            emit stateUP_R(7);
        }
        else
        {
            test_1.isOn1 = true;  
            test_1.isOn3 = false;
            test_1.isOn2 = false;
            emit stateUP_R(4);
            emit stateUP_R(8);
            emit stateUP_R(9);
        }
        update();
    }
    if (visualIndexAt(event->pos().x()) == CHECKBOX_COLUMN_7)
    {
        if (test_1.isOn2)
        {
            test_1.isOn2 = false;
            emit stateUP_R(8);
        }
        else
        {
            test_1.isOn2 = true;
            test_1.isOn1 = false;
            test_1.isOn3 = false;
            emit stateUP_R(5);
            emit stateUP_R(7);
            emit stateUP_R(9);
        }
        update();
    }
    if (visualIndexAt(event->pos().x()) == CHECKBOX_COLUMN_8)
    {
        if (test_1.isOn3)
        {
            test_1.isOn3 = false;
            emit stateUP_R(9);
        }
        else
        {
            test_1.isOn3 = true;
            test_1.isOn1 = false;
            test_1.isOn2 = false;
            emit stateUP_R(6);
            emit stateUP_R(7);
            emit stateUP_R(8);
        }
        update();
    }

    QHeaderView::mousePressEvent(event);
}

void ExecuteSheet_R::SetHeaderBox(int state)
{
    if(state == 1)
    {
        test_1.isOn = false;
    }
    if(state == 2)
    {
        test_1.isOn1 = false;
        test_1.isOn2 = false;
        test_1.isOn3 = false;
    }
    update();
}

