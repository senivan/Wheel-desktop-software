#ifndef UNTITLED_LIBRARY_H
#define UNTITLED_LIBRARY_H
#include <stdbool.h>
#include <stdint.h>

// void hello(void);
typedef struct __attribute__((packed)){
	int rotation;
	bool left_arr;
	bool right_arr;
	bool up_arr;
	bool down_arr;

	bool a_butt;
	bool b_butt;
	bool x_butt;
	bool y_butt;

	bool dl_butt;
	bool dr_butt;

	bool r_shift;
	bool l_shift;

	int acceleration;
	int breaking;
} WheelSystemState;
void init_serial(struct sp_port* port);
void send_bytes(struct sp_port* port, char* data);
void read_bytes(struct sp_port* port, WheelState* state);
uint16_t calculate_crc16_checksum(WheelState* state);
bool close_serial(struct sp_port* port);

#endif //UNTITLED_LIBRARY_H
