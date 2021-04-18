#ifndef SOLUTION_H
#define SOLUTION_H

#include <QDialog>

namespace Ui {
class Solution;
}

class Solution : public QDialog
{
    Q_OBJECT

public:
    explicit Solution(QWidget *parent = nullptr);
    ~Solution();

    void northWestCornerRule();

    void fixDegenerateCase();

    void steppingStone();

    void showResult();

public slots:
    void receiveData(QVector<int> supply, QVector<int> demand, QVector< QVector<double> > costs, QVector<QString> title);
    //QVector<int> supply, QVector<int> demand, QVector< QVector<double> > costs

private slots:
    void on_back_clicked();

private:
    Ui::Solution *ui;
};

#endif // SOLUTION_H
