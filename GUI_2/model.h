#ifndef MODEL_H
#define MODEL_H

#include <QTimer>
#include "connectionmonitor.h"
#include "simulationscreen.h"
#include "cockpitscreen.h"

#include <stdint.h>

class Model: public QObject
{
    Q_OBJECT

    ConnectionMonitor *OutputInterface ;
    simulationScreen * OutputWindow ;
    CockpitScreen * InputWindow ;
    QTimer timer ;

    bool simulate = 0 ;

    // Model parameters
    double TorqueSetpoint, TrueTorque ; //[Nm]
    double SpeedSetpoint, TrueSpeed ; //[rpm]
    double acceleration[3] ; //[m/s^2]
    double netSpeed ;   //[m/s]
    double steeringAngle ; //[rad] (right positive)
    double breakPedal ; //[-]
    double acceleratorPedal;//[-]
    double WheelSpeed[4] ; //[rpm]
    double WheelTorque[4] ;
    double OutputPower ;
    double StateOfCharge ;

    // Modle functions
    // calculates the new torque after one DELTA_T
    void NewTorque(void) ;
    // calculates the new acceleration and speed of the car
    void NewAcceleration(void) ;
    // calculates the new speed af the four wheels
    void NewWheelSpeed(void) ;

private slots:
    // update model by one step
    void UpdateStep(void) ;

public:
    Model(ConnectionMonitor *outputEmitter,
          simulationScreen *outputScreen,
          CockpitScreen *inputScreen);
    // initialise the simulation
    void init(void) ;
    // pause and run the simulation depending on the current state of the simulation
    void pause(void) ;

    // set private variables
    void setBreakPedal(int newBreakPedal) ;
    void setAccelerationPedal(int newAccelerationPedal) ;
    void setSteeringAngle(int newSteeringAngle) ;

    void setSpeedSetpoint(uint16_t speedSetpoint);
    void setTorqueSetpoint(uint16_t torqueSetpoint) ;
};



#endif // MODEL_H
