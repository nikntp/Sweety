#include "sweetyteam.h"
#include "ui_sweetyteam.h"

SweetyTeam::SweetyTeam(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SweetyTeam)
{
    ui->setupUi(this);
}

SweetyTeam::~SweetyTeam()
{
    delete ui;
}
