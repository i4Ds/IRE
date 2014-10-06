#pragma once

#include <windows.h>
#include <initializer_list>
class MaestroController
{
public:
	MaestroController();
	~MaestroController();
	bool Connect(const char * portName, unsigned int baudRate);
	bool Disconnect();
	bool SetTarget(unsigned char channel, unsigned short target);
	bool SetMultipleTargets(unsigned char firstChannel, std::initializer_list<unsigned short> targets);
private:
	HANDLE serial;
	bool SendData(unsigned char* command);
	bool SendData(unsigned char* command, const short commandSize);
};

