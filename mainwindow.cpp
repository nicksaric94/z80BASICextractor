#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "listvars.h"
#include <dirent.h>
#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>
#include "common_functions.h"
#include "basic_to_ascii.h"
#include "memory_extractor.h"

//instead in .header files, will be declared here, and in other files with prefix "extern"
int array_index; //used when traversing arrays
char buffer[50]; //output buffer for printing numbers;
FILE* source;
int version;
char is_compressed;
int header_end;
unsigned char* memory;
enum hardware_mode_enum {forty, onetwenty, SamRam} machine_type; //hardware mode determines markers of different memory pages

char* path_for_list;
char* path_for_vars;

char msg[MSG_MAX];
//
QString t_1="";
QString t_2="";

int cols;
FILE* output;
FILE* vars_output;

int process_command_arguments(int argc, char** argv)
{
  int i, vars_only = FALSE, list_only = FALSE;
  char* extension_begin;
  int extension_begin_index, path_len;
  int output_file_index = cols = 0;

  for (i = 0; i < argc; i++)
  {
    if (strcmp("-c", argv[i]) == 0)
    {
      cols = atoi(argv[++i]);
    }
    else if (strcmp("-o", argv[i]) == 0)
    {
      if (output_file_index != -1)
      {
        output_file_index = i + 1;
      }
      i++;
    }
    else if (strcmp("-stdout", argv[i]) == 0)
    {
      output_file_index = -1;
    }
    else if (strcmp("-vars", argv[i]) == 0)
    {
      vars_only = TRUE;
    }
    else if (strcmp("-list", argv[i]) == 0)
    {
      list_only = TRUE;
    }
    else
    {
      open_file(argv[i]);
      path_for_list = strdup(argv[i]);
      path_for_vars = strdup(argv[i]);
    }
  }

  if (vars_only && list_only) //if both are set, reset the exclusivity
  {
    vars_only = FALSE;
    list_only = FALSE;
  }

  //we had the name supplied with -o
  if (output_file_index > 0)
  {
    if (vars_only != TRUE)
    {
      output = fopen(argv[output_file_index], "w");
      if (output == NULL)
      {
        strcpy(msg,"process_command_arguments(): Neuspesna alokacija.");
        fatal_error(msg);
        return -1;
      }
    }

    if (list_only != TRUE)
    {
      path_len = strlen(argv[output_file_index]);
      path_for_vars = (char*)malloc(path_len + 6); //we remove the extension from the provided path and replace it with "-vars.txt" to mark the files with variables
      if (path_for_vars == NULL)
      {
        strcpy(msg,"process_command_arguments(): Neuspesna alokacija.");
        fatal_error(msg);
        return -1;
      }
      path_for_vars = strcpy(path_for_vars, argv[output_file_index]); //copy the path to the enlarged buffer
      extension_begin = strrchr(path_for_vars, '.'); //get the pointer to the last . in the filename
      if (extension_begin == NULL)
      {
        path_for_vars = (char*)realloc(path_for_vars, path_len + 10); //if there is no extension, add sufix to the end of file name
        if (path_for_vars == NULL)
        {
          strcpy(msg,"process_command_arguments(): Neuspesna alokacija.");
          fatal_error(msg);
          return -1;
        }
        extension_begin = &(path_for_vars[path_len]);
      }
      extension_begin = strcpy(extension_begin, "-vars.txt"); //overwrite the end
      vars_output = fopen(path_for_vars, "w");
      if (vars_output == NULL)
      {
        strcpy(msg,"process_command_arguments(): Neuspesno otvaranje fajla za izlistavanje promenljivih.");
        fatal_error(msg);
        return -1;
      }
    }
  }
  else if (output_file_index == 0)
  {
    if (vars_only != TRUE)
    {
      //replace "z80" in the path with "txt"
      extension_begin = strrchr(path_for_list, '.');
      extension_begin_index = extension_begin - path_for_list;
      path_for_list[extension_begin_index + 1] = 't';
      path_for_list[extension_begin_index + 2] = 'x';
      path_for_list[extension_begin_index + 3] = 't';
      output = fopen(path_for_list, "w");
      if (output == NULL)
      {
        strcpy(msg,"process_command_arguments(): Neuspesno otvaranje fajla za izlistavanje programa.");
        fatal_error(msg);
        return -1;
      }
    }

    if (list_only != TRUE)
    {
      path_len = strlen(path_for_vars);
      path_for_vars = (char*)realloc(path_for_vars, path_len + 6);
      if (path_for_vars == NULL)
      {
        strcpy(msg,"process_command_arguments(): Neuspesna alokacija.");
        fatal_error(msg);
        return -1;
      }
      extension_begin = strrchr(path_for_vars, '.');
      extension_begin = strcpy(extension_begin, "-vars.txt");
      vars_output = fopen(path_for_vars, "w");
      if (vars_output == NULL)
      {
        strcpy(msg,"process_command_arguments(): Neuspesno otvaranje fajla za izlistavanje promenljivih.");
        fatal_error(msg);
        return -1;
      }
    }
  }
  else {
    //-stdout option
    if (vars_only != TRUE)
    {
      output = stdout;
    }
    if (list_only != TRUE)
    {
      vars_output = stdout;
    }
  }
  return 0;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPixmap pix("D:/Users/Nikola/Downloads/ZX_Spect-whoami_d-1540/pix.png");
    ui->label_pic->setPixmap(pix);
    ui->lineEdit->setReadOnly(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionIzlaz_triggered()
{
    close();
}

void MainWindow::on_actionInfo_triggered()
{
    QMessageBox::information(this,tr("O programu:"), tr("Parametri:\n"
                                                   "- Putanja - Potrebno je odabrati fajl tipa .z80 za dalju obradu.\n"
                                                   "  (za pojedinačnu obradu fajlova)\n"
                                                   "- Opcije - Potrebno je odabrati bar jednu od opcija izlistavanja\n"
                                                   "  programa i promenljivih.\n"
                                                   "- Izlistavanje - Odabir opcije \"Izlistaj sve\" ne zahteva putanju i\n"
                                                   "  vrši izlistavanje svih fajlova tipa .z80 u tekućem direktorijumu.\n\n"
                                                   "Program napravili:\n"
                                                   " - Uroš Gojković\n"
                                                   " - Nikola Sarić"));

}

void MainWindow::on_pushButton_clicked()
{
   QString ime_fajla= QFileDialog::getOpenFileName(
          this,
          tr("Izaberite fajlove"),
          "./",
          "Spektrum Z80 (*.z80)"
          );
    if(!ime_fajla.isNull()){
    ui->lineEdit->setText(ime_fajla);
}
}

void MainWindow::on_pushButton_3_clicked()
{
    int argc=0;
    char** argv;
    argv = (char**)malloc(3 * sizeof(char*));
    if(argv==NULL){
        strcpy(msg,"arguments: Neuspesna alokacija.");
        fatal_error(msg);
        return;
    }

    QLineEdit *putanja= ui->lineEdit;
    QCheckBox *prog= ui->checkBox;
    QCheckBox *prom= ui->checkBox_2;


    int put_len=0;

    if(putanja->text()!=""){
        argc++;
        put_len= putanja->text().size();
        argv[0] = (char*)malloc((put_len+1) * sizeof(char));
        if(argv[0] == NULL){
            strcpy(msg,"arguments: Neuspesna alokacija.");
            fatal_error(msg);
            return;
        }

        QByteArray array = putanja->text().toLocal8Bit();
        char* put = array.data();
        strcpy(argv[0],put);

    }

    if(prog->isChecked()){
        if(argc==1){
            argc++;
            argv[1] = (char*)malloc(6 * sizeof(char));
                if(argv[1] == NULL){
                    strcpy(msg,"arguments: Neuspesna alokacija.");
                    fatal_error(msg);
                    return;
                }

           strcpy(argv[1],"-list");
           }
    }
    if(prom->isChecked()){
        if(argv[0]!=NULL){
            argc++;
            argv[argc-1] = (char*)malloc(6 * sizeof(char));
                if(argv[argc-1] == NULL){
                    strcpy(msg,"arguments: Neuspesna alokacija.");
                    fatal_error(msg);
                    return;
                }

           strcpy(argv[argc-1],"-vars");
           }

    }

    if(!prog->isChecked()&&!prom->isChecked()){
        QMessageBox::warning(this,tr("Parametri:"), tr("Potrebno je odabrati bar jednu od opcija izlistavanja programa i promenljivih."));
    }
    else if(putanja->text()== ""){
        QMessageBox::warning(this,tr("Parametri:"), tr("Potrebno je odabrati fajl tipa .z80 za dalju obradu."));
    }
    else {
          //EXTRACTION_BEGIN
              if(process_command_arguments(argc, argv)){
                  return;
              }
              if(check_header()){
                  return;
              }
              if(extract_pages()){
                  return;
              }

              if(prog->isChecked()){
                  extract_basic(memory, output, cols);
              }
              if(prom->isChecked()){
                  extract_basic_variables(memory, vars_output, cols);
              }

              if (output != stdout)
              {
               QMessageBox::information(this,tr("Izlistavanje zavrseno!"), tr("Uspesno ste izlistali datoteku!"));
               fclose(output);
               fclose(vars_output);
               if(prog->isChecked()){
                 t_1=path_for_list;
               }
               if(prom->isChecked()){
                 t_2=path_for_vars;
               }
               openListVars();
               t_1="";
               t_2="";
              }
          //EXTRACTION END
          }
}

void MainWindow::on_pushButton_4_clicked()
{
    DIR *dir;
    int postoji=0;
    char *ret;
    char *fext=(char*)".z80";
    struct dirent *ent;
    QString def_put= QCoreApplication::applicationDirPath();
    QByteArray arr = def_put.toLocal8Bit();
    char* dp = arr.data();
    QCheckBox *prog= ui->checkBox;
    QCheckBox *prom= ui->checkBox_2;


    if(!prog->isChecked()&&!prom->isChecked()){
        QMessageBox::warning(this,tr("Parametri:"), tr("Potrebno je odabrati bar jednu od opcija izlistavanja programa i promenljivih."));
    }
    else{
    if ((dir = opendir (dp)) != NULL) {
      while ((ent = readdir (dir)) != NULL) {
        ret=strrchr(ent->d_name, '.');
        if(ret!=NULL){
          if((strcmp(ret,fext))==0){
                 //POKRENI
              {
              postoji=1;
              int argc=0;
              char** argv;
              argv = (char**)malloc(3 * sizeof(char*));
              if(argv==NULL){
                  strcpy(msg,"arguments: Neuspesna alokacija.");
                  fatal_error(msg);
                  return;
              }

              QString putanja= QCoreApplication::applicationDirPath();
              putanja.append("/");
              putanja.append(ent->d_name);
              ui->lineEdit->setText(putanja);

              int put_len=0;
              int poredi = QString::compare(putanja, "");
              if(poredi){
                  argc++;
                  put_len= putanja.size();
                  argv[0] = (char*)malloc((put_len+1) * sizeof(char));
                  if(argv[0] == NULL){
                      strcpy(msg,"arguments: Neuspesna alokacija.");
                      fatal_error(msg);
                      return;
                  }

                  QByteArray array = putanja.toLocal8Bit();
                  char* put = array.data();
                  strcpy(argv[0],put);

              }

              if(prog->isChecked()){
                  if(argc==1){
                      argc++;
                      argv[1] = (char*)malloc(6 * sizeof(char));
                          if(argv[1] == NULL){
                              strcpy(msg,"arguments: Neuspesna alokacija.");
                              fatal_error(msg);
                              return;
                          }

                     strcpy(argv[1],"-list");
                     }
              }
              if(prom->isChecked()){
                  if(argv[0]!=NULL){
                      argc++;
                      argv[argc-1] = (char*)malloc(6 * sizeof(char));
                          if(argv[argc-1] == NULL){
                              strcpy(msg,"arguments: Neuspesna alokacija.");
                              fatal_error(msg);
                              return;
                          }

                     strcpy(argv[argc-1],"-vars");
                     }

              }
                    //EXTRACTION_BEGIN
                        if(process_command_arguments(argc, argv)){
                            return;
                        }
                        if(check_header()){
                            return;
                        }
                        if(extract_pages()){
                            return;
                        }
                        if(prog->isChecked()){
                            extract_basic(memory, output, cols);
                        }
                        if(prom->isChecked()){
                            extract_basic_variables(memory, vars_output, cols);
                        }

                        if (output != stdout)
                        {
                         fclose(output);
                         fclose(vars_output);
                        }
                    //EXTRACTION END
                    }
                 //ZAVRSI
              }
          }
        }

      closedir (dir);
        if (postoji){
           QMessageBox::information(this,tr("Izlistavanje zavrseno!"), tr("Uspesno ste izlistali sve datoteke u direktorijumu!"));
        }
        else{
           QMessageBox::warning(this,tr("Parametri:"), tr("U direktorijumu ne postoji nijedan fajl tipa .z80."));
        }
        } else {
           QMessageBox::critical(NULL,QObject::tr("GRESKA!"), QObject::tr("Neuspesno otvaranje direktorijuma."));
        }
    }
}
void MainWindow::openListVars()
{
    lv= new ListVars(this);
    lv->show();

}
