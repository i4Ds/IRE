#include "stdafx.h"
#include "MaestroController.h"
#include <vector>

MaestroController::MaestroController()
{
}


MaestroController::~MaestroController()
{
}

bool MaestroController::Connect(const char * portName, unsigned int baudRate)
{
	DCB commState;
	BOOL success;
	COMMTIMEOUTS timeouts;

	serial = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (serial == INVALID_HANDLE_VALUE)
	{
		switch (GetLastError())
		{
		case ERROR_ACCESS_DENIED:
			fprintf(stderr, "Error: Access denied.  Try closing all other programs that are using the device.\n");
			break;
		case ERROR_FILE_NOT_FOUND:
			fprintf(stderr, "Error: Serial port not found.  "
				"Make sure that \"%s\" is the right port name.  "
				"Try closing all programs using the device and unplugging the "
				"device, or try rebooting.\n", portName);
			break;
		default:
			fprintf(stderr, "Error: Unable to open serial port.  Error code 0x%x.\n", GetLastError());
			break;
		}
		return false;
	}

	/* Set the timeouts. */
	success = GetCommTimeouts(serial, &timeouts);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to get comm timeouts.  Error code 0x%x.\n", GetLastError());
		CloseHandle(serial);
		return false;
	}
	timeouts.ReadIntervalTimeout = 1000;
	timeouts.ReadTotalTimeoutConstant = 1000;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 1000;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	success = SetCommTimeouts(serial, &timeouts);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to set comm timeouts.  Error code 0x%x.\n", GetLastError());
		CloseHandle(serial);
		return false;
	}

	/* Set the baud rate. */
	success = GetCommState(serial, &commState);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to get comm state.  Error code 0x%x.\n", GetLastError());
		CloseHandle(serial);
		return false;
	}
	commState.BaudRate = baudRate;
	commState.ByteSize = 8;
	commState.StopBits = ONESTOPBIT;
	commState.Parity = NOPARITY;
	success = SetCommState(serial, &commState);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to set comm state.  Error code 0x%x.\n", GetLastError());
		CloseHandle(serial);
		return false;
	}

	/* Flush out any bytes received from the device earlier. */
	success = FlushFileBuffers(serial);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to flush port buffers.  Error code 0x%x.\n", GetLastError());
		CloseHandle(serial);
		return false;
	}

	return true;
}

bool MaestroController::Disconnect() {
	if (serial) {
		return (bool) CloseHandle(serial);
	}
	return false;
}

bool MaestroController::SetTarget(unsigned char channel, unsigned short target) {
	unsigned char command[4];

	// Compose the command.
	command[0] = 0x84;
	command[1] = channel;
	command[2] = target & 0x7F;
	command[3] = (target >> 7) & 0x7F;

	return SendData(command);
}

bool MaestroController::SetMultipleTargets(unsigned char firstChannel, std::initializer_list<unsigned short> targets) {
	const unsigned char numTargets = targets.size();
	const short commandSize = 3 + (numTargets * 2);
	std::vector<unsigned char> command(commandSize);

	// Compose the command.
	command[0] = 0x9F;
	command[1] = numTargets;
	command[2] = firstChannel;

	int i = 3;
	for (auto& target : targets) {
		command[i] = target & 0x7F;
		command[i + 1] = (target >> 7) & 0x7F;
		i += 2;
	}

	return SendData(&command[0], commandSize);
}

bool MaestroController::SendData(unsigned char* command) {
	return SendData(command, sizeof(command));
}

bool MaestroController::SendData(unsigned char* command, const short commandSize) {
	DWORD bytesTransferred;
	BOOL success;

	// Send the command to the device.
	success = WriteFile(serial, command, commandSize, &bytesTransferred, NULL);
	if (!success)
	{
		fprintf(stderr, "Error: Unable to write Set Target command to serial port.  Error code 0x%x.", GetLastError());
		return false;
	}
	if (commandSize != bytesTransferred)
	{
		fprintf(stderr, "Error: Expected to write %d bytes but only wrote %d.", sizeof(command), bytesTransferred);
		return false;
	}

	return true;
}