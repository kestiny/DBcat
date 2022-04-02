#ifndef DBCAT_H
#define DBCAT_H
#include <QMainWindow>
#include <QTranslator>

QT_BEGIN_NAMESPACE
namespace Ui { class DBcat; }
QT_END_NAMESPACE

class DBcat : public QMainWindow
{
    Q_OBJECT

public:
    DBcat(QWidget *parent = nullptr);
    ~DBcat();

    void initWidgetSize();

private slots:
    void slotLoadStyleSheet(const QString& file);
    void slotExec();

private:
    Ui::DBcat *ui;
};
#endif // DBCAT_H
