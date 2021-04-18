#include "solution.h"
#include "ui_solution.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <numeric>
#include <string>
#include <vector>
#include <cmath>

Solution::Solution(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Solution)
{
    ui->setupUi(this);
}

Solution::~Solution()
{
    delete ui;
}

using namespace std;

class Shipment {
public:
    double costPerUnit;
    int r, c;
    double quantity;

    Shipment() : quantity(0), costPerUnit(0), r(-1), c(-1) {
        // empty
    }

    Shipment(double q, double cpu, int r, int c) : quantity(q), costPerUnit(cpu), r(r), c(c) {
        // empty
    }

    friend bool operator==(const Shipment &lhs, const Shipment &rhs) {
        return lhs.costPerUnit == rhs.costPerUnit
               && lhs.quantity == rhs.quantity
               && lhs.r == rhs.r
               && lhs.c == rhs.c;
    }

    friend bool operator!=(const Shipment &lhs, const Shipment &rhs) {
        return !(lhs == rhs);
    }

    static Shipment ZERO;
};
Shipment Shipment::ZERO = {};

vector<int> demand, supply;
vector<vector<double>> costs;
vector<vector<Shipment>> matrix;

void Solution::receiveData(QVector<int> sup, QVector<int> dem, QVector< QVector<double> > cost, QVector<QString> title)
{
    for (auto x : sup) qDebug() << x;
    qDebug() << '\n';
    for (auto x : dem) qDebug() << x;
    qDebug() << '\n';
    for (auto cost : cost)
        for (auto c : cost)
            qDebug() << c;



    size_t numSources = sup.size(), numDestinations = dem.size();

    vector<int> src, dst;
    int t;

    for (size_t i = 0; i < numSources; i++) {
        src.push_back(sup[i]);
    }

    for (size_t i = 0; i < numDestinations; i++) {
        dst.push_back(dem[i]);
    }

    // fix imbalance
    int totalSrc = accumulate(src.cbegin(), src.cend(), 0);
    int totalDst = accumulate(dst.cbegin(), dst.cend(), 0);
    if (totalSrc > totalDst) {
        dst.push_back(totalSrc - totalDst);
    } else if (totalDst > totalSrc) {
        src.push_back(totalDst - totalSrc);
    }

    supply = src;
    demand = dst;

    costs.clear();
    matrix.clear();

    double d;
    for (size_t i = 0; i < numSources; i++) {
        size_t cap = max(numDestinations, demand.size());

        vector<double> dt(cap);
        vector<Shipment> st(cap);
        for (size_t j = 0; j < numDestinations; j++) {
            d = cost[i][j];
            dt[j] = d;
        }
        costs.push_back(dt);
        matrix.push_back(st);
    }
    for (size_t i = numSources; i < supply.size(); i++) {
        size_t cap = max(numDestinations, demand.size());

        vector<Shipment> st(cap);
        matrix.push_back(st);

        vector<double> dt(cap);
        costs.push_back(dt);
    }
    northWestCornerRule();
    steppingStone();
    showResult();
}

void Solution::northWestCornerRule() {
    int northwest = 0;
    for (size_t r = 0; r < supply.size(); r++) {
        for (size_t c = northwest; c < demand.size(); c++) {
            int quantity = min(supply[r], demand[c]);
            if (quantity > 0) {
                matrix[r][c] = Shipment(quantity, costs[r][c], r, c);

                supply[r] -= quantity;
                demand[c] -= quantity;

                if (supply[r] == 0) {
                    northwest = c;
                    break;
                }
            }
        }
    }
}

vector<Shipment> matrixToVector() {
    vector<Shipment> result;
    for (auto &row : matrix) {
        for (auto &s : row) {
            if (s != Shipment::ZERO) {
                result.push_back(s);
            }
        }
    }
    return result;
}

vector<Shipment> getNeighbors(const Shipment &s, const vector<Shipment> &lst) {
    vector<Shipment> nbrs(2);
    for (auto &o : lst) {
        if (o != s) {
            if (o.r == s.r && nbrs[0] == Shipment::ZERO) {
                nbrs[0] = o;
            } else if (o.c == s.c && nbrs[1] == Shipment::ZERO) {
                nbrs[1] = o;
            }
            if (nbrs[0] != Shipment::ZERO && nbrs[1] != Shipment::ZERO) {
                break;
            }
        }
    }
    return nbrs;
}

vector<Shipment> getClosedPath(const Shipment &s) {
    auto path = matrixToVector();
    path.insert(path.begin(), s);

    // remove (and keep removing) elements that do not have a
    // vertical AND horizontal neighbor
    size_t before;
    do {
        before = path.size();
        path.erase(
                remove_if(
                        path.begin(), path.end(),
                        [&path](Shipment &ship) {
                            auto nbrs = getNeighbors(ship, path);
                            return nbrs[0] == Shipment::ZERO || nbrs[1] == Shipment::ZERO;
                        }),
                path.end());
    } while (before != path.size());

    // place the remaining elements in the correct plus-minus order
    vector<Shipment> stones(path.size());
    fill(stones.begin(), stones.end(), Shipment::ZERO);
    auto prev = s;
    for (size_t i = 0; i < stones.size(); i++) {
        stones[i] = prev;
        prev = getNeighbors(prev, path)[i % 2];
    }
    return stones;
}

void Solution::fixDegenerateCase() {
    double eps = 10e-10;
    if (supply.size() + demand.size() - 1 != matrixToVector().size()) {
        for (size_t r = 0; r < supply.size(); r++) {
            for (size_t c = 0; c < demand.size(); c++) {
                if (matrix[r][c] == Shipment::ZERO) {
                    Shipment dummy(eps, costs[r][c], r, c);
                    if (getClosedPath(dummy).empty()) {
                        matrix[r][c] = dummy;
                        return;
                    }
                }
            }
        }
    }
}

void Solution::steppingStone() {
    double maxReduction = 0;
    vector<Shipment> move;
    Shipment leaving;
    bool isNull = true;

    fixDegenerateCase();

    for (size_t r = 0; r < supply.size(); r++) {
        for (size_t c = 0; c < demand.size(); c++) {
            if (matrix[r][c] != Shipment::ZERO) {
                continue;
            }

            Shipment trial(0, costs[r][c], r, c);
            vector<Shipment> path = getClosedPath(trial);

            double reduction = 0;
            double lowestQuantity = INT32_MAX;
            Shipment leavingCandidate;

            bool plus = true;
            for (auto &s : path) {
                if (plus) {
                    reduction += s.costPerUnit;
                } else {
                    reduction -= s.costPerUnit;
                    if (s.quantity < lowestQuantity) {
                        leavingCandidate = s;
                        lowestQuantity = s.quantity;
                    }
                }
                plus = !plus;
            }
            if (reduction < maxReduction) {
                isNull = false;
                move = path;
                leaving = leavingCandidate;
                maxReduction = reduction;
            }
        }
    }

    if (!isNull) {
        double q = leaving.quantity;
        bool plus = true;
        for (auto &s : move) {
            s.quantity += plus ? q : -q;
            matrix[s.r][s.c] = s.quantity == 0 ? Shipment::ZERO : s;
            plus = !plus;
        }
        steppingStone();
    }
}

void Solution::showResult() {

    ui->tableWidget_result->setColumnCount(3);
    ui->tableWidget_result->setShowGrid(true);
    ui->tableWidget_result->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_result->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_result->setHorizontalHeaderLabels(QStringList() << ("от фабрики 1")
                                                    << ("от фабрики 2")
                                                    << ("затраты"));
    ui->tableWidget_result->horizontalHeader()->setStretchLastSection(true);

    qDebug() << "Optimal solution ";
    double totalCosts = 0.0;
    for (size_t c = 0; c < demand.size(); c++) {
        ui->tableWidget_result->insertRow(ui->tableWidget_result->rowCount());
        ui->tableWidget_result->setItem(ui->tableWidget_result->rowCount()-1, 0, new QTableWidgetItem(QString("%1").arg(trunc(matrix[0][c].quantity*100)/100)));
        ui->tableWidget_result->setItem(ui->tableWidget_result->rowCount()-1, 1, new QTableWidgetItem(QString("%1").arg(trunc(matrix[1][c].quantity*100)/100)));
        ui->tableWidget_result->setItem(ui->tableWidget_result->rowCount()-1, 2, new QTableWidgetItem(QString("%1").arg(trunc((matrix[0][c].quantity*matrix[0][c].costPerUnit+matrix[1][c].quantity*matrix[1][c].costPerUnit)*100)/100)));
        ui->tableWidget_result->resizeColumnsToContents();
        ui->tableWidget_result->item(ui->tableWidget_result->rowCount()-1, 0)->data(Qt::DisplayRole).toDouble();

        totalCosts += trunc((matrix[0][c].quantity*matrix[0][c].costPerUnit+matrix[1][c].quantity*matrix[1][c].costPerUnit)*100)/100;
    }
    /*for (size_t r = 0; r < supply.size(); r++) {
        string s = "";
        for (size_t c = 0; c < demand.size(); c++) {
            auto s = matrix[r][c];
            if (s != Shipment::ZERO && s.r == r && s.c == c) {
                qDebug() << s.quantity;
                totalCosts += s.quantity * s.costPerUnit;
            } else {
                qDebug() << 0;
            }
        }
    }*/
    ui->label_result->setText("Минимальная стоимость доставки: " + QString("%1").arg(totalCosts));
    qDebug() << "\nTotal costs: " << totalCosts << "";
}


void Solution::on_back_clicked()
{
    this->close();
}
