#ifndef CHECKBOXDELEGATE_H
#define CHECKBOXDELEGATE_H


#define CHECKBOX_COLUMN_0 0
#define CHECKBOX_COLUMN_1 1
#define CHECKBOX_COLUMN_2 2
#define CHECKBOX_COLUMN_6 6
#define CHECKBOX_COLUMN_7 7
#define CHECKBOX_COLUMN_8 8

#include <QStyledItemDelegate>
#include <QSqlTableModel>

class CheckBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CheckBoxDelegate(QObject *parent = 0);

signals:
    void headUP(int state);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

private:
    int data;

};

class MySqlTableModel : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit MySqlTableModel(QObject *parent = 0 , QSqlDatabase db = QSqlDatabase());

    QVariant data(const QModelIndex &index, int role) const;
//    bool setData(const QModelIndex &index, const QVariant &value, int role);
//    Qt::ItemFlags flags(const QModelIndex &index) const;
private:
//    bool setName(int studentId, const QString &name);
//    void refresh();



};

class ReturnAgentiaCheckBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ReturnAgentiaCheckBoxDelegate(QObject *parent = 0);

signals:
    void headUP_R(int state);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

private:
    int data;
};

#endif // CHECKBOXDELEGATE_H
