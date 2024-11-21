//
// Created by user on 11/9/24.
//
//#include <stdio.h>
extern "C" {
	#include "libserialcom.h"
}
//#include "..\SDK\inc\public.h"
////#include "..\SDK\inc\vjoyinterface.h"
#include "stdafx.h"
//#include "Devioctl.h"
#include "public.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "vjoyinterface.h"
#include "Math.h"

#define DEV_ID		1

JOYSTICK_POSITION_V2 iReport;

//extern "C" {
//	//void init_serial(struct sp_port **port);
//	WheelSystemState read_bytes(struct sp_port** port);
//}
int main() {
	struct sp_port *port;
    //init_serial(&port);
	port = init_serial();
    printf("Hello, World!\n");
	int stat = 0;
	UINT DevID = DEV_ID;
	USHORT X = 0;
	USHORT Y = 0;
	USHORT Z = 0;
	LONG   Btns = 0;


	PVOID pPositionMessage;
	UINT	IoCode = LOAD_POSITIONS;
	UINT	IoSize = sizeof(JOYSTICK_POSITION);
	// HID_DEVICE_ATTRIBUTES attrib;
	BYTE id = 1;
	UINT iInterface = 1;


	// Get the driver attributes (Vendor ID, Product ID, Version Number)
	if (!vJoyEnabled())
	{
		_tprintf("Function vJoyEnabled Failed - make sure that vJoy is installed and enabled\n");
		int dummy = getchar();
		stat = -2;
		return 0;
	}
	/*else
	{
		wprintf(L"Vendor: %s\nProduct :%s\nVersion Number:%s\n", static_cast<TCHAR*> (GetvJoyManufacturerString()), static_cast<TCHAR*>(GetvJoyProductString()), static_cast<TCHAR*>(GetvJoySerialNumberString()));
	};*/

	// Get the status of the vJoy device before trying to acquire it
	VjdStat status = GetVJDStatus(DevID);
	switch (status)
	{
	case VJD_STAT_OWN:
		_tprintf("vJoy device %d is already owned by this feeder\n", DevID);
		break;
	case VJD_STAT_FREE:
		_tprintf("vJoy device %d is free\n", DevID);
		break;
	case VJD_STAT_BUSY:
		_tprintf("vJoy device %d is already owned by another feeder\nCannot continue\n", DevID);
		return -3;
	case VJD_STAT_MISS:
		_tprintf("vJoy device %d is not installed or disabled\nCannot continue\n", DevID);
		return -4;
	default:
		_tprintf("vJoy device %d general error\nCannot continue\n", DevID);
		return -1;
	};
	if (!AcquireVJD(DevID))
	{
		_tprintf("Failed to acquire vJoy device number %d.\n", DevID);
		int dummy = getchar();
		stat = -1;
		return 0;
	}
	else
		_tprintf("Acquired device number %d - OK\n", DevID);

	// Read serial, push serial to vjoydriver in a loop
	
	WheelSystemState state;
	try {
		state = read_bytes(&port);
	}
	catch (int e) {
		printf("Error reading bytes\n");
	}
	while (1) {
		try {
			state = read_bytes(&port);
		}
		catch (int e) {
			printf("Error reading bytes\n");
		}
		X = state.rotation;
		Y = state.acceleration;
		Z = state.breaking;
		Btns = 0;
		if (state.up_arr) {
			Btns |= 1;
		}
		if (state.right_arr) {
			Btns |= 2;
		}
		if (state.down_arr) {
			Btns |= 4;
		}
		if (state.left_arr) {
			Btns |= 8;
		}
		if (state.a_butt) {
			Btns |= 16;
		}
		if (state.x_butt) {
			Btns |= 32;
		}
		if (state.y_butt) {
			Btns |= 64;
		}
		if (state.b_butt) {
			Btns |= 128;
		}
		if (state.dl_butt) {
			Btns |= 256;
		}
		if (state.dr_butt) {
			Btns |= 512;
		}
		if (state.l_shift) {
			Btns |= 1024;
		}
		if (state.r_shift) {
			Btns |= 2048;
		}
		iReport.bDevice = DEV_ID;
		iReport.wAxisX = X;
		iReport.wAxisY = Y;
		iReport.wAxisZ = Z;
		iReport.lButtons = Btns;
		iReport.bHats = 0x0;
		UpdateVJD(DevID, (PVOID)&iReport);
	}


    return 0;
}