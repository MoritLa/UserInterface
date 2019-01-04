#include "connectionscreen.h"
#include "ui_connectionscreen.h"

#include "messages_header.h"

#include <iostream>

ConnectionScreen::ConnectionScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConnectionScreen)
{
    ui->setupUi(this);

}

ConnectionScreen::~ConnectionScreen()
{
    delete ui;
}
 // Callback functions for text entered
void ConnectionScreen::motor_cb(void)
{
    extMotor_cb(this->ui->MotorPort->text()) ;
}
void ConnectionScreen::sensor_cb(void)
{
    extSensor_cb(this->ui->SensorPort->text()) ;
}
void ConnectionScreen::ECU_cb(void)
{
    extECU_cb(this->ui->ECUPort->text()) ;
}

// initialise callback Functions
void ConnectionScreen::setCallbacks(ConnectionCallbackFP extMotor, ConnectionCallbackFP extSensor, ConnectionCallbackFP extECU)
{
    extMotor_cb = extMotor ;
    extSensor_cb = extSensor ;
    extECU_cb = extECU ;
}

// set button text
void ConnectionScreen::changeButton(int device, int state)
{
    switch (device)
    {
    case MOTOR  :
        switch (state)
        {
        case CONNECTED:     ui->ConnectMotor->setText("Disconnect") ;   break ;
        case CONNECTING:    ui->ConnectMotor->setText("...") ;          break ;
        case NOTCONNECTED:  ui->ConnectMotor->setText("Connect") ;      break ;
        }break ;
    case SENSOR :
        switch (state)
        {
        case CONNECTED:     ui->ConnectSensor->setText("Disconnect") ;   break ;
        case CONNECTING:    ui->ConnectSensor->setText("...") ;          break ;
        case NOTCONNECTED:  ui->ConnectSensor->setText("Connect") ;      break ;
        }break ;
    case ECU    :
        switch (state)
        {
        case CONNECTED:     ui->ConnectECU->setText("Disconnect") ;   break ;
        case CONNECTING:    ui->ConnectECU->setText("...") ;          break ;
        case NOTCONNECTED:  ui->ConnectECU->setText("Connect") ;      break ;
        }break ;
    default     : break ;
    }
}
