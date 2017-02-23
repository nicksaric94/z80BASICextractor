#include "listvars.h"
#include "ui_listvars.h"
#include <QFile>
#include <QTextStream>

extern QString t_1;
extern QString t_2;

ListVars::ListVars(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ListVars)
{

    ui->setupUi(this);
    ui->tb_1->setLineWrapMode(QTextEdit::NoWrap);
    ui->tb_2->setLineWrapMode(QTextEdit::NoWrap);
    this->setFixedSize(800,650);
    this->setMinimumSize(100,100);
    ui->tb_1->setFixedSize(380,615);
    ui->tb_2->setFixedSize(380,615);

    QFile prog(t_1);
    QFile vars(t_2);

    if(prog.open(QIODevice::ReadOnly))
    {
        QTextStream in(&prog);
        ui->tb_1->setText(in.readAll());
    }

    if(vars.open(QIODevice::ReadOnly))
    {
        QTextStream in(&vars);
        ui->tb_2->setText(in.readAll());
    }

}

ListVars::~ListVars()
{
    delete ui;
}
