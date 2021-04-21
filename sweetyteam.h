#ifndef SWEETYTEAM_H
#define SWEETYTEAM_H

#include <QDialog>

namespace Ui {
class SweetyTeam;
}

class SweetyTeam : public QDialog
{
    Q_OBJECT

public:
    explicit SweetyTeam(QWidget *parent = nullptr);
    ~SweetyTeam();

private:
    Ui::SweetyTeam *ui;
};

#endif // SWEETYTEAM_H
