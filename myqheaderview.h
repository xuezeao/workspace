#ifndef MYQHEADERVIEW_H
#define MYQHEADERVIEW_H

#define CHECKBOX_COLUMN_0 0
#define CHECKBOX_COLUMN_1 1
#define CHECKBOX_COLUMN_2 2
#define CHECKBOX_COLUMN_6 6
#define CHECKBOX_COLUMN_7 7
#define CHECKBOX_COLUMN_8 8

#include <QHeaderView>


class MyQHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    explicit MyQHeaderView(Qt::Orientation orientation, QWidget *parent = 0);



signals:
      void stateUP(int state);

private slots:
    void SetHeaderBox(int state);

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
    void mousePressEvent(QMouseEvent *e);


private:
    struct{

        bool isOn;

    }test_1;


};

class ExecuteSheet : public QHeaderView
{
    Q_OBJECT
public:
    explicit ExecuteSheet(Qt::Orientation orientation, QWidget *parent = 0);



signals:
      void stateUP_ES(int state);

private slots:
    void SetHeaderBox(int state);

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
    void mousePressEvent(QMouseEvent *e);


private:
    struct{

        bool isOn;

    }test_1;


};

class ExecuteSheet_R : public QHeaderView
{
    Q_OBJECT
public:
    explicit ExecuteSheet_R(Qt::Orientation orientation, QWidget *parent = 0);



signals:
      void stateUP_R(int state);

private slots:
    void SetHeaderBox(int state);

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
    void mousePressEvent(QMouseEvent *e);


private:
    struct{

        bool isOn;
        bool isOn1;
        bool isOn2;
        bool isOn3;

    }test_1;


};

#endif // MYQHEADERVIEW_H
