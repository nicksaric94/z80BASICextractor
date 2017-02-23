#include "common_functions.h"
#include <QMessageBox>

void fatal_error(char *message)
{
    QMessageBox::critical(NULL,QObject::tr("GRESKA!"), message);

    return;

}
