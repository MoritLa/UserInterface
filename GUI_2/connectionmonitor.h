#ifndef CONNECTIONMONITOR_H
#define CONNECTIONMONITOR_H

#include "serialport/serialport.h"
#include "connectionscreen.h"

#include <stdint.h>

#include <QString>
#include <QTimer>
#include <QTime>

typedef void (*ReceiveCallback16FP)(uint16_t data) ;
typedef void (*ReceiveCallback8FP)(uint8_t data) ;

class ConnectionMonitor: public QObject
{
    Q_OBJECT

private:
    QTimer timer ;
    QTime timing ;
    ConnectionScreen * Display ;
    SerialPort Motor, Sensor, Ecu ;
    bool treatData[3][3] ;
    bool doReadOut[3] ;
    bool MotorConnected = NOTCONNECTED, SensorConnected =NOTCONNECTED, EcuConnected = NOTCONNECTED ;

    ReceiveCallback16FP extSpeed_cb ;
    ReceiveCallback16FP extTorque_cb ;
    ReceiveCallback8FP  extState_cb ;
    ReceiveCallback16FP extAngle_cb ;

private slots:
    void ReadBuffer(void) ;
public:
    ConnectionMonitor();
    ~ConnectionMonitor() ;

    void init(ConnectionScreen *extDisplay,
              ReceiveCallback16FP speed_cb,
              ReceiveCallback16FP torque_cb,
              ReceiveCallback8FP state_cb,
              ReceiveCallback16FP angle_cb) ;

    // connect to port, when the node is the same as the function, the program stays connected
    void ConnectMotor(QString port) ;
    void ConnectSensor(QString port) ;
    void ConnectECU(QString port) ;

    // send start/stop message to nodes and start timer for receiving
    void StartReceiving(void) ;
    void StopReceiving(void) ;

    // send updated simulation data
    void UpdateMotor(double TrueSpeed, double TrueTorque) ;
    void UpdateECU(void) ;
    void UpdateSensor(double WheelSpeed[4], double Acceleration[3]) ;

    // send information from cockpit screen
    void send_Pedals(double breakPedal, double accelerationPedal) ;
    void send_steering(double steeringAngle) ;
};

#endif // CONNECTIONMONITOR_H
