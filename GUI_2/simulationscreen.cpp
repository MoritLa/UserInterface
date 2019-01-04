#include "simulationscreen.h"
#include "ui_simulationscreen.h"

#include "constants.h"
#include <iostream>

simulationScreen::simulationScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::simulationScreen)
{
    ui->setupUi(this);

//  set list title
    ui->VSI->setText("VSI") ;
    ui->batterie->setText(("Batterie")) ;
    ui->wheel_fl->setText("front left") ;
    ui->wheel_fr->setText("front right") ;
    ui->wheel_rl->setText("rear left") ;
    ui->wheel_rr->setText("rear right") ;
}

simulationScreen::~simulationScreen()
{
    delete ui;
}

void simulationScreen::UpdateScreen(double WheelSpeed[4],double WheelTorque[4], double MotorSpeed,double MotorTroque,double StateOfCharge)
{
    QString buffer ;

    buffer.sprintf("%.3lf kWh\n", StateOfCharge/60000) ;
    ui->batterie_charge->setText(buffer);

    buffer.sprintf("%.2lf rpm\n", WheelSpeed[FL]) ;
    ui->speed_fl->setText(buffer) ;
    buffer.sprintf("%.2lf rpm\n", WheelSpeed[FR]) ;
    ui->speed_fr->setText(buffer) ;
    buffer.sprintf("%.2lf rpm\n", WheelSpeed[RL]) ;
    ui->speed_rl->setText(buffer) ;
    buffer.sprintf("%.2lf rpm\n", WheelSpeed[RR]) ;
    ui->speed_rr->setText(buffer) ;

    buffer.sprintf("%.2lf Nm",WheelTorque[FL]);
    ui->breakPw_fl->setText(buffer) ;
    buffer.sprintf("%.2lf Nm",WheelTorque[FR]);
    ui->breakPw_fr->setText(buffer) ;
    buffer.sprintf("%.2lf Nm",WheelTorque[RL]);
    ui->breakPw_rl->setText(buffer) ;
    buffer.sprintf("%.2lf Nm",WheelTorque[RR]);
    ui->breakPw_rr->setText(buffer) ;

    buffer.sprintf("%.2lf rpm\n", MotorSpeed) ;
    ui->speed_motor->setText(buffer) ;
    buffer.sprintf(("%.2lf Nm\n"),MotorTroque) ;
    ui->torque->setText(buffer) ;
}

void simulationScreen::setSteeringAngle(double angle)
{
    QString buffer ;

    buffer.sprintf("%.2lf\xB0\n",angle*180/PI) ;
    ui->angle_fl->setText(buffer) ;
    ui->angle_fr->setText(buffer) ;
}
