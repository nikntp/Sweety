#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void creatTableWidgets();

private slots:
    void on_pbn_fact_add_clicked();

    void on_pbn_shop_add_clicked();

    void on_pbn_fact_del_clicked();

    void on_pbd_shop_del_clicked();

    void finished(QNetworkReply * reply);

    void on_solve_clicked();


    //void receiveData(int s);

signals:
    void sendData(QVector<int> supply, QVector<int> demand, QVector< QVector<double> > costs, QVector<QString> title);

    void solve_clicked();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *m_network_manager;
    QNetworkReply *m_reply;
};
#endif // MAINWINDOW_H
