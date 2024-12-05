#ifndef UNTITLED_LIBRARY_H
#define UNTITLED_LIBRARY_H
#include <stdbool.h>
#include <stdint.h>

// void hello(void);
#pragma pack(1)
typedef struct{
	int16_t rotation;
	uint8_t left_arr;
	uint8_t right_arr;
	uint8_t up_arr;
	uint8_t down_arr;

	uint8_t a_butt;
	uint8_t b_butt;
	uint8_t x_butt;
	uint8_t y_butt;

	uint8_t dl_butt;
	uint8_t dr_butt;

	uint8_t r_shift;
	uint8_t l_shift;

	uint16_t acceleration;
	uint16_t breaking;
} WheelSystemState;
typedef struct {
	uint8_t ffb_strength; // 0-255
	uint8_t ffb_direction : 1; // 0 - left, 1 - right
	uint8_t ffb_type : 2; // 0 - constant, 1 - ramp, 2 - rumble, 3 - unused
} ffb_packet;
#pragma pack()
struct sp_port* init_serial();
	void send_ffb_bytes(struct sp_port* port, char* data, size_t size);
	WheelSystemState read_bytes(struct sp_port** port);
	uint16_t calculate_crc16_checksum(WheelSystemState* state);
	bool close_serial(struct sp_port* port);
	void calibrate_wheel(struct sp_port** port);
	uint8_t* read_calibration(struct sp_port** port);

#endif //UNTITLED_LIBRARY_H
