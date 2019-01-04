#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MAX_BREAK_PEDAL         100
#define MAX_ACCELERATION_PEDAL  100
#define MAX_STEERING_WHEEL      100
#define MAX_SPEED               100

#define STEERING_FACTOR         0.3         //[rad]

#define TORQUE_STEP         5             //[Nm]
#define REDUCTION_FACTOR    (64/13)
#define EFFICIENCY          0.95
#define WHEEL_DIAMETER      0.508           //[m]
#define MU                  0.7
#define CAR_MASS            300             //[kg]
#define FN                  (CAR_MASS*9.81) //[N]
#define DELTA_T             0.01           //[s]
#define WHEEL_BASE          1.525           //[m]
#define BREAK_FACTOR        600             //[N]
#define BREAK_FACTOR_M      300             //[N]
#define TRACKWIDTH_FRONT    1.5             //[m]
#define TRACKWIDTH_REAR     1.5             //[m]
#define INITIAL_CHARGE      450000          //[J]

#define FRICTION            0.1             //[m/s^2]
#define LOSS                1               //[J/ms]
#define SLIP_SPEED          0.001           //[-]

#define PI                     3.141592

#define MY_BAUD     57600                   //115200
#define ECU_TYPE    0
#define MOTOR_TYPE  1
#define SENSOR_TYPE 2

#define MAX_UART_MESSAGE_LENGTH		8

#define MAX_VAL     0xFFFF
#define MAX_VAL_2   0x7FFF

#define V_MAX_MOTOR 5500                    //[rpm]
#define M_MAX_MOTOR 240                     //[Nm]
#define V_MAX_WHEEL 1500                    //[rpm]
#define A_MAX       40                      //[m/s^2]
#define V_MAX       40                      //[m/s]

#endif // CONSTANTS_H
