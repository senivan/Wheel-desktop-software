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
#include <time.h>
#include <iostream>

#define DEV_ID		1

JOYSTICK_POSITION_V2 iReport;
//void  CALLBACK FfbFunction(PVOID data) {
//	printf("FFB\n");
//	FFB_DATA* FfbData = (FFB_DATA*)data;
//	FFBEType FfbEffect = (FFBEType)-1;
//	LPCTSTR FfbEffectName[] =
//	{ "NONE", "Constant Force", "Ramp" };
//	
//
//	
//};


//extern "C" {
//	//void init_serial(struct sp_port **port);
//	WheelSystemState read_bytes(struct sp_port** port);
//}
void ParseFfbData(PFFB_DATA pPacket) {
	if (!pPacket) {
		std::cerr << "Invalid FFB packet received!" << std::endl;
		return;
	}
	// Extract size, command, and data
	ULONG size = pPacket->size;
	ULONG cmd = pPacket->cmd;
	const UCHAR* data = pPacket->data;

	// Ensure the packet is valid
	if (size < sizeof(FFB_DATA)) {
		std::cerr << "Packet size too small!" << std::endl;
		return;
	}

	// Handle the command type
	switch (cmd) {
	case PT_CONSTREP: {
		// Parse constant force
		if (size >= sizeof(FFB_DATA) + sizeof(SHORT)) {
			SHORT magnitude = *reinterpret_cast<const SHORT*>(data);
			std::cout << "Constant Force: Magnitude = " << magnitude << std::endl;
		}
		break;
	}
	case PT_RAMPREP: {
		// Parse ramp force
		if (size >= sizeof(FFB_DATA) + 2 * sizeof(SHORT)) {
			SHORT start = *reinterpret_cast<const SHORT*>(data);
			SHORT end = *reinterpret_cast<const SHORT*>(data + sizeof(SHORT));
			std::cout << "Ramp Effect: Start = " << start << ", End = " << end << std::endl;
		}
		break;
	}
	case PT_CONDREP: {
		// Parse friction effect (conditions)
		if (size >= sizeof(FFB_DATA) + 2 * sizeof(SHORT)) {
			SHORT positiveCoeff = *reinterpret_cast<const SHORT*>(data);
			SHORT negativeCoeff = *reinterpret_cast<const SHORT*>(data + sizeof(SHORT));
			std::cout << "Friction Effect: Positive Coeff = " << positiveCoeff
				<< ", Negative Coeff = " << negativeCoeff << std::endl;
		}
		break;
	}
	default:
		std::cerr << "Unknown command type: " << cmd << std::endl;
	}
}

VOID CALLBACK FfbPacketCallback(PVOID data, PVOID userData) {
	auto* pPacket = reinterpret_cast<PFFB_DATA>(data);
	ParseFfbData(pPacket);
}
int main() {
	struct sp_port *port;
    //init_serial(&port);
	port = init_serial();
    //printf("Hello, World!\n");
	static FFBEType FfbEffect = (FFBEType)-1;
	LPCTSTR FfbEffectName[] =
	{ "NONE", "Constant Force", "Ramp"};
	int stat = 0;
	UINT DevID = DEV_ID;
	USHORT X = 0;
	USHORT Y = 0;
	USHORT Z = 0;
	LONG   Btns = 0;
	FILE* logfile;
	logfile = fopen("Potentiometer.csv", "w+");
	if (logfile == NULL) return -1;
	fprintf(logfile, "Timestamp, Poten, Left_arr, Right_arr, Down_arr, Up_arr, A_butt, B_butt, X_butt, Y_butt, DL_butt, DR_butt, R_shift, L_shift\n");


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

	// register ffb callback
	//BOOL Ffbstarted = FfbStart(DevID);/*
	/*if (!Ffbstarted)
	{
		_tprintf("Failed to start FFB on vJoy device number %d.\n", DevID);
		int dummy = getchar();
		stat = -3;
		return;
	}
	else
		_tprintf("Started FFB on vJoy device number %d - OK\n", DevID);*/

	WheelSystemState state;
	ffb_packet ffb;
	// Read serial, push serial to vjoydriver in a loop
	calibrate_wheel(&port);
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
		printf("Rotation: %d; Breaking: %d; Acc_pedal: %d\n", state.rotation, state.breaking, state.acceleration);
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
		unsigned long timestamp = (unsigned long)time(NULL);
		fprintf(logfile, "%lu, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n", timestamp, state.rotation, state.left_arr, state.right_arr, state.down_arr, state.up_arr, state.a_butt, state.b_butt, state.x_butt, state.y_butt, state.dl_butt, state.dr_butt, state.r_shift, state.l_shift);
		UpdateVJD(DevID, (PVOID)&iReport);
	}

	fclose(logfile);
    return 0;
}