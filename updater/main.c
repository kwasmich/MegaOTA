//
//  main.c
//  serial
//
//  Created by Michael Kwasnicki on 21.07.16.
//  Copyright © 2016 Michael Kwasnicki. All rights reserved.
//

// https://en.wikibooks.org/wiki/Serial_Programming/termios

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h> // needed for memset

int main(int argc,char** argv)
{
    unsigned char chr = '\0';

    if (argc <= 1) {
        printf("Please start with %s /dev/ttyS1 (for example)\n",argv[0]);
        return EXIT_FAILURE;
    }

    struct termios stdio;
    memset(&stdio,0,sizeof(stdio));
    stdio.c_iflag=0;
    stdio.c_oflag=0;
    stdio.c_cflag=0;
    stdio.c_lflag=0;
    stdio.c_cc[VMIN]=1;
    stdio.c_cc[VTIME]=0;
    tcsetattr(STDOUT_FILENO,TCSANOW,&stdio);
    tcsetattr(STDOUT_FILENO,TCSAFLUSH,&stdio);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking

    struct termios tio;
    memset(&tio,0,sizeof(tio));
    tio.c_iflag=0;
    tio.c_oflag=0;
    tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
    tio.c_lflag=0;
    tio.c_cc[VMIN]=1;
    tio.c_cc[VTIME]=5;

    int tty_fd = open(argv[1], O_RDWR | O_NONBLOCK);
    cfsetospeed(&tio,B19200);            // 115200 baud
    cfsetispeed(&tio,B19200);            // 115200 baud

    tcsetattr(tty_fd,TCSANOW,&tio);

    while (chr != '#') {
        fflush(stdout);

        if (read(tty_fd, &chr, 1) > 0) {
            write(STDOUT_FILENO, &chr, 1);              // if new data is available on the serial port, print it out
        }

        if (read(STDIN_FILENO, &chr, 1) > 0) {                     // if new data is available on the console, send it to the serial port
            if (chr == '\r') {
                putchar(chr);
                write(tty_fd, &chr, 1);
                chr = '\n';
            }

            putchar(chr);
            write(tty_fd, &chr, 1);
        }

        usleep(100);
    }

    close(tty_fd);
    return 0;
}
