#include "messageform.h"
#include "ui_messageform.h"

MessageForm::MessageForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageForm)
{
    ui->setupUi(this);
    ui->msg_label->setAlignment(Qt::AlignCenter);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setStyleSheet("background-color:#c8c8c8;");
}

MessageForm::~MessageForm()
{
    delete ui;
}

void MessageForm::setMsg(QString msg)
{
    ui->msg_label->setText(msg);
}

void MessageForm::on_pushButton_clicked()
{
    this->hide();
}
