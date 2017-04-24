#ifndef COMMONMODEL_H
#define COMMONMODEL_H

#include <QObject>
#include <QItemSelectionModel>
#include <QTableWidget>

class CommonModel : public QObject
{
    Q_OBJECT
public:
    explicit CommonModel(QObject *parent = 0);
    QList <int> RequestRowId(QItemSelectionModel *selections);
    void Add(QTableWidget *goal, QTableWidget *source);
    void Del(QTableWidget *source);
    void ScreenInfo(QTableWidget *source, QString &text);
    void LoadSQL(QTableWidget *source, QString name, QStringList &hearders);//将数据库写入widget

signals:

public slots:

};

#endif // COMMONMODEL_H
