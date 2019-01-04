#include "model.h"
#include <QObject>

#include "constants.h"
#include <iostream>

enum{LONGITUDINAL, LATERAL, UPWARDS} ;

//public funtions

Model::Model(ConnectionMonitor * outputEmitter,
             simulationScreen *outputScreen,
             CockpitScreen *inputScreen)
{
    OutputInterface = outputEmitter ;
    OutputWindow = outputScreen ;
    InputWindow = inputScreen ;

    //defien callback function for timer
    connect(&timer, SIGNAL(timeout()),this, SLOT(UpdateStep())) ;
    init() ;
}

void Model::init(void)
{
    //stop simulation if running
    simulate = 0 ;
    if(timer.isActive())
        timer.stop() ;
    OutputInterface->StopReceiving();

    //set initial values
    for(int i = 0; i<4;i++)
    {
        WheelSpeed[i] = 0;
        WheelTorque[i] = 0;
    }
    TrueSpeed = 0;
    TrueTorque = 0;
    netSpeed = 0;
    acceleration[0] = acceleration[1] = acceleration[2] = 0;

    StateOfCharge = INITIAL_CHARGE ;

    //dispaly initial state
    OutputWindow->UpdateScreen(WheelSpeed,
                               WheelTorque,
                               TrueSpeed,
                               TrueTorque,
                               StateOfCharge) ;

}

void Model::pause(void)
{
    simulate = !simulate;

    if (simulate)
    {
        timer.start(10) ;
        OutputInterface->StartReceiving();
    }
    else
    {
        OutputInterface->StopReceiving();
        timer.stop() ;
    }
    InputWindow->setStartButton(simulate) ;
}

void Model::setBreakPedal(int newBreakPedal)
{
    breakPedal = (double)newBreakPedal/MAX_BREAK_PEDAL ;
    OutputInterface->send_Pedals(breakPedal, acceleratorPedal) ;
}

void Model::setAccelerationPedal(int newAccelerationPedal)
{
    acceleratorPedal = (double)newAccelerationPedal/MAX_ACCELERATION_PEDAL ;
    OutputInterface->send_Pedals(breakPedal, acceleratorPedal) ;
 //   TorqueSetpoint = acceleratorPedal*150 ;
}

void Model::setSteeringAngle(int newSteeringAngle)
{
    steeringAngle =  (static_cast<double>(newSteeringAngle)-MAX_VAL_2)/MAX_VAL_2*STEERING_FACTOR ;
    OutputInterface->send_steering(steeringAngle) ;
    OutputWindow->setSteeringAngle(steeringAngle) ;
}

void Model::setSpeedSetpoint(uint16_t speedSetpoint)
{
    SpeedSetpoint = ((static_cast<double>(speedSetpoint)-MAX_VAL_2)/MAX_VAL_2)*V_MAX_MOTOR ; //2 because int goes only until 0x7FFF
//    std::cout<<"Speed: "<<std::hex<<speedSetpoint<<std::endl;
//    std::cout<<"Speed: "<<SpeedSetpoint<<std::endl;
}

void Model::setTorqueSetpoint(uint16_t torqueSetpoint)
{
    TorqueSetpoint = (static_cast<double>(torqueSetpoint)-MAX_VAL_2)/MAX_VAL_2*M_MAX_MOTOR ;
//    std::cout<<std::hex<<"Torque: "<<std::hex<<torqueSetpoint<<std::endl;
//    std::cout<<"Torque: "<<TorqueSetpoint<<std::endl;
}

// private slots
void Model::UpdateStep()
{
    //update simulation
    if(simulate)
    {
        NewTorque() ;
        NewAcceleration() ;
        NewWheelSpeed() ;

        OutputWindow->UpdateScreen(WheelSpeed,WheelTorque, TrueSpeed, TrueTorque, StateOfCharge) ;
        OutputInterface->UpdateMotor(TrueSpeed,TrueTorque) ;
        OutputInterface->UpdateSensor(WheelSpeed, acceleration) ;
    }
}


//private functions
void Model::NewTorque(void)
{
    if(StateOfCharge>0)
    {
        if(TrueTorque >= TorqueSetpoint+TORQUE_STEP)
            TrueTorque -= TORQUE_STEP ;
        else if(TrueTorque<= TorqueSetpoint-TORQUE_STEP)
            TrueTorque += TORQUE_STEP ;
        else
            TrueTorque = TorqueSetpoint ;
    }
    else
    {
        StateOfCharge = 0 ;
        TrueTorque = 0 ;
    }
}

void Model::NewAcceleration(void) //VSI
{
    double Traction = TrueTorque*((REDUCTION_FACTOR/EFFICIENCY)/(WHEEL_DIAMETER/2)) ;
    if (std::abs(Traction-breakPedal*BREAK_FACTOR) > MU * FN)
    {
        acceleration[LONGITUDINAL] = MU*FN/CAR_MASS ;
        if(Traction-breakPedal*BREAK_FACTOR < 0)
             acceleration[LONGITUDINAL] = -acceleration[LONGITUDINAL] ;
    }
    else
        acceleration[LONGITUDINAL] = (Traction-breakPedal*BREAK_FACTOR)/CAR_MASS ;
    acceleration[LONGITUDINAL] -= FRICTION; //breaking because of friction

    netSpeed += acceleration[LONGITUDINAL]*DELTA_T ;
    TrueSpeed = netSpeed/(PI*WHEEL_DIAMETER)*REDUCTION_FACTOR ;

    if (TrueSpeed <= 0)
    {
        acceleration[LONGITUDINAL] = 0;
        TrueTorque = 0 ;
        netSpeed = 0 ;
        TrueSpeed = 0 ;
    }
    else if(TrueSpeed >= SpeedSetpoint)
    {
        TrueSpeed = SpeedSetpoint ;
        netSpeed = TrueSpeed*(PI*WHEEL_DIAMETER)/(REDUCTION_FACTOR) ;
        acceleration[LONGITUDINAL] = 0 ;
        TrueTorque = 0 ;
    }

    OutputPower = TrueTorque*TrueSpeed/(60*2*PI) ;

    StateOfCharge = StateOfCharge - OutputPower*DELTA_T - LOSS; //LOSS are constant losses

    acceleration[LATERAL] = netSpeed*netSpeed*tan(steeringAngle)/WHEEL_BASE ;
}

void Model::NewWheelSpeed(void)
{
    WheelSpeed[FR] = netSpeed/(PI*WHEEL_DIAMETER)*(1-TRACKWIDTH_FRONT/WHEEL_BASE*tan(steeringAngle)) ;
    WheelSpeed[FL] = netSpeed/(PI*WHEEL_DIAMETER)*(1+TRACKWIDTH_FRONT/WHEEL_BASE*tan(steeringAngle)) ;
    WheelSpeed[RR] = (SLIP_SPEED+1)*netSpeed/(PI*WHEEL_DIAMETER)*(1-TRACKWIDTH_REAR/WHEEL_BASE*tan(steeringAngle)) ;
    WheelSpeed[RL] = (SLIP_SPEED+1)*netSpeed/(PI*WHEEL_DIAMETER)*(1+TRACKWIDTH_REAR/WHEEL_BASE*tan(steeringAngle)) ;

    WheelTorque[FR] = 0 ;
    WheelTorque[FL] = 0 ;
    if (WheelSpeed[RR] <= 0)
        WheelTorque[RR] = 0 ;
    else
        WheelTorque[RR] = OutputPower/(WheelSpeed[RR]/(60*PI)); //half of the power goes to each wheel
    if (WheelSpeed[RL] <= 0)
        WheelTorque[RL] = 0 ;
    else
        WheelTorque[RL] = OutputPower/(WheelSpeed[RL]/(60*PI));
}

