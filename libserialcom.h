#ifndef UNTITLED_LIBRARY_H
#define UNTITLED_LIBRARY_H
#include <stdbool.h>
#include <stdint.h>

// void hello(void);
#pragma pack(1)
typedef struct{
	uint16_t rotation;
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

	uint8_t acceleration;
	uint8_t breaking;
} WheelSystemState;
#pragma pack()
struct sp_port* init_serial();
	void send_bytes(struct sp_port* port, char* data);
	WheelSystemState read_bytes(struct sp_port** port);
	uint16_t calculate_crc16_checksum(WheelSystemState* state);
	bool close_serial(struct sp_port* port);

#endif //UNTITLED_LIBRARY_H
