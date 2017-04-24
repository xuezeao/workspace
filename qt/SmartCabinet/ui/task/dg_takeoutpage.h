#ifndef DG_TAKEOUTPAGE_H
#define DG_TAKEOUTPAGE_H

#include <QDialog>
#include "common/updatesql.h"
#include "common/commonmodel.h"
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QMouseEvent>
#include <QMap>

namespace Ui {
class Dg_TakeOutPage;
}

class Dg_TakeOutPage : public QDialog
{
    Q_OBJECT

public:
    explicit Dg_TakeOutPage(QWidget *parent = 0);
    ~Dg_TakeOutPage();
    QString table_exec;
    QString table_sea;

private slots:
    void on_pB_exec_clicked();

    void on_pB_apply_clicked();

    void on_pB_delete_clicked();

    void on_pB_search_clicked();

    void on_CB_return_clicked();

protected:
    void SetTitle(QString name);

private:

    Ui::Dg_TakeOutPage *ui;
    UpdateSQL *updateSQL;
    CommonModel *commonModel;

    void Add(QTableWidget *goal, QTableWidget *source);
    void Del(QTableWidget *source);
    void ShowUI();
    void ShowSQL();
    void DownLoad_TaskList();
    void SortAscend();
    void mousePressEvent(QMouseEvent *event);
    void CleanSQLTable(QString name);
    void Write_Sql(QMap <int, QString> map, QString &name);
    bool GetContent(QTableWidget *source, QString name);
    virtual void ShowExecuteWindow();

};

#endif // DG_TAKEOUTPAGE_H
