#ifndef NEWHOST_H
#define NEWHOST_H
#include <QDialog>
#include "dbtype.h"

QT_BEGIN_NAMESPACE
namespace Ui { class NewHost; }
QT_END_NAMESPACE

class NewHost : public QDialog
{
    Q_OBJECT
public:
    NewHost(QWidget *parent);
    void setHostInfo(HostInfo info);

private slots:
    void slotClose() { return reject(); }
    void slotCatMysql();
    void slotCatSqlite();
    void slotSubmit();
    void slotCancel();

private:
    Ui::NewHost *ui;
    HostInfo* hostInfo;
};

#endif // NEWHOST_H
