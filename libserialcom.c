#include "libserialcom.h"
#include "libserialport.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

// void hello(void) {
//     printf("Hello, World!\n");
// }
WheelSystemState read_bytes(struct sp_port **port) {
    WheelSystemState state;
    uint8_t buf[sizeof(WheelSystemState)+1];
    uint8_t temp[1];
    temp[0] = 0x69;
    memset(buf, 0, sizeof(buf));
	//memset(temp, 0, sizeof(temp));
    //delay(100);
    printf("Sending poll byte\n");
	sp_blocking_write(*port, &temp[0], 1, 1000);
	sp_blocking_read(*port, temp, 1, 1000);
	while ((uint8_t)temp[0] != 83) {
		//printf("%d\n", temp[0]);
		sp_blocking_read(*port, temp, 1, 1000);
	}
    printf("Received data\n");
 
    /*while (true) {
		
        if (sp_input_waiting(*port) >= sizeof(WheelSystemState)+1) {
            printf("%d\n", sp_input_waiting(*port));
            break;
        }
    }*/
	sp_blocking_read(*port, buf, sizeof(buf), 1000);
	for (int i = 0; i < sizeof(buf); i++) {
		printf("%d ", buf[i]);
	}
	printf("\n");
    memcpy(&state.rotation, &buf[1], 1);
    state.left_arr = ((uint8_t)buf[2] == 1) ? 1 : 0;
	state.right_arr = ((uint8_t)buf[3] == 1) ? 1 : 0;
	state.up_arr = ((uint8_t)buf[4] == 1) ? 1 : 0;
	state.down_arr = ((uint8_t)buf[5] == 1) ? 1 : 0;
	state.a_butt = ((uint8_t)buf[6] == 1) ? 1 : 0;
	state.b_butt = ((uint8_t)buf[7] == 1) ? 1 : 0;
	state.x_butt = ((uint8_t)buf[8] == 1) ? 1 : 0;
	state.y_butt = ((uint8_t)buf[9] == 1) ? 1 : 0;
	state.dl_butt = ((uint8_t)buf[10] == 1) ? 1 : 0;
	state.dr_butt = ((uint8_t)buf[11] == 1) ? 1 : 0;
	state.r_shift = ((uint8_t)buf[12] == 1) ? 1 : 0;
	state.l_shift = ((uint8_t)buf[13] == 1) ? 1 : 0;
	memcpy(&state.acceleration, &buf[14], 1);
	memcpy(&state.breaking, &buf[15], 1);
	printf("Rotation: %d\n", state.rotation);
	return state;
}

void send_bytes(struct sp_port *port, char *data) {
    sp_nonblocking_write(port, data, sizeof(data));
}

/*uint16_t calculate_crc16_checksum(WheelSystemState* state) {
    char buf[7];
    buf[0] = (state->rotation >> 16) & 0xFF;
    buf[1] = (state->rotation >> 8) & 0xFF;
    buf[2] = state->rotation & 0xFF;
    buf[3] = (state->button_1 << 7) | (state->button_2 << 6) | (state->button_3 << 5) | (state->button_4 << 4) | (state->button_5 << 3) | (state->button_6 << 2) | (state->button_7 << 1) | state->button_8;
    buf[4] = (state->button_9 << 7) | (state->button_10 << 6);
    buf[5] = state->acc_pedal;
    buf[6] = state->brake_pedal;

    uint16_t crc = 0xFFFF;
    for (int i = 0; i < 7; i++) {
        crc ^= buf[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
     /*          crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}*/

struct sp_port* init_serial() {
	struct sp_port* port = (struct sp_port*)malloc(sizeof(struct sp_port*));
    sp_get_port_by_name("\\\\.\\COM8", &port);
    sp_open(port, SP_MODE_READ_WRITE);
    sp_set_baudrate(port, 115200);
	sp_set_bits(port, 8);
	sp_set_parity(port, SP_PARITY_NONE);
	sp_set_stopbits(port, 1);
	sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE);
	return port;
}

bool close_serial(struct sp_port *port) {
    return sp_close(port);
}

