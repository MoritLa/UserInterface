#include "connectionmonitor.h"
#include "messages_header.h"
#include "constants.h"

#include <iostream>
#include <stdio.h>

using namespace std ;

#define HEADER      "\\\\.\\"

//-------Public functions
ConnectionMonitor::ConnectionMonitor()
{
    connect(&timer, SIGNAL(timeout()),this, SLOT(ReadBuffer())) ;
    StopReceiving() ;
}

void ConnectionMonitor::init(ConnectionScreen *extDisplay,
                             ReceiveCallback16FP speed_cb,
                             ReceiveCallback16FP torque_cb,
                             ReceiveCallback8FP  state_cb,
                             ReceiveCallback16FP angle_cb)
{
    Display = extDisplay ;
    extSpeed_cb = speed_cb ;
    extTorque_cb = torque_cb ;
    extState_cb = state_cb ;
    extAngle_cb = angle_cb ;
}

ConnectionMonitor::~ConnectionMonitor()
{
    StopReceiving() ;
}

void ConnectionMonitor::ConnectMotor(QString port)
{
    uint8_t out[2] = {TYPE_ID,TYPE_LENGTH} ;
    char in[4] ;

    QString port_head  = HEADER ;

    // force disconnection of port
    if(port == "0")
    {
        Display->changeButton(MOTOR,NOTCONNECTED) ;
        MotorConnected = NOTCONNECTED ;
        Motor.disconnect();
        return ;
    }

    // add prefix \\.\ to allow ports bigger than 9
    if(!port.contains(HEADER))
        port = port_head + port ;

    // initialise comport and connect to port when not connected
    Display->changeButton(MOTOR, CONNECTING) ;
    if(MotorConnected == NOTCONNECTED)
    {
        Motor.setBaudRate(MY_BAUD) ;
        Motor.setCommPort(port.toStdString()) ;
        if(Motor.connect())
        {
            Display->changeButton(MOTOR,NOTCONNECTED) ;
            MotorConnected = NOTCONNECTED ;

        }
        else
        {
            // verify type of the connected node
            Motor.sendBuffer((char*) out, 2) ;
            Motor.receiveBuffer(in,4) ;

            if(in[3] == MOTOR_TYPE)
            {
                Display->changeButton(MOTOR, CONNECTED) ;
                MotorConnected = CONNECTED ;
            }
            else
            {
                Motor.disconnect() ;
                Display->changeButton(MOTOR, NOTCONNECTED) ;
                MotorConnected = NOTCONNECTED ;
            }
        }
    }
    // disconnect when already connected
    else
    {
        if(!Motor.disconnect())
        {
            Display->changeButton(MOTOR, NOTCONNECTED) ;
            MotorConnected = NOTCONNECTED ;
        }
        else
        {
            Display->changeButton(MOTOR,CONNECTED) ;
            MotorConnected = CONNECTED ;
        }
    }
}

void ConnectionMonitor::ConnectSensor(QString port)
{
    uint8_t out[2] = {TYPE_ID,TYPE_LENGTH} ;
    char in[4] ;

    QString port_head  = HEADER ;

    // force disconnection of port
    if(port == "0")
    {
        Display->changeButton(MOTOR,NOTCONNECTED) ;
        MotorConnected = NOTCONNECTED ;
        Motor.disconnect();
        return ;
    }

    // add prefix \\.\ to allow ports bigger than 9
    if(!port.contains(HEADER))
        port = port_head + port ;

    // initialise comport and connect to port when not connected
    Display->changeButton(SENSOR, CONNECTING) ;
    if (SensorConnected == NOTCONNECTED)
    {
        Sensor.setBaudRate(MY_BAUD) ;
        Sensor.setCommPort(port.toStdString()) ;
        if(Sensor.connect())
        {
            Display->changeButton(SENSOR, NOTCONNECTED) ;
            SensorConnected = NOTCONNECTED ;
        }
        else
        {
            // verify type of the connected node
            Sensor.sendBuffer((char*) out, 2) ;
            Sensor.receiveBuffer(in,4) ;
            if(in[3] == SENSOR_TYPE)
            {
                Display->changeButton(SENSOR, CONNECTED) ;
                SensorConnected = CONNECTED ;
            }
            else
            {
                Sensor.disconnect() ;
                Display->changeButton(SENSOR, NOTCONNECTED) ;
                SensorConnected = NOTCONNECTED ;
            }
        }
    }
    // disconnect when already connected
    else
    {
        if(!Sensor.disconnect())
        {
            Display->changeButton(SENSOR, NOTCONNECTED) ;
            SensorConnected = NOTCONNECTED ;
        }
        else
        {
            Display->changeButton(SENSOR,CONNECTED) ;
            SensorConnected = CONNECTED ;
        }
    }
}

void ConnectionMonitor::ConnectECU(QString port)
{
    uint8_t out[2] = {TYPE_ID,TYPE_LENGTH} ;
    uint8_t in[4] ;
    int showIn[4] ;

    QString port_head  = HEADER ;

    // force disconnection of port
    if(port == "0")
    {
        Display->changeButton(MOTOR,NOTCONNECTED) ;
        MotorConnected = NOTCONNECTED ;
        Motor.disconnect();
        return ;
    }

    // add prefix \\.\ to allow ports bigger than 9
    if(!port.contains(HEADER))
        port = port_head + port ;

    // initialise comport and connect to port when not connected
    Display->changeButton(ECU, CONNECTING) ;
    if(EcuConnected == NOTCONNECTED)
    {
        Ecu.setBaudRate(MY_BAUD) ;
        Ecu.setCommPort(port.toStdString()) ;
        if(Ecu.connect())
        {
            Display->changeButton(ECU, NOTCONNECTED) ;
            EcuConnected = NOTCONNECTED ;
        }
        else
        {
            // verify type of the connected node
            Ecu.sendBuffer((char*) out, 2) ;
            Ecu.receiveBuffer((char*) in,4) ;
            showIn[0] = in[0]; showIn[1] = in[1]; showIn[2] = in[2]; showIn[3] = in[3];
            if(in[3] == ECU_TYPE)
            {
                Display->changeButton(ECU, CONNECTED) ;
                EcuConnected = CONNECTED ;
            }
            else
            {
                Ecu.disconnect() ;
                Display->changeButton(ECU, NOTCONNECTED) ;
                EcuConnected = NOTCONNECTED ;
            }
        }
    }
    // disconnect when already connected
    else
    {
        if(!Ecu.disconnect())
        {
            Display->changeButton(ECU, NOTCONNECTED) ;
            EcuConnected = NOTCONNECTED ;
        }
        else
        {
            Display->changeButton(ECU,CONNECTED) ;
            EcuConnected = CONNECTED ;
        }
    }
}

void ConnectionMonitor::StartReceiving(void)
{
    char output[3] ;
    if(!timer.isActive())
        timer.start(100) ;

    // request start of transmission of data from the three nodes
    output[0] = static_cast<char>(headers_S[MOTOR][MOTOR_CONFIG].id); output[1] = headers_S[MOTOR][MOTOR_CONFIG].length ;
    output[2] = START_OUTPUT;
    Motor.sendBuffer(output, HEADER_LENGTH+headers_S[MOTOR][MOTOR_CONFIG].length) ;

    output[0] = static_cast<char>(headers_S[SENSOR][SENSOR_CONFIG].id); output[1] = headers_S[SENSOR][MOTOR_CONFIG].length ;
    output[2] = START_OUTPUT;
    Sensor.sendBuffer(output, HEADER_LENGTH+headers_S[SENSOR][MOTOR_CONFIG].length) ;

    output[0] = static_cast<char>(headers_S[ECU][ECU_CONFIG].id); output[1] = headers_S[ECU][ECU_CONFIG].length ;
    output[2] = START_OUTPUT;
    Ecu.sendBuffer(output, HEADER_LENGTH+headers_S[ECU][ECU_CONFIG].length) ;
    timing.start();
}

void ConnectionMonitor::StopReceiving(void)
{
    char output[3] ;

    // request stop of transmission of data from the three nodes
    output[0] = static_cast<char>(headers_S[MOTOR][MOTOR_CONFIG].id); output[1] = headers_S[MOTOR][MOTOR_CONFIG].length ;
    output[2] = !START_OUTPUT;
    Motor.sendBuffer(output, HEADER_LENGTH+headers_S[MOTOR][MOTOR_CONFIG].length) ;

    output[0] = static_cast<char>(headers_S[SENSOR][SENSOR_CONFIG].id); output[1] = headers_S[SENSOR][SENSOR_CONFIG].length ;
    output[2] = !START_OUTPUT;
    Sensor.sendBuffer(output, HEADER_LENGTH+headers_S[SENSOR][SENSOR_CONFIG].length) ;

    output[0] = static_cast<char>(headers_S[ECU][ECU_CONFIG].id); output[1] = headers_S[ECU][ECU_CONFIG].length ;
    output[2] = !START_OUTPUT;
    Ecu.sendBuffer(output, HEADER_LENGTH+headers_S[ECU][ECU_CONFIG].length) ;

    timer.stop() ;
}

void ConnectionMonitor::UpdateMotor(double TrueSpeed, double TrueTorque)
{

    char output[MAX_UART_MESSAGE_LENGTH];
    uint16_t outdata ;
    //conversion not from float to uint8_t and to output format
    outdata = ((TrueSpeed*MAX_VAL)/V_MAX_MOTOR);
    output[0] = headers_S[MOTOR][TRUE_MOTOR_SPEED].id ; output[1] = headers_S[MOTOR][TRUE_MOTOR_SPEED].length ;
    output[2] = outdata; output[3] = outdata>>8;

    Motor.sendBuffer(output,HEADER_LENGTH+headers_S[MOTOR][TRUE_MOTOR_SPEED].length) ;

    outdata = ((TrueTorque*MAX_VAL)/M_MAX_MOTOR);
    output[0] = headers_S[MOTOR][TRUE_MOTOR_TORQUE].id ; output[1] = headers_S[MOTOR][TRUE_MOTOR_TORQUE].length ;
    output[2] = outdata>>8; output[3] = outdata;

    Motor.sendBuffer(output,HEADER_LENGTH+headers_S[MOTOR][TRUE_MOTOR_TORQUE].length) ;
}

void ConnectionMonitor::UpdateECU(void)
{
    // no messages for ECU
}

void ConnectionMonitor::UpdateSensor(double WheelSpeed[4], double Acceleration[3])
{
    char output[MAX_UART_MESSAGE_LENGTH+2];
    uint16_t outdata ;

    for(int i = 0; i<4;i++)
    {
        outdata = WheelSpeed[i]*MAX_VAL_2/V_MAX_WHEEL+MAX_VAL_2 ;
        output[2+2*i] = outdata; output[3+2*i] = outdata>>8 ;
    }
    output[0] = headers_S[SENSOR][TRUE_SPEED].id ; output[1] = headers_S[SENSOR][TRUE_SPEED].length ;
    Sensor.sendBuffer(output,2+headers_S[SENSOR][TRUE_SPEED].length) ;

    for(int i = 0; i<3;i++)
    {
        outdata = Acceleration[i]*MAX_VAL/A_MAX ;
        output[2+2*i] = outdata; output[3+2*i] = outdata>>8 ;
    }
    output[0] = headers_S[SENSOR][ACCELERATION].id ; output[1] = headers_S[SENSOR][ACCELERATION].length ;
    Sensor.sendBuffer(output,2+headers_S[SENSOR][ACCELERATION].length) ;

}

void ConnectionMonitor::send_Pedals(double breakPedal, double accelerationPedal)
{
    char output[MAX_UART_MESSAGE_LENGTH];
    uint16_t outdata ;
    //conversion not from float to uint8_t and to output format
    outdata = breakPedal*MAX_VAL;
    output[2] = outdata; output[3] = outdata>>8;
    outdata = accelerationPedal*MAX_VAL;
    output[4] = outdata; output[5] = outdata>>8;
    output[0] = headers_S[SENSOR][PEDALS].id ; output[1] = headers_S[SENSOR][PEDALS].length ;

    Sensor.sendBuffer(output,2+headers_S[SENSOR][PEDALS].length) ;

}

void ConnectionMonitor::send_steering(double steeringAngle)
{
    char output[MAX_UART_MESSAGE_LENGTH];
    uint16_t outdata ;
    //conversion not from float to uint8_t and to output format
    outdata = steeringAngle*MAX_VAL_2/STEERING_FACTOR+MAX_VAL_2 ;
    output[2] = outdata; output[3] = outdata>>8;
    output[0] = headers_S[SENSOR][STEERING].id ; output[1] = headers_S[SENSOR][STEERING].length ;

    Sensor.sendBuffer(output,2+headers_S[SENSOR][STEERING].length) ;
}

//--------Private functions
void ConnectionMonitor::ReadBuffer(void)
{
    char id[2] ;
    uint16_t data16 ;
    uint8_t data8 ;
    char length ;
    char data[64];
    int i =0;
    int time_elapsed ;
    time_elapsed = timing.elapsed() ;

    // catch all the messages in the input buffer of the motor port
    while(!Motor.receiveBuffer(id, 2))
    {
        Motor.receiveBuffer(&length,1) ;
        Motor.receiveBuffer(data,static_cast<unsigned int>(length)&0xFF) ;
        if ((static_cast<int>(id[1])&0xFF) != ID_HIGH)
            return ;
        switch (static_cast<int>(id[0])&0xFF)
        {
        case SPEED_SETPOINT_ID :

            if(length == headers_R[MOTOR][SPEED_SETPOINT].length)
            {
                data16 = (static_cast<int>(data[0])&0xFF)+((static_cast<int>(data[1])&0xFF)<<8) ;
                extSpeed_cb(data16);
            }
            break;
        case TORQUE_SETPOINT_ID:
            if(length == headers_R[MOTOR][TORQUE_SETPOINT].length)
            {
                data16 = (static_cast<int>(data[0])&0xFF)+((static_cast<int>(data[1])&0xFF)<<8) ;
                extTorque_cb(data16);
            } break;
        default: break;
        }
        // stop inifinite loops
        if (i>10)
            break ;
        i++;
    }


    i = 0 ;
    // catch all the messages in the input buffer of the sensor port
    while(!Sensor.receiveBuffer(id, 2))
    {
        Sensor.receiveBuffer(&length,1) ;
        Sensor.receiveBuffer(data,static_cast<unsigned int>(length)&0xFF) ;

        if ((static_cast<int>(id[1])&0xFF) != ID_HIGH)
            return ;

        switch (static_cast<int>(id[0])&0xFF)
        {
        case STEERING_ANGLE_ID:

            if(length == headers_R[SENSOR][STEERING_ANGLE].length)
            {
                data16 = (static_cast<int>(data[0])&0xFF)+((static_cast<int>(data[1])&0xFF)<<8) ;
                extAngle_cb(data16) ;
                cout<<"Sensor: "<<data16<<endl;
            }
            break;
        default: break;
        }
        // stop inifinite loops
        if (i>10)
            break ;
        i++;
    }

    i = 0;
    // catch all the messages in the input buffer of the motor port
    while(!Ecu.receiveBuffer(id, 2))
    {
        Ecu.receiveBuffer(&length,1) ;
        Ecu.receiveBuffer(data,static_cast<unsigned int>(length)&0xFF) ;

        if ((static_cast<int>(id[1])&0xFF) != ID_HIGH)
            return ;
        switch (static_cast<int>(id[0])&0xFF)
        {
        case SYSTEM_STATE_ID:
            if(length == headers_R[ECU][SYSTEM_STATE].length)
            {
                data8 = data[0] ;
                extState_cb(data8) ;
            } break;
        default: break;
        }
        // stop inifinite loops
        if (i>10)
            break ;
        i++;
    }
    std::cout<<"Timer: "<<timing.elapsed() - time_elapsed<<endl;
    time_elapsed =timing.elapsed() ;
}
