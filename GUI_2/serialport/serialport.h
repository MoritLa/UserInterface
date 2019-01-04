#ifndef SERIALPORT_H
#define SERIALPORT_H


#include <windows.h>

#include <string>

//! This class interfaces a serial port both under Windows and Linux
class SerialPort
{
public:
	//! Constructor: 1 stop bit, 57600, first serial port
	SerialPort();
	//! Constructor: first serial port, configurable number of stop bits and baud rate
	SerialPort(char stopBits, unsigned baudRate);
	//! Constructor: configurable serial port, number of stop bits and baud rate
	SerialPort(char stopBits, unsigned baudRate, std::string port);
	// ! Destructor, release serial port
	virtual ~SerialPort();

	// Connect serial port, return 0 on success, another value on error
	virtual int connect();
	// Disconnect, return 0 on success, another value on error
	virtual int disconnect();
	
	//! Send a buffer of bytes. Return 0 on success, another value on error
	virtual int sendBuffer(const char *buffer, const unsigned length);
	//! Send a string, do not add any carriage return nor new line. Return 0 on success, another value on error
	virtual int sendString(const std::string& string);
	
	//! Receive a buffer of bytes. Return 0 on success, another value on error
	virtual int receiveBuffer(char *buffer, const unsigned length);
	//! Receive a string (eat new line). Return 0 on success, another value on error
	virtual int receiveString(std::string& string);

	// Change baud rate, effect on next disconnect/connect
	void setBaudRate(unsigned baudRate) { mBaudRate = baudRate; }
	// Change serial port, takes effect on next disconnect/connect
	void setCommPort(std::string commPort) { mPort = commPort; }
	// Change number of stop bits, takes effect on next disconnect/connect
	void setCommPort(char stopBits) { mStopBits = stopBits; }

	// Convenient function to have an OS-agnostic sleep
	void sleep(unsigned long ms);

protected:
	char *		mBuffer;
	std::string	mPort;
	unsigned	mBaudRate;
	char 		mParity, mStopBits, mByteSize;
	bool 		mConnected;

	HANDLE 		mHandle;
	DCB				mDcb;
	COMMTIMEOUTS  mTimeOuts;
};

#endif /* SERIALPORT_H */
