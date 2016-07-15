#ifndef MESSAGEFORM_H
#define MESSAGEFORM_H

#include <QWidget>
#include <QString>
namespace Ui {
class MessageForm;
}

class MessageForm : public QWidget
{
    Q_OBJECT

public:
    explicit MessageForm(QWidget *parent = 0);
    ~MessageForm();
    void setMsg(QString msg);
private slots:
    void on_pushButton_clicked();

private:
    Ui::MessageForm *ui;
};

#endif // MESSAGEFORM_H
