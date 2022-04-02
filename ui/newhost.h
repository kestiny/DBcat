#ifndef NEWHOST_H
#define NEWHOST_H
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class NewHost; }
QT_END_NAMESPACE

class NewHost : public QDialog
{
    Q_OBJECT
public:
    NewHost(QWidget *parent);

private slots:
    void slotClose() { return reject(); }
    void slotCatMysql();
    void slotCatSqlite();
    void slotSubmit();
    void slotCancel();

private:
    Ui::NewHost *ui;
};

#endif // NEWHOST_H
