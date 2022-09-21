#ifndef MESSAGEBOC_H
#define MESSAGEBOC_H
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class MessageBox; }
QT_END_NAMESPACE
class MessageBox : public QDialog
{
    Q_OBJECT
public:
    static int messageWarning(QWidget *parent, const QString &title, const QString &text);

private:
    MessageBox(const QString &title, const QString &text, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

private slots:
    void slotOk() { accept(); }
    void slotCancel() { reject(); }

private:
    Ui::MessageBox *ui;
};

#endif // MESSAGEBOC_H
