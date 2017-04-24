#ifndef DG_RETURNPAGE_H
#define DG_RETURNPAGE_H

#include <QDialog>
#include "common/commonmodel.h"
#include "common/updatesql.h"

namespace Ui {
class Dg_ReturnPage;
}

class Dg_ReturnPage : public QDialog
{
    Q_OBJECT

public:
    explicit Dg_ReturnPage(QWidget *parent = 0);
    ~Dg_ReturnPage();

private slots:
    void on_pB_Exec_clicked();

    void on_CB_return_clicked();

private:
    Ui::Dg_ReturnPage *ui;
    CommonModel *commonModel;
    UpdateSQL *updateSQL;


    void DownLoad_TaskList();
    void ShowGUI();
    void ShowExecuteWindow();
    bool GetContent(QTableWidget *source, QString name);
    bool WriteSelected2SQL(QTableWidget *source, QString table_name);//将被选行写入数据库
    void WriteSql(QMap <int, QString> map, QString name);
};

#endif // DG_RETURNPAGE_H
