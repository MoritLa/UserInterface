#ifndef MESSAGES_HEADER_H
#define MESSAGES_HEADER_H

#include <stdint.h>

#define START_OUTPUT    (1<<0)
#define DO_READOUT      (1<<4)
#define INIT_ID        0x81
#define INIT_LENGTH     1

#define TYPE_ID         0x80
#define TYPE_LENGTH     0

#define HEADER_LENGTH   2

typedef struct UART_Receive
{
    uint8_t id[2] ;
    uint8_t length ;
} ReceiveFrame ;

typedef struct UART_Send
{
    uint8_t id;
    uint8_t length ;
} SendFrame;



enum elements{ECU,MOTOR,SENSOR} ;
enum ECUMessagesR{SYSTEM_STATE} ;
enum ECUMessagesS{ECU_CONFIG,PARAM} ;
enum MotorMessagesR{SPEED_SETPOINT, TORQUE_SETPOINT } ;
enum MotorMessagesS{MOTOR_CONFIG, TRUE_MOTOR_SPEED,TRUE_MOTOR_TORQUE} ;
enum SensorMessagesR{STEERING_ANGLE}  ;
enum SensorMessagesS{SENSOR_CONFIG, TRUE_SPEED, ACCELERATION, PEDALS,BUTTONS,ENVIRONMENT, STEERING};

#define ID_HIGH         0x80
enum ECUReceivIdLow{SYSTEM_STATE_ID = 0xA2} ;
enum MotorReceiveIdLow{SPEED_SETPOINT_ID = 0xC9, TORQUE_SETPOINT_ID =0xCD} ;
enum SensorReceiveIdLow{STEERING_ANGLE_ID=0xE7};

static UART_Receive headers_R[3][8] = {
    {{{headers_R[ECU][SYSTEM_STATE].id[0] = SYSTEM_STATE_ID,headers_R[ECU][SYSTEM_STATE].id[1] = ID_HIGH}, headers_R[ECU][SYSTEM_STATE].length = 1}},

    {{{headers_R[MOTOR][SPEED_SETPOINT].id[0] = SPEED_SETPOINT_ID, headers_R[MOTOR][SPEED_SETPOINT].id[1] = ID_HIGH}, headers_R[MOTOR][SPEED_SETPOINT].length = 2},
    {{headers_R[MOTOR][TORQUE_SETPOINT].id[0] = TORQUE_SETPOINT_ID, headers_R[MOTOR][TORQUE_SETPOINT].id[1] = ID_HIGH}, headers_R[MOTOR][TORQUE_SETPOINT].length = 2}},

    {{{headers_R[SENSOR][STEERING_ANGLE].id[0] = STEERING_ANGLE_ID, headers_R[SENSOR][STEERING_ANGLE].id[1] = ID_HIGH}, headers_R[SENSOR][STEERING_ANGLE].length = 2}}
} ;

static UART_Send headers_S[3][8] = {
    {{headers_S[ECU][ECU_CONFIG].id = 0xA0, headers_S[ECU][ECU_CONFIG].length = 1},
    {headers_S[ECU][PARAM].id = 0xA1, headers_S[ECU][PARAM].length = 1 }},

    {{headers_S[MOTOR][MOTOR_CONFIG].id = 0xC0, headers_S[MOTOR][MOTOR_CONFIG].length = 1},
    {headers_S[MOTOR][TRUE_MOTOR_SPEED].id = 0xC1, headers_S[MOTOR][TRUE_MOTOR_SPEED].length = 2},
    {headers_S[MOTOR][TRUE_MOTOR_TORQUE].id = 0xC5, headers_S[MOTOR][TRUE_MOTOR_TORQUE].length = 2}},

    {{headers_S[SENSOR][SENSOR_CONFIG].id = 0xE0, headers_S[SENSOR][SENSOR_CONFIG].length = 1},
    {headers_S[SENSOR][TRUE_SPEED].id = 0xE1, headers_S[SENSOR][TRUE_SPEED].length = 8},
    {headers_S[SENSOR][ACCELERATION].id = 0xE2, headers_S[SENSOR][ACCELERATION].length = 6},
    {headers_S[SENSOR][PEDALS].id = 0xE3, headers_S[SENSOR][PEDALS].length = 4},
    {headers_S[SENSOR][BUTTONS].id = 0xE4, headers_S[SENSOR][BUTTONS].length = 1},
    {headers_S[SENSOR][ENVIRONMENT].id = 0xE5, headers_S[SENSOR][ENVIRONMENT].length = 2},
    {headers_S[SENSOR][STEERING].id = 0xE6, headers_S[SENSOR][STEERING].length = 2}}
} ;



#endif // MESSAGES_HEADER_H
