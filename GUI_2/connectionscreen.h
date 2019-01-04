#ifndef CONNECTIONSCREEN_H
#define CONNECTIONSCREEN_H

#include <QWidget>
#include <QLineEdit>

#define NOTCONNECTED    0
#define CONNECTED       1
#define CONNECTING      2

typedef void (*ConnectionCallbackFP)(QString port) ;

namespace Ui {
class ConnectionScreen;
}

class ConnectionScreen : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionScreen(QWidget *parent = nullptr);
    ~ConnectionScreen();
    // initialise function pointers with external callback function
    void setCallbacks(ConnectionCallbackFP extMotor,
                      ConnectionCallbackFP extSensor,
                      ConnectionCallbackFP extECU) ;
    // change button text of one of the buttons
    void changeButton(int device, int state) ;

private slots:
    // callback functions that are called when a button is pressed in the window
    void motor_cb() ;
    void ECU_cb() ;
    void sensor_cb() ;
private:
    // function pointer that are called by the slot of above
    ConnectionCallbackFP extMotor_cb ;
    ConnectionCallbackFP extSensor_cb ;
    ConnectionCallbackFP extECU_cb ;

    Ui::ConnectionScreen *ui;
};

#endif // CONNECTIONSCREEN_H
