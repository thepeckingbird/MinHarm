#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

int calcHp(int baseHP, int investedEvs);
int calcDef(int baseDef, int investedEvs, bool nature);
int calcSpDef(int baseSpDef, int investedEvs, bool nature);
double calcHarm(int hp, int def, int spDef, float bias);
void calcSpread(int spareEvs, int hp, int def, int spDef, float bias, bool defType);
void calcSpread(int spareEvs, int hp, int def, int spDef, float bias);
int* opSpread = new int[3];
double resHarm;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    opSpread[0] = 0;
    opSpread[1] = 0;
    opSpread[2] = 0;
    resHarm = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}
//Calculates HP stat:
int calcHp(int baseHP,int investedEvs){
    return (((31+(2*baseHP)+(investedEvs/4)+100)*50)/100)+10;
}
//Calculates Defence stat:
int calcDef(int baseDef, int investedEvs, bool nature){
    if (nature){
        return (((((31+(2*baseDef)+(investedEvs/4))*50)/100)+5)*1.1);
    }
    else{
        return (((31+(2*baseDef)+(investedEvs/4))*50)/100)+5;
    }
}
//Calculates Special Defence stat:
int calcSpDef(int baseSpDef, int investedEvs, bool nature){
    if (nature){
        return (((((31+(2*baseSpDef)+(investedEvs/4))*50)/100)+5)*1.1);
    }
    else{
        return (((31+(2*baseSpDef)+(investedEvs/4))*50)/100)+5;
    }
}
//Calculates the overall harm, when hit with 20000 damage:
double calcHarm(int hp, int def, int spDef, float bias){
    return ((2 - (2*bias))*((double)(20000 + (2*def))/(hp*def))) + ((2*bias)*((double)(20000+(2*spDef))/(hp*spDef)));
}
//Calculates the optimum EV spread:
void calcSpread(int spareEvs, int hp, int def, int spDef, float bias, bool defType){
    double overallHarm = 9001;
    double bestOverallHarm = 9000;
    int opHp = 0;
    int opDef = 0;
    int opSpDef = 0;
    for (int j = 0; j < 256; (j += 4)){
        for (int k = 0; k < 256; (k+=4)){
            for (int l = 0; l < 256; (l+=4)){
                if ((j+k+l) == (spareEvs - (spareEvs%4))){
                    overallHarm = calcHarm(calcHp(hp,j),calcDef(def,k,defType),calcSpDef(spDef,l,(!defType)),bias);
                    if (overallHarm < bestOverallHarm){
                        opHp = j;
                        opDef = k;
                        opSpDef = l;
                        bestOverallHarm = overallHarm;
                        resHarm = overallHarm;
                    }
                }
            }
        }
    }
    opSpread[0] = opHp;
    opSpread[1] = opDef;
    opSpread[2] = opSpDef;
}
//Calculates the spread if the nature is irrelevant:
void calcSpread(int spareEvs, int hp, int def, int spDef, float bias){
    double overallHarm = 9001;
    double bestOverallHarm = 9000;
    int opHp = 0;
    int opDef = 0;
    int opSpDef = 0;
    for (int j = 0; j < 256; (j += 4)){
        for (int k = 0; k < 256; (k += 4)){
            for (int l = 0; l < 256; (l += 4)){
                if ((j+k+l) == (spareEvs - (spareEvs%4))){
                    overallHarm = calcHarm(calcHp(hp,j),calcDef(def,k,false),calcSpDef(spDef,l,false),bias);
                    if (overallHarm < bestOverallHarm){
                        opHp = j;
                        opDef = k;
                        opSpDef = l;
                        bestOverallHarm = overallHarm;
                        resHarm = overallHarm;
                    }
                }
            }
        }
    }
    opSpread[0] = opHp;
    opSpread[1] = opDef;
    opSpread[2] = opSpDef;
}
//Sets the cogs into gear:
void MainWindow::on_btnGo_clicked()
{
    //Resets the result min harm:
    resHarm = 0;
    //Checks if nature is relevant:
    if(ui->cbxNature->isChecked()){
        //Executes calcs for defensive nature:
        if(ui->rbtDef->isChecked()){
            calcSpread(ui->sbxSpareEvs->value(),ui->sbxHP->value(),ui->sbxDef->value(),ui->sbxSpDef->value(),((float)ui->hrsDefType->value()/100),true);
            ui->lblResHPStat->setText(QString::number(calcHp(ui->sbxHP->value(),opSpread[0])));
            ui->lblResDefStat->setText(QString::number(calcDef(ui->sbxDef->value(),opSpread[1],true)));
            ui->lblResSpDefStat->setText(QString::number(calcSpDef(ui->sbxSpDef->value(),opSpread[2],false)));
        }
        //Executes calcs for spDefensive nature:
        else{
            calcSpread(ui->sbxSpareEvs->value(),ui->sbxHP->value(),ui->sbxDef->value(),ui->sbxSpDef->value(),((float)ui->hrsDefType->value()/100),false);
            ui->lblResHPStat->setText(QString::number(calcHp(ui->sbxHP->value(),opSpread[0])));
            ui->lblResDefStat->setText(QString::number(calcDef(ui->sbxDef->value(),opSpread[1],false)));
            ui->lblResSpDefStat->setText(QString::number(calcSpDef(ui->sbxSpDef->value(),opSpread[2],true)));
        }
    }
    else{
        //Executes calcs for irrelevant nature:
        calcSpread(ui->sbxSpareEvs->value(),ui->sbxHP->value(),ui->sbxDef->value(),ui->sbxSpDef->value(),((float)ui->hrsDefType->value()/100));
        ui->lblResHPStat->setText(QString::number(calcHp(ui->sbxHP->value(),opSpread[0])));
        ui->lblResDefStat->setText(QString::number(calcDef(ui->sbxDef->value(),opSpread[1],false)));
        ui->lblResSpDefStat->setText(QString::number(calcSpDef(ui->sbxSpDef->value(),opSpread[2],false)));
    }
    ui->lblResHP->setText(QString::number(opSpread[0]));
    ui->lblResDef->setText(QString::number(opSpread[1]));
    ui->lblResSpDef->setText(QString::number(opSpread[2]));
    ui->lblResHarm->setText(QString::number(resHarm));
}
//Toggles having a defensive nature:
void MainWindow::on_cbxNature_clicked()
{
    if (ui->cbxNature->isChecked()){
        ui->rbtDef->setEnabled(true);
        ui->rbtSpDef->setEnabled(true);
    }
    else{
        ui->rbtDef->setEnabled(false);
        ui->rbtSpDef->setEnabled(false);
    }
}

void MainWindow::on_hrsDefType_valueChanged(int value)
{
    ui->lblResBias->setText(QString::number((float)value/100));
}
