#ifndef SECONDWINDOW_H
#define SECONDWINDOW_H

#include <QDialog>

namespace Ui {
class SecondWindow;
}

class SecondWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SecondWindow(QWidget *parent = nullptr);
    ~SecondWindow();

    void northWestCornerRule();

    void fixDegenerateCase();

    void steppingStone();

    void showResult();

public slots:
    void receiveData(QVector<double> supply, QVector<double> demand, QVector< QVector<double> > costs, QVector<QString> title);

private slots:
    void on_back_clicked();

    void on_pushSelectDir_clicked();

    void on_pushExportCSV_clicked();

private:
    Ui::SecondWindow *ui;
    QString dir_name;
};

#endif // SECONDWINDOW_H
