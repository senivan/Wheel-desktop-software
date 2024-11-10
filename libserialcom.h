#ifndef UNTITLED_LIBRARY_H
#define UNTITLED_LIBRARY_H
#include <stdbool.h>
#include <stdint.h>

// void hello(void);
typedef struct WheelState wheel_state;
void init_serial();
void send_bytes(struct sp_port *port, char *data);
void read_bytes();
uint16_t calculate_crc16_checksum(wheel_state *state);
bool close_serial(struct sp_port *port);
#endif //UNTITLED_LIBRARY_H
