#ifndef PBTDELEGATE_H
#define PBTDELEGATE_H

#include <QStyledItemDelegate>

class AddNew : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit AddNew(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);


signals:
    void sendNewFromPBT(int option);

public slots:

};

class DelNew : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DelNew(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);


signals:
    void sendNewFromPBT(int option);

public slots:

};

class ChangeNew : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ChangeNew(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);


signals:
    void sendNewFromPBT(int option);

public slots:

};

class BlakeDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit BlakeDelegate(QObject *parent = 0);

//    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
//    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);


signals:
//    void sendNewFromPBT(int option);

public slots:

};
#endif // PBTDELEGATE_H
