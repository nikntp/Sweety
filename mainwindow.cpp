#include "mainwindow.h"
#include "secondwindow.h"
#include "ui_mainwindow.h"
#include <QUrlQuery>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QStandardItemModel>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    creatTableWidgets();
    m_network_manager = new QNetworkAccessManager(this);
    connect(m_network_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::creatTableWidgets()
{
    ui->tableWidget_fact->setColumnCount(3);
    ui->tableWidget_fact->setShowGrid(true);
    ui->tableWidget_fact->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_fact->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_fact->setHorizontalHeaderLabels(QStringList() << ("запасы")
                                                    << ("широта")
                                                    << ("долгота"));
    ui->tableWidget_fact->horizontalHeader()->setStretchLastSection(true);

    ui->tableWidget_shop->setColumnCount(6);
    ui->tableWidget_shop->setShowGrid(true);
    ui->tableWidget_shop->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_shop->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_shop->setHorizontalHeaderLabels(QStringList() << ("название")
                                                    << ("потребность")
                                                    << ("тариф 1")
                                                    << ("тариф 2")
                                                    << ("широта")
                                                    << ("долгота"));
    ui->tableWidget_shop->horizontalHeader()->setStretchLastSection(true);
}

bool is_number(QString s)
{
    if (s.toStdString() == "") return false;
    bool symb = false;
    for (auto x : s.toStdString()) {
        if (!((x-48)>=0 && (x-48)<=9))
            {
                if (x != '.') return false;
                else
                {
                    if (symb)
                        return false;
                    symb = true;
                }
            }
    }
    return true;
}

void MainWindow::on_pbn_fact_add_clicked()
{
    QString a = ui->lineEdit_fact_a->text();
    QString latitude = ui->lineEdit_fact_lat->text();
    QString longitude = ui->lineEdit_fact_long->text();
    if (is_number(a) && is_number(latitude) && is_number(longitude))
    {
        if (ui->tableWidget_fact->rowCount() < 2)
        {
            ui->tableWidget_fact->insertRow(ui->tableWidget_fact->rowCount());
            ui->tableWidget_fact->setItem(ui->tableWidget_fact->rowCount()-1, 0, new QTableWidgetItem(a));
            ui->tableWidget_fact->setItem(ui->tableWidget_fact->rowCount()-1, 1, new QTableWidgetItem(latitude));
            ui->tableWidget_fact->setItem(ui->tableWidget_fact->rowCount()-1, 2, new QTableWidgetItem(longitude));
        }
        else
            QMessageBox::warning(this, "Ошибка", "Максимально число фабрик: 2");

    }
    else
        QMessageBox::warning(this, "Ошибка", "Неверный формат входных данных");
}

void MainWindow::on_pbn_fact_del_clicked()
{
    for (int row = 0; row < ui->tableWidget_fact->rowCount(); ++row){
        if (ui->tableWidget_fact->model()->index(row, 0).data() == ui->lineEdit_fact_a->text() &&
            ui->tableWidget_fact->model()->index(row, 1).data() == ui->lineEdit_fact_lat->text() &&
            ui->tableWidget_fact->model()->index(row, 2).data() == ui->lineEdit_fact_long->text()){

            ui->tableWidget_fact->removeRow(row);
            break;
        }
    }
}

void MainWindow::on_pbn_shop_add_clicked()
{
    QString title = ui->lineEdit_shop_title->text();
    QString b = ui->lineEdit_shop_b->text();
    QString fr1 = ui->lineEdit_shop_fr1->text();
    QString fr2 = ui->lineEdit_shop_fr2->text();
    QString latitude = ui->lineEdit_shop_lat->text();
    QString longitude = ui->lineEdit_shop_long->text();
    if (title != "" && is_number(b) && is_number(fr1) && is_number(fr2) && is_number(latitude) && is_number(longitude))
    {
        ui->tableWidget_shop->insertRow(ui->tableWidget_shop->rowCount());
        ui->tableWidget_shop->setItem(ui->tableWidget_shop->rowCount()-1, 0, new QTableWidgetItem(title));
        ui->tableWidget_shop->setItem(ui->tableWidget_shop->rowCount()-1, 1, new QTableWidgetItem(b));
        ui->tableWidget_shop->setItem(ui->tableWidget_shop->rowCount()-1, 2, new QTableWidgetItem(fr1));
        ui->tableWidget_shop->setItem(ui->tableWidget_shop->rowCount()-1, 3, new QTableWidgetItem(fr2));
        ui->tableWidget_shop->setItem(ui->tableWidget_shop->rowCount()-1, 4, new QTableWidgetItem(latitude));
        ui->tableWidget_shop->setItem(ui->tableWidget_shop->rowCount()-1, 5, new QTableWidgetItem(longitude));
    }
    else
        QMessageBox::warning(this, "Ошибка", "Неверный формат входных данных");
}

void MainWindow::on_pbd_shop_del_clicked()
{
    for (int row = 0; row < ui->tableWidget_shop->rowCount(); ++row){
        if (ui->tableWidget_shop->model()->index(row, 0).data() == ui->lineEdit_shop_title->text() &&
            ui->tableWidget_shop->model()->index(row, 1).data() == ui->lineEdit_shop_b->text() &&
            ui->tableWidget_shop->model()->index(row, 2).data() == ui->lineEdit_shop_fr1->text() &&
            ui->tableWidget_shop->model()->index(row, 3).data() == ui->lineEdit_shop_fr2->text() &&
            ui->tableWidget_shop->model()->index(row, 4).data() == ui->lineEdit_shop_lat->text() &&
            ui->tableWidget_shop->model()->index(row, 5).data() == ui->lineEdit_shop_long->text()){

            ui->tableWidget_shop->removeRow(row);
            break;
        }
    }
}

QVector<QString> t;
QVector<double> dem, sup;
QVector<QVector<double>> cost;
int cnt = 0;

void MainWindow::finished(QNetworkReply * reply)
{
    if (m_reply != reply){
        reply->deleteLater();
        return;
    }
    m_reply = nullptr;
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        reply->deleteLater();
        return;
    }
    const QByteArray content = reply->readAll();
    const QJsonDocument doc = QJsonDocument::fromJson(content);
    if (!doc.isObject()) {
        qDebug() << "Error while reading the JSON file.";
        reply->deleteLater();
        return;
    }

    const QJsonObject obj = doc.object();
    const QJsonArray origins = obj.value("origin_addresses").toArray();
    const QJsonArray destinations = obj.value("destination_addresses").toArray();
    QString output;
    for (int i = 0; i < origins.count(); ++i){
        QVector<double> c;
        cost.push_back(c);
        const QString origin = origins.at(i).toString();
        const QJsonArray row = obj.value("rows").toArray().at(i).toObject().value("elements").toArray();
        for (int j = 0; j < destinations.count(); ++j){
            const QJsonObject data = row.at(j).toObject();
            const QString status = data.value("status").toString();

            cost[i].push_back(data.value("distance").toObject().value("value").toDouble()/1000.0 * ui->tableWidget_shop->model()->index(j, 2+i).data().toDouble());
            output += tr("Distance: %1\n").arg(data.value("distance").toObject().value("text").toString());

        }
    }

    for (int i = 0; i < ui->tableWidget_fact->rowCount(); ++i)
        sup.push_back(ui->tableWidget_fact->model()->index(i, 0).data().toDouble());
    for (int i = 0; i < ui->tableWidget_shop->rowCount(); ++i)
        dem.push_back(ui->tableWidget_shop->model()->index(i, 1).data().toDouble());
    for (int i = 0; i < ui->tableWidget_shop->rowCount(); ++i)
        t.push_back(ui->tableWidget_shop->model()->index(i, 0).data().toString());
    emit sendData(sup, dem, cost, t);
    reply->deleteLater();
}

void MainWindow::on_solve_clicked()
{
    if (ui->tableWidget_fact->rowCount()!=2) { QMessageBox::warning(this, "Ошибка", "Требуемое число фабрик - 2"); return; }

    double supply_sum = 0, demand_sum = 0;
    for (int i = 0; i < ui->tableWidget_fact->rowCount(); ++i) supply_sum += ui->tableWidget_fact->model()->index(i, 0).data().toDouble();
    for (int i = 0; i < ui->tableWidget_shop->rowCount(); ++i) demand_sum += ui->tableWidget_shop->model()->index(i, 1).data().toDouble();

    if (supply_sum < demand_sum) { QMessageBox::warning(this, "Ошибка", "Невозможно выполнить все запросы магазинов"); return; }

    emit solve_clicked();

    cost.clear();
    sup.clear();
    dem.clear();

    QString query = "https://maps.googleapis.com/maps/api/distancematrix/json?";
    query += "&origins=";
    for (int i = 0; i < ui->tableWidget_fact->rowCount(); ++i){
        if (i != 0) query += '|';
        query += (ui->tableWidget_fact->model()->index(i, 1).data().toString() + ", " + ui->tableWidget_fact->model()->index(i, 2).data().toString());
    }
    query += "&destinations=";
    for (int i = 0; i < ui->tableWidget_shop->rowCount(); ++i){
        if (i != 0) query += '|';
        query += (ui->tableWidget_shop->model()->index(i, 4).data().toString() + ", " + ui->tableWidget_shop->model()->index(i, 5).data().toString());
    }
    query += "&units=km&mode=driving&departure_time=now&key=AIzaSyCYQDtdRLCLGheHVnrgORF915KAHTi1R0I";
    QUrl url(query);

    SecondWindow *optimize = new SecondWindow();
    connect(this, &MainWindow::sendData, optimize, &SecondWindow::receiveData);
    connect(this, &MainWindow::solve_clicked, optimize, &SecondWindow::close);

    m_reply = m_network_manager->get(QNetworkRequest(url));

    optimize->show();
    optimize->exec();
}

void MainWindow::on_importCSVFile_clicked()
{
    using namespace std;

    QString data;
    QFile importedCSV(dir_name);
    QStringList rowOfData;
    QStringList rowData;
    int tempint = 0;
    data.clear();
    rowOfData.clear();
    rowData.clear();

    if (importedCSV.open(QFile::ReadOnly))
    {
        data = importedCSV.readAll();
        QString k = "";
        for (int i = 0; i < data.size()-1; ++i) {if (data[i] != '\"') k+= data[i];}
        if (data[data.size()-1] != '\n') k+=data[data.size()-1];
        data = k;
        rowOfData = data.split("\n");
        importedCSV.close();
    }
    for (int x = 0; x < rowOfData.size(); x++)
    {
        int cnt = 0;
        rowData = rowOfData.at(x).split(";");
        if (ui->tableWidget_fact->rowCount()<2){
            ui->tableWidget_fact->insertRow(ui->tableWidget_fact->rowCount());
            for (int y = 0; y < rowData.size(); y++)
            {
                ui->tableWidget_fact->setItem(x, y, new QTableWidgetItem(rowData[y]));
            }
        } else {
            ui->tableWidget_shop->insertRow(ui->tableWidget_shop->rowCount());
            for (int y = 0; y < rowData.size(); y++)
            {
                ui->tableWidget_shop->setItem(ui->tableWidget_shop->rowCount()-1, y, new QTableWidgetItem(rowData[y]));
            }
        }
        ++cnt;
    }
}


void MainWindow::on_pushSelectDir_clicked()
{
    QFileDialog directory;
    ui->lineEditDir->setText(directory.getOpenFileName(this,"Choose an directory"));
    dir_name = ui->lineEditDir->text();
}
