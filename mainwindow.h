#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "listvars.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_actionIzlaz_triggered();

    void on_actionInfo_triggered();

    void on_actionInfo_toggled(bool arg1);

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void openListVars();

private:
    Ui::MainWindow *ui;
    ListVars *lv;
};

#endif // MAINWINDOW_H
