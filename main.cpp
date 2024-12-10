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
FILE* ffb_file = fopen("test.txt", "w+");
PVOID FfbData;
// Convert Packet type to String
BOOL PacketType2Str(FFBPType Type, LPTSTR OutStr)
{
	BOOL stat = TRUE;
	LPTSTR Str = "";

	switch (Type)
	{
	case PT_EFFREP:
		Str = "Effect Report";
		break;
	case PT_ENVREP:
		Str = "Envelope Report";
		break;
	case PT_CONDREP:
		Str = "Condition Report";
		break;
	case PT_PRIDREP:
		Str = "Periodic Report";
		break;
	case PT_CONSTREP:
		Str = "Constant Force Report";
		break;
	case PT_RAMPREP:
		Str = "Ramp Force Report";
		break;
	case PT_CSTMREP:
		Str = "Custom Force Data Report";
		break;
	case PT_SMPLREP:
		Str = "Download Force Sample";
		break;
	case PT_EFOPREP:
		Str = "Effect Operation Report";
		break;
	case PT_BLKFRREP:
		Str = "PID Block Free Report";
		break;
	case PT_CTRLREP:
		Str = "PID Device Contro";
		break;
	case PT_GAINREP:
		Str = "Device Gain Report";
		break;
	case PT_SETCREP:
		Str = "Set Custom Force Report";
		break;
	case PT_NEWEFREP:
		Str = "Create New Effect Report";
		break;
	case PT_BLKLDREP:
		Str = "Block Load Report";
		break;
	case PT_POOLREP:
		Str = "PID Pool Report";
		break;
	default:
		stat = FALSE;
		break;
	}

	if (stat)
		_tcscpy_s(OutStr, 100, Str);

	return stat;
}

// Convert Effect type to String
BOOL EffectType2Str(FFBEType Type, LPTSTR OutStr)
{
	BOOL stat = TRUE;
	LPTSTR Str = "";

	switch (Type)
	{
	case ET_NONE:
		stat = FALSE;
		break;
	case ET_CONST:
		Str = "Constant Force";
		break;
	case ET_RAMP:
		Str = "Ramp";
		break;
	case ET_SQR:
		Str = "Square";
		break;
	case ET_SINE:
		Str = "Sine";
		break;
	case ET_TRNGL:
		Str = "Triangle";
		break;
	case ET_STUP:
		Str = "Sawtooth Up";
		break;
	case ET_STDN:
		Str = "Sawtooth Down";
		break;
	case ET_SPRNG:
		Str = "Spring";
		break;
	case ET_DMPR:
		Str = "Damper";
		break;
	case ET_INRT:
		Str = "Inertia";
		break;
	case ET_FRCTN:
		Str = "Friction";
		break;
	case ET_CSTM:
		Str = "Custom Force";
		break;
	default:
		stat = FALSE;
		break;
	};

	if (stat)
		_tcscpy_s(OutStr, 100, Str);

	return stat;
}

// Convert PID Device Control to String
BOOL DevCtrl2Str(FFB_CTRL Ctrl, LPTSTR OutStr)
{
	BOOL stat = TRUE;
	LPTSTR Str = "";

	switch (Ctrl)
	{
	case CTRL_ENACT:
		Str = "Enable Actuators";
		break;
	case CTRL_DISACT:
		Str = "Disable Actuators";
		break;
	case CTRL_STOPALL:
		Str = "Stop All Effects";
		break;
	case CTRL_DEVRST:
		Str = "Device Reset";
		break;
	case CTRL_DEVPAUSE:
		Str = "Device Pause";
		break;
	case CTRL_DEVCONT:
		Str = "Device Continue";
		break;
	default:
		stat = FALSE;
		break;
	}
	if (stat)
		_tcscpy_s(OutStr, 100, Str);

	return stat;
}

// Convert Effect operation to string
BOOL EffectOpStr(FFBOP Op, LPTSTR OutStr)
{
	BOOL stat = TRUE;
	LPTSTR Str = "";

	switch (Op)
	{
	case EFF_START:
		Str = "Effect Start";
		break;
	case EFF_SOLO:
		Str = "Effect Solo Start";
		break;
	case EFF_STOP:
		Str = "Effect Stop";
		break;
	default:
		stat = FALSE;
		break;
	}

	if (stat)
		_tcscpy_s(OutStr, 100, Str);

	return stat;
}

// Polar values (0x00-0xFF) to Degrees (0-360)
int Polar2Deg(BYTE Polar)
{
	return ((UINT)Polar * 360) / 255;
}

// Convert range 0x00-0xFF to 0%-100%
int Byte2Percent(BYTE InByte)
{
	return ((UINT)InByte * 100) / 255;
}

// Convert One-Byte 2's complement input to integer
int TwosCompByte2Int(BYTE in)
{
	int tmp;
	BYTE inv = ~in;
	BOOL isNeg = in >> 7;
	if (isNeg)
	{
		tmp = (int)(inv);
		tmp = -1 * tmp;
		return tmp;
	}
	else
		return (int)in;
}

//extern "C" {
//	//void init_serial(struct sp_port **port);
//	WheelSystemState read_bytes(struct sp_port** port);
//}
void ParseFfbData(PVOID data, sp_port** port) {
	// Extract size, command, and data
	int size = 12;
	FFBPType Type;
	Ffb_h_Type((FFB_DATA*)data, &Type);
	switch (Type) {
	case PT_CONSTREP: {
		// Parse constant force
			//SHORT magnitude = *reinterpret_cast<const SHORT*>(data);/*
			//std::cout << "Constant Force: Magnitude = " << magnitude << std::endl;*/
		FFB_EFF_CONSTANT Effect;
		Ffb_h_Eff_Constant((FFB_DATA*)data, &Effect);
		send_const_force(Effect.Magnitude, port);
	}
	}
	//send_const_force(1000, port);
	
}

//VOID CALLBACK FfbPacketCallback(PVOID data, PVOID userData) {
	
//	FFB_DATA* pPacket = reinterpret_cast<FFB_DATA *>(data);
//	ParseFfbData(pPacket);
//	//printf("%l %l %l\n", pPacket->cmd, pPacket->size, pPacket->data);
//	int i;
//	for (int i = 0; i < 8; i++) {
//		printf("%02X", ((uint8_t*)data)[i]);
//	}
//	printf("\n");
	//fprintf(ffb_file,"%l %l %l\n", pPacket->cmd, pPacket->size, pPacket->data);
//	for (int i = 0; i < 8; i++) {
//		fprintf(ffb_file,"%02X", ((uint8_t*)data)[i]);
//	}
//	fprintf(ffb_file,"\n");
//}
void CALLBACK FfbFunction(PVOID data)
{
	FFB_DATA* FfbData = (FFB_DATA*)data;
	int size = FfbData->size;
	_tprintf("\nFFB Size %d\n", size);

	_tprintf("Cmd:%08.8X ", FfbData->cmd);
	_tprintf("ID:%02.2X ", FfbData->data[0]);
	_tprintf("Size:%02.2d ", static_cast<int>(FfbData->size - 8));
	_tprintf(" - ");
	for (UINT i = 0; i < FfbData->size - 8; i++)
		_tprintf(" %02.2X", (UINT)FfbData->data);
	_tprintf("\n");
}

void CALLBACK FfbFunction1(PVOID data, PVOID userdata)
{
	// Packet Header
	_tprintf("\n ============= FFB Packet size Size %d =============\n", static_cast<int>(((FFB_DATA*)data)->size));

	/////// Packet Device ID, and Type Block Index (if exists)
#pragma region Packet Device ID, and Type Block Index
	int DeviceID, BlockIndex;
	FFBPType	Type;
	TCHAR	TypeStr[100];

	if (ERROR_SUCCESS == Ffb_h_DeviceID((FFB_DATA*)data, &DeviceID))
		_tprintf("\n > Device ID: %d", DeviceID);
	if (ERROR_SUCCESS == Ffb_h_Type((FFB_DATA*)data, &Type))
	{
		if (!PacketType2Str(Type, TypeStr))
			_tprintf("\n > Packet Type: %d", Type);
		else
			_tprintf("\n > Packet Type: %s", TypeStr);

	}
	if (ERROR_SUCCESS == Ffb_h_EBI((FFB_DATA*)data, &BlockIndex))
		_tprintf("\n > Effect Block Index: %d", BlockIndex);
#pragma endregion


	/////// Effect Report
#pragma region Effect Report
	FFB_EFF_CONST Effect;
	if (ERROR_SUCCESS == Ffb_h_Eff_Report((FFB_DATA*)data, &Effect))
	{
		if (!EffectType2Str(Effect.EffectType, TypeStr))
			_tprintf("\n >> Effect Report: %02x", Effect.EffectType);
		else
			_tprintf("\n >> Effect Report: %s", TypeStr);

		if (Effect.Polar)
		{
			_tprintf("\n >> Direction: %d deg (%02x)", Polar2Deg(Effect.Direction), Effect.Direction);


		}
		else
		{
			_tprintf("\n >> X Direction: %02x", Effect.DirX);
			_tprintf("\n >> Y Direction: %02x", Effect.DirY);
		};

		if (Effect.Duration == 0xFFFF)
			_tprintf("\n >> Duration: Infinit");
		else
			_tprintf("\n >> Duration: %d MilliSec", static_cast<int>(Effect.Duration));

		if (Effect.TrigerRpt == 0xFFFF)
			_tprintf("\n >> Trigger Repeat: Infinit");
		else
			_tprintf("\n >> Trigger Repeat: %d", static_cast<int>(Effect.TrigerRpt));

		if (Effect.SamplePrd == 0xFFFF)
			_tprintf("\n >> Sample Period: Infinit");
		else
			_tprintf("\n >> Sample Period: %d", static_cast<int>(Effect.SamplePrd));


		_tprintf("\n >> Gain: %d%%", Byte2Percent(Effect.Gain));

	};
#pragma endregion
#pragma region PID Device Control
	FFB_CTRL	Control;
	TCHAR	CtrlStr[100];
	if (ERROR_SUCCESS == Ffb_h_DevCtrl((FFB_DATA*)data, &Control) && DevCtrl2Str(Control, CtrlStr))
		_tprintf("\n >> PID Device Control: %s", CtrlStr);

#pragma endregion
#pragma region Effect Operation
	FFB_EFF_OP	Operation;
	TCHAR	EffOpStr[100];
	if (ERROR_SUCCESS == Ffb_h_EffOp((FFB_DATA*)data, &Operation) && EffectOpStr(Operation.EffectOp, EffOpStr))
	{
		_tprintf("\n >> Effect Operation: %s", EffOpStr);
		if (Operation.LoopCount == 0xFF)
			_tprintf("\n >> Loop until stopped");
		else
			_tprintf("\n >> Loop %d times", static_cast<int>(Operation.LoopCount));

	};
#pragma endregion
#pragma region Global Device Gain
	BYTE Gain;
	if (ERROR_SUCCESS == Ffb_h_DevGain((FFB_DATA*)data, &Gain))
		_tprintf("\n >> Global Device Gain: %d", Byte2Percent(Gain));

#pragma endregion
#pragma region Condition
	FFB_EFF_COND Condition;
	if (ERROR_SUCCESS == Ffb_h_Eff_Cond((FFB_DATA*)data, &Condition))
	{
		if (Condition.isY)
			_tprintf("\n >> Y Axis");
		else
			_tprintf("\n >> X Axis");
		_tprintf("\n >> Center Point Offset: %d", TwosCompByte2Int(Condition.CenterPointOffset) * 10000 / 127);
		_tprintf("\n >> Positive Coefficient: %d", TwosCompByte2Int(Condition.PosCoeff) * 10000 / 127);
		_tprintf("\n >> Negative Coefficient: %d", TwosCompByte2Int(Condition.NegCoeff) * 10000 / 127);
		_tprintf("\n >> Positive Saturation: %d", Condition.PosSatur * 10000 / 255);
		_tprintf("\n >> Negative Saturation: %d", Condition.NegSatur * 10000 / 255);
		_tprintf("\n >> Dead Band: %d", Condition.DeadBand * 10000 / 255);
	}
#pragma endregion
#pragma region Envelope
	FFB_EFF_ENVLP Envelope;
	if (ERROR_SUCCESS == Ffb_h_Eff_Envlp((FFB_DATA*)data, &Envelope))
	{
		_tprintf("\n >> Attack Level: %d", Envelope.AttackLevel * 10000 / 255);
		_tprintf("\n >> Fade Level: %d", Envelope.FadeLevel * 10000 / 255);
		_tprintf("\n >> Attack Time: %d", static_cast<int>(Envelope.AttackTime));
		_tprintf("\n >> Fade Time: %d", static_cast<int>(Envelope.FadeTime));
	};

#pragma endregion
#pragma region Periodic
	FFB_EFF_PERIOD EffPrd;
	if (ERROR_SUCCESS == Ffb_h_Eff_Period((FFB_DATA*)data, &EffPrd))
	{
		_tprintf("\n >> Magnitude: %d", EffPrd.Magnitude * 10000 / 255);
		_tprintf("\n >> Offset: %d", TwosCompByte2Int(EffPrd.Offset) * 10000 / 127);
		_tprintf("\n >> Phase: %d", EffPrd.Phase * 3600 / 255);
		_tprintf("\n >> Period: %d", static_cast<int>(EffPrd.Period));
	};
#pragma endregion

#pragma region Effect Type
	FFBEType EffectType;
	if (ERROR_SUCCESS == Ffb_h_EffNew((FFB_DATA*)data, &EffectType))
	{
		if (EffectType2Str(EffectType, TypeStr))
			_tprintf("\n >> Effect Type: %s", TypeStr);
		else
			_tprintf("\n >> Effect Type: Unknown");
	}

#pragma endregion

#pragma region Ramp Effect
	FFB_EFF_RAMP RampEffect;
	if (ERROR_SUCCESS == Ffb_h_Eff_Ramp((FFB_DATA*)data, &RampEffect))
	{
		_tprintf("\n >> Ramp Start: %d", TwosCompByte2Int(RampEffect.Start) * 10000 / 127);
		_tprintf("\n >> Ramp End: %d", TwosCompByte2Int(RampEffect.End) * 10000 / 127);
	};

#pragma endregion

	_tprintf("\n");
	FfbFunction(data);
	FfbData = data;
	_tprintf("\n ====================================================\n");

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

	//register ffb callback
	BOOL Ffbstarted = FfbStart(DevID);
	if (!Ffbstarted)
	{
		_tprintf("Failed to start FFB on vJoy device number %d.\n", DevID);
		int dummy = getchar();
		stat = -3;
		return 0;
	}
	else
		_tprintf("Started FFB on vJoy device number %d - OK\n", DevID);
	FfbRegisterGenCB(FfbFunction1, NULL);

	WheelSystemState state;
	//ffb_packet ffb;
	// Read serial, push serial to vjoydriver in a loop
	calibrate_wheel(&port);
	bool flag = true;
	try {
		state = read_bytes(&port);
	}
	catch (int e) {
		printf("Error reading bytes\n");
	}
	while (1) {
		try {
			if (flag) {
				state = read_bytes(&port);
			}
			else {
				ParseFfbData(FfbData, &port);
			}
			flag = !flag;
		}
		catch (int e) {
			printf("Error reading bytes\n");
		}
		//printf("Rotation: %d; Breaking: %d; Acc_pedal: %d\n", state.rotation, state.breaking, state.acceleration);
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