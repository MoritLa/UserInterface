#include <windows.h>

#include "serialport.h"
#include <iostream>

#define SERIAL_BUFFERLEN 512

SerialPort::SerialPort()
{
    mBuffer = new char[SERIAL_BUFFERLEN];

    mPort = "COM1";
    mParity = NOPARITY;
    mByteSize = 8;

    mStopBits = ONESTOPBIT;


    mBaudRate = 57600;
    mConnected = false;
}

SerialPort::SerialPort(char stopBits, unsigned baudRate)
{
    mBuffer = new char[SERIAL_BUFFERLEN];


    mPort = "COM1";
    mParity = NOPARITY;
    mByteSize = 8;

    if(stopBits == 2)
        mStopBits = TWOSTOPBITS;
    else
        mStopBits = ONESTOPBIT;

    mBaudRate = baudRate;
    mConnected = false;
}

SerialPort::SerialPort(char stopBits, unsigned baudRate, std::string    port)
{
    mBuffer = new char[SERIAL_BUFFERLEN];

    mPort = port;
    mParity = NOPARITY;
    mByteSize = 8;

    if(stopBits == 2)
        mStopBits = TWOSTOPBITS;
    else
        mStopBits = ONESTOPBIT;


    mBaudRate = baudRate;
    mConnected = false;
}

SerialPort::~SerialPort()
{	
    if(mConnected)
        disconnect();
    delete [] mBuffer;
}

int SerialPort::connect()
{
    if(mConnected) return 0; // Already connected..

    mHandle = CreateFileA(mPort.c_str(),
                        GENERIC_READ | GENERIC_WRITE,
                        0,    /* comm devices must be opened w/exclusive-access */
                        NULL, /* no security attrs */
                        OPEN_EXISTING, /* comm devices must use OPEN_EXISTING */
                        0,    /* not overlapped I/O */
                        NULL  /* hTemplate must be NULL for comm devices */
                        );
    if(mHandle == INVALID_HANDLE_VALUE)
        return 1;
    // Now get the DCB properties of the port we just opened
    if(!GetCommState(mHandle,&mDcb))
    {
        CloseHandle(mHandle);
        return 2;
    }

    // Fill mDcb with requested settings
    mDcb.BaudRate  =  mBaudRate;
    mDcb.ByteSize  =  mByteSize;
    mDcb.Parity    =  mParity;
    mDcb.StopBits  =  mStopBits;
    mDcb.fRtsControl = RTS_CONTROL_HANDSHAKE;

    // Set the port with requested settings.
    if(!SetCommState(mHandle,&mDcb))
    {
        CloseHandle(mHandle);
        return 3;
    }

    // Set the intial size of the transmit and receive queues.
    // Receive buffer to 32k, and the transmit buffer to 9k (just a default).
    if(!SetupComm(mHandle, 1024*32, 1024*9))
    {
        CloseHandle(mHandle);
        return 4;
    }

    // Deliberately made rather slow to match Linux settings (which cannot be made faster).
    mTimeOuts.ReadIntervalTimeout         = 5;
    mTimeOuts.ReadTotalTimeoutMultiplier  = 0;      // 1
    mTimeOuts.ReadTotalTimeoutConstant    = 10;    // 15
    mTimeOuts.WriteTotalTimeoutMultiplier = 1;
    mTimeOuts.WriteTotalTimeoutConstant   = 250;

    // Original
/*     mTimeOuts.ReadIntervalTimeout         = 15;
    mTimeOuts.ReadTotalTimeoutMultiplier  = 1;
    mTimeOuts.ReadTotalTimeoutConstant    = 250;
    mTimeOuts.WriteTotalTimeoutMultiplier = 1;
    mTimeOuts.WriteTotalTimeoutConstant   = 250;
*/

/*  // From Blimp bluetooth app
    mTimeOuts.ReadIntervalTimeout         = 100;
    mTimeOuts.ReadTotalTimeoutMultiplier  = 0;
    mTimeOuts.ReadTotalTimeoutConstant    = 5000;
    mTimeOuts.WriteTotalTimeoutMultiplier = 10;
    mTimeOuts.WriteTotalTimeoutConstant   = 100;*/


/*
    // Specifies the maximum time, in [ms], allowed to elapse between
    // the arrival of two successive characters
    mTimeOuts.ReadIntervalTimeout         = 100;

    // Total time out =
    // ReadTotalTimeoutMultiplier x requested number of bytes + ReadTotalTimeoutConstant
    mTimeOuts.ReadTotalTimeoutMultiplier  = 0;
    mTimeOuts.ReadTotalTimeoutConstant    = 5000;
    mTimeOuts.WriteTotalTimeoutMultiplier = 10;
    mTimeOuts.WriteTotalTimeoutConstant   = 100;
*/

    if(!SetCommTimeouts(mHandle, &mTimeOuts))
    {
        CloseHandle(mHandle);
        return 5;
    }

    /* Flush the content of the port. */
    //FlushFileBuffers(mHandle);
    char buffer;
    unsigned long read;
    do
    {
        ReadFile(mHandle, &buffer, 1, &read, NULL);
    }
    while(read != 0);

    // Connected!
    mConnected = true;
    return 0;
}

int SerialPort::disconnect()
{
    if(!mConnected)       // if already closed, return
        return 0;

    if(!CloseHandle(mHandle)) // non-zero on success
        return 1;
    mHandle = 0x0;


    mConnected = false;
    return 0;
}

int SerialPort::sendBuffer(const char *buffer, const unsigned length)
{
    if(!mConnected) return 1;

    if((length==0) || (buffer==NULL))
        return 2;

    unsigned long bogus;
    if(!WriteFile(mHandle, buffer, length, &bogus, NULL))
        return 3;


    return 0;
}

int SerialPort::sendString(const std::string& string)
{
    if(!mConnected) return 1;

    if(string.length() == 0)
    return 2;

    unsigned long bogus;
    FlushFileBuffers(mHandle);
    if(!WriteFile(mHandle, string.c_str(), string.length(), &bogus, NULL))
        return 3;

    return 0;
}

int SerialPort::receiveBuffer(char *buffer, const unsigned length)
{
    if(!mConnected) return 1;
    unsigned long bytesRead = 0, totalRead = 0;
    unsigned counter = 0;

    while(totalRead < length && counter < 1)
    {
        if(!ReadFile(mHandle, buffer + totalRead, length - totalRead, &bytesRead, NULL) || !bytesRead)

        counter++;
        totalRead += bytesRead;
    }

    if (totalRead < length)
    {
       // std::cerr << "SerialPort - ReceiveBuffer - Read " << totalRead << " bytes, expected " << length << std::endl;
        return 2;
    }
    return 0;
}

int SerialPort::receiveString(std::string& string)
{
	char *buffer = mBuffer;
	if(!mConnected)
		return 1;
	
	string.clear();
	
	unsigned long bytesRead = 0;
	while (true)
	{
		#ifdef WIN32
        ReadFile(mHandle, buffer, 1, &bytesRead, NULL);
		#else
        bytesRead = read(mFileDesc, buffer, 1);
		#endif
		char c = buffer[0];
		if (bytesRead > 0)
		{
			if (c == '\n')
				break;
			string += c;
		}
		else
			return 1;
	}
	return 0;
}

void SerialPort::sleep(unsigned long ms)
{
    Sleep(ms);
}
