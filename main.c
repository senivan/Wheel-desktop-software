//
// Created by user on 11/9/24.
//
#include <stdio.h>
#include "libserialcom.h"

int main() {
    struct sp_port *port;
    init_serial(&port);
    printf("Hello, World!\n");
    return 0;
}

//int WinM/*ain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	/*struct sp_port* port;
	init_serial(&port);
	printf("Hello, World!\n");
	return 0;
}*/