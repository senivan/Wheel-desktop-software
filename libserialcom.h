#ifndef UNTITLED_LIBRARY_H
#define UNTITLED_LIBRARY_H
#include <stdbool.h>
#include <stdint.h>

// void hello(void);
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
    uint16_t checksum;
} WheelState;
void init_serial(struct sp_port* port);
void send_bytes(struct sp_port* port, char* data);
void read_bytes(struct sp_port* port, WheelState* state);
uint16_t calculate_crc16_checksum(WheelState* state);
bool close_serial(struct sp_port* port);

#endif //UNTITLED_LIBRARY_H
