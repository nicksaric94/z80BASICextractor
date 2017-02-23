#ifndef LISTVARS_H
#define LISTVARS_H
#include <QDialog>
namespace Ui {
class ListVars;
}

class ListVars : public QDialog
{
    Q_OBJECT

public:
    explicit ListVars(QWidget *parent = 0);
    ~ListVars();
    Ui::ListVars *ui;

private:
};

#endif // LISTVARS_H
