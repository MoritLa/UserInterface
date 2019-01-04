#include "simulationscreen.h"
#include "cockpitscreen.h"
#include "connectionscreen.h"
#include "connectionmonitor.h"

#include "model.h"
#include "constants.h"

#include <iostream>
#include <stdint.h>
#include <QApplication>
#include <QThread>
#include <QObject>

using namespace std ;

namespace
{
    static simulationScreen *SimulationScreen ;
    static CockpitScreen *Cockpit ;
    static ConnectionScreen *Connection ;

    static ConnectionMonitor *EPuckMonitor ;

    static Model *Simulation ;

    static bool UseEpuck ;
}

// callback functions
void motor_cb(QString port)
{
    EPuckMonitor->ConnectMotor(port) ;
}

void sensor_cb(QString port)
{
    EPuckMonitor->ConnectSensor(port) ;
}

void ECU_cb(QString port)
{
    EPuckMonitor->ConnectECU(port) ;
}

void startStop_cb(void)
{
    Simulation->pause() ;
}

void restart_cb(void)
{
    Simulation->init() ;
}

void driveMode_cb(int Mode)
{
    //send to sensor unit ??
}

void break_cb(int BreakPedal)
{
    Simulation->setBreakPedal(BreakPedal) ;
}

void breakBalance_cb(int BreakBalance)
{
    //sned to sensor
}

void accelerator_cb(int AcceleratorPedal)
{
    Simulation->setAccelerationPedal(AcceleratorPedal) ;
}

void cruise_cb(int MaxSpeed)
{
    //send to sensor unit??
}

void drive_cb(void)
{
    //Flag to sensor unit
}

void TS_cb(void)
{
    //Flag to Sensor unit
}

void useEpuck_cb(int use)
{
    UseEpuck = static_cast<bool>(use) ;
}

void steering_cb(int SteeringAngle)
{
    SteeringAngle = SteeringAngle*MAX_VAL/MAX_STEERING_WHEEL;
    if(!UseEpuck)
        Simulation->setSteeringAngle(SteeringAngle) ;
}

void speed_cb(uint16_t data)
{
    Simulation->setSpeedSetpoint(data) ;
}

void torque_cb(uint16_t data)
{
    Simulation->setTorqueSetpoint(data) ;
}

void state_cb(uint8_t data)
{
    //show state
}

void angle_cb(uint16_t data)
{
    if(UseEpuck)
        Simulation->setSteeringAngle(data) ;
}

void quit_cb(void)
{
    EPuckMonitor->StopReceiving() ;

    QApplication::quit() ;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SimulationScreen = new simulationScreen ;
    Cockpit = new CockpitScreen;
    Connection = new ConnectionScreen ;
    EPuckMonitor = new ConnectionMonitor ;

    Simulation = new Model(EPuckMonitor,
                           SimulationScreen,
                           Cockpit) ;
    Cockpit->setCallbacks(startStop_cb,
                          restart_cb,
                          driveMode_cb,
                          break_cb,
                          breakBalance_cb,
                          accelerator_cb,
                          cruise_cb,
                          drive_cb,
                          TS_cb,
                          useEpuck_cb,
                          steering_cb,
                          quit_cb) ;
    Connection->setCallbacks(motor_cb, sensor_cb, ECU_cb) ;



    EPuckMonitor->init(Connection,
                       speed_cb,
                       torque_cb,
                       state_cb,
                       angle_cb) ;

    SimulationScreen->show() ;
    Cockpit->show() ;
    Connection->show() ;

    return a.exec();

}


