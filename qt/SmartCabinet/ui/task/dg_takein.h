#ifndef DG_TAKEIN_H
#define DG_TAKEIN_H

#include <QDialog>
#include "common/updatesql.h"
#include "common/commonmodel.h"
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QMouseEvent>
#include <QMap>

namespace Ui {
class Dg_TakeIn;
}

class Dg_TakeIn : public QDialog
{
    Q_OBJECT

public:
    explicit Dg_TakeIn(QWidget *parent = 0);
    ~Dg_TakeIn();

private slots:
    void on_pB_exec_clicked();

    void on_pB_apply_clicked();

    void on_pB_delete_clicked();

    void on_pB_search_clicked();

    void on_CB_return_clicked();

    void ShowPage();
    //接收位置信息
    void AcceptPositionInfo(int positionNo, int positionId,\
                            int drawer, QString dataTime, QString bottle,\
                            QString dose);

private:
    Ui::Dg_TakeIn *ui;
    UpdateSQL *updateSQL;
    CommonModel *commonModel;

    int selectRow;

    void ShowUI();
    void ShowSQL();
    void DownLoad_TaskList();
    void SortAscend();
    void mousePressEvent(QMouseEvent *event);
    void CleanSQLTable(QString name);
    void Write_Sql(QMap <int, QString> map, QString &name);
    bool GetContent(QTableWidget *source, QString name);
    void ShowExecuteWindow();
    void SetTitle(QString name);
};

#endif // DG_TAKEIN_H
