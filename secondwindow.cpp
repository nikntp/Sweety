#include "secondwindow.h"
#include "ui_secondwindow.h"
#include "potentials_method.h"
#include <QVariant>
#include <iostream>
#include <iomanip>
#include <QMessageBox>
#include <QFileDialog>
#include "math.h"

using namespace transportation_problem;
using namespace std;
SecondWindow::SecondWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SecondWindow)
{
    ui->setupUi(this);
}

void SecondWindow::receiveData(QVector<double> sup, QVector<double> dem, QVector< QVector<double> > cost, QVector<QString> title)
{

    Vector suppliers = {}; for (auto a : sup) suppliers.push_back(a);
    Vector consumers = {}; for (auto b : dem) consumers.push_back(b);
    Matrix costs(suppliers.size(), consumers.size());
    for (int i = 0; i < cost.size(); ++i) for (int j = 0; j < cost[i].size(); ++j) costs[i][j] = cost[i][j];

    cout << "Northwest corner method test" << endl;
    auto solution = TableNCM(costs, suppliers, consumers);
    cout << "Plan:" << endl << solution.plan << endl;
    cout << "Cost of the plan = " << solution.f() << endl;
    cout << endl << endl;

    cout << "Potentials method test" << endl;
    auto optimizer = PotentialsMethod(solution);
    while (!optimizer.is_optimal())
    {
        optimizer.optimize();
    }
    cout << "Cost of the plan = " << optimizer.table.f() << endl;
    ui->tableWidget_result->setColumnCount(3);
    ui->tableWidget_result->setShowGrid(true);
    ui->tableWidget_result->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_result->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_result->setHorizontalHeaderLabels(QStringList() << ("factory 1")
                                                    << ("factory 2")
                                                    << ("cost"));
    ui->tableWidget_result->horizontalHeader()->setStretchLastSection(true);
    double z = 0;
    for (int j = 0; j < consumers.size(); ++j){
        double x1 = optimizer.table.plan[0][j], x2 = optimizer.table.plan[1][j];
        if (std::isnan(optimizer.table.plan[0][j])) x1 = 0;
        if (std::isnan(optimizer.table.plan[1][j])) x2 = 0;
        z += round((x1*cost[0][j]+x2*cost[1][j])*100)/100;
        ui->tableWidget_result->insertRow(ui->tableWidget_result->rowCount());
        ui->tableWidget_result->setItem(ui->tableWidget_result->rowCount()-1, 0, new QTableWidgetItem(QString::number(round(x1*100)/100)));
        ui->tableWidget_result->setItem(ui->tableWidget_result->rowCount()-1, 1, new QTableWidgetItem(QString::number(round(x2*100)/100)));
        ui->tableWidget_result->setItem(ui->tableWidget_result->rowCount()-1, 2, new QTableWidgetItem(QString::number(round((x1*cost[0][j]+x2*cost[1][j])*100)/100)));
    }
    ui->label_result->setText("минимальная стоимость доставки - " + QString::number(z));
}


SecondWindow::~SecondWindow()
{
    delete ui;
}

void SecondWindow::on_back_clicked()
{
    this->close();
}

void SecondWindow::on_pushSelectDir_clicked()
{
    QFileDialog directory;
    ui->lineEditDir->setText(directory.getSaveFileName(this,"Choose an directory"));
    dir_name = ui->lineEditDir->text();
}

void SecondWindow::on_pushExportCSV_clicked()
{
    QFile file(dir_name+".csv");
    if (file.open(QFile::WriteOnly | QFile::Truncate))
    {
        QTextStream data( &file );
        QStringList strList;
        for( int c = 0; c < ui->tableWidget_result->columnCount(); ++c )
        {
            strList <<
                       "\" " +
                       ui->tableWidget_result->horizontalHeaderItem(c)->data(Qt::DisplayRole).toString() +
                       "\" ";
        }
        data << strList.join(";") << "\n";

        for( int r = 0; r < ui->tableWidget_result->rowCount(); ++r )
        {
            strList.clear();
            for( int c = 0; c < ui->tableWidget_result->columnCount(); ++c )
            {   QTableWidgetItem* item = ui->tableWidget_result->item(r,c);        //Load items
                if (!item || item->text().isEmpty())                        //Test if there is something at item(r,c)
                {
                    ui->tableWidget_result->setItem(r,c,new QTableWidgetItem("0"));//IF there is nothing write 0
                }
                strList << "\" "+ui->tableWidget_result->item( r, c )->text()+"\" ";

            }
            data << strList.join( ";" )+"\n";
        }
        file.close();
        QMessageBox::information(this, "csv", "Файл успешно сохранен!");

    }
}
