#include "libserialcom.h"
#include <libserialport.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// void hello(void) {
//     printf("Hello, World!\n");
// }
char PORTNAME[14] = "/dev/tty/USB0\0";

typedef struct WheelState {
    uint32_t wheel_turn;
    uint8_t acc_pedal;
    uint8_t brake_pedal;
    bool button_1;
    bool button_2;
    bool button_3;
    bool button_4;
    bool button_5;
    bool button_6;
    bool button_7;
    bool button_8;
    bool button_9;
    bool button_10;
    uint16_t checksum;;
} wheel_state;
void read_bytes(struct sp_port *port, wheel_state *state) {
    char buf[10];
    sp_nonblocking_read(port, buf, 12);
    // 1 byte is start byte
    // 3 bytes is wheel turn
    // 2 bytes are buttins
    // 1 byte is acc pedal
    // 1 byte is brake pedal
    // 2 bytes is crc16 checksum
    state->wheel_turn = (buf[1] << 16) | (buf[2] << 8) | buf[3];
    state->button_1 = buf[4] & 0x01;
    state->button_2 = buf[4] & 0x02;
    state->button_3 = buf[4] & 0x04;
    state->button_4 = buf[4] & 0x08;
    state->button_5 = buf[4] & 0x10;
    state->button_6 = buf[4] & 0x20;
    state->button_7 = buf[4] & 0x40;
    state->button_8 = buf[4] & 0x80;
    state->button_9 = buf[5] & 0x01;
    state->button_10 = buf[5] & 0x02;
    state->acc_pedal = buf[6];
    state->brake_pedal = buf[7];
    state->checksum = (buf[8] << 8) | buf[9];
    if (state->checksum != calculate_crc16_checksum(state)) {
        printf("Checksum error\n");
    }
}

void send_bytes(struct sp_port *port, char *data) {
    sp_nonblocking_write(port, data, sizeof(data));
}

uint16_t calculate_crc16_checksum(wheel_state *state) {
    char buf[7];
    buf[0] = (state->wheel_turn >> 16) & 0xFF;
    buf[1] = (state->wheel_turn >> 8) & 0xFF;
    buf[2] = state->wheel_turn & 0xFF;
    buf[3] = (state->button_1 << 7) | (state->button_2 << 6) | (state->button_3 << 5) | (state->button_4 << 4) | (state->button_5 << 3) | (state->button_6 << 2) | (state->button_7 << 1) | state->button_8;
    buf[4] = (state->button_9 << 7) | (state->button_10 << 6);
    buf[5] = state->acc_pedal;
    buf[6] = state->brake_pedal;

    uint16_t crc = 0xFFFF;
    for (int i = 0; i < 7; i++) {
        crc ^= buf[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

void init_serial(struct sp_port *port) {
    sp_get_port_by_name(PORTNAME, &port);
    sp_open(port, SP_MODE_READ_WRITE);
    sp_set_baudrate(port, 115200);
}

bool close_serial(struct sp_port *port) {
    return sp_close(port);
}

