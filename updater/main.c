//
//  main.c
//  serial
//
//  Created by Michael Kwasnicki on 21.07.16.
//  Copyright © 2016 Michael Kwasnicki. All rights reserved.
//

// https://en.wikibooks.org/wiki/Serial_Programming/termios


#include "mmapHelper.h"
#include "crc.h"
#include "config.h"


#include <assert.h>
#include <iso646.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h> // needed for memset
#include <inttypes.h>


typedef struct {
    uint16_t main_crc;
    uint16_t write_crc;
    uint16_t boot_crc;
    uint8_t main_page_count;
    uint8_t write_page_count;
    uint8_t boot_page_count;
    uint8_t lfuse;
    uint8_t hfuse;
    uint8_t efuse;
    uint8_t lock;
    uint8_t signature[3];
} __attribute__((packed)) Update_t;


#define PAGE_SIZE 128



static bool waitForTTY(const int in_TTY_FD, const char * const in_BUF) {
    size_t buf_len = strlen(in_BUF);
    char buf[80] = {'\0'};

    do {
        ssize_t cnt = read(in_TTY_FD, buf, sizeof(buf) - 1);

        if (cnt > 0) {
            buf[cnt] = '\0';
//            printf("< %2zd: %.3s\n", cnt, buf);
            bool result = (cnt >= buf_len && strncmp(buf, in_BUF, buf_len) == 0);

            do {
                usleep(20000);
            } while (read(in_TTY_FD, buf, sizeof(buf) - 1) > 0);

            return result;
        }
    } while (true);

    return false;
}



static void updateSection(uint16_t * const out_CRC, uint8_t * const out_NUM_PAGES, const int in_TTY_FD, const MapFile_s * const in_BIN, const uint16_t in_OFFSET) {
    bool ret;
    uint8_t *data = in_BIN->data;
    const uint32_t num_pages = (uint32_t)in_BIN->len / PAGE_SIZE;
    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < in_BIN->len; i++) {
        crc = crc16_update(crc, data[i]);
    }

    for (int p = 0; p < num_pages; p++) {
        uint16_t addr = p * PAGE_SIZE + in_OFFSET;
        char buf[80] = {'\0'};
        char *buf_end = &buf[78];
        char *buf_ptr = buf;

        do {
            buf_ptr = buf;
            uint8_t chksum = 0x00 - IHEX_DATA_MAX - (addr & 0xFF) - (addr >> 8);
            snprintf(buf_ptr, buf_end - buf_ptr, ":%02X%04X00", IHEX_DATA_MAX, addr);
            buf_ptr += 9;

            for (uint8_t i = 0; i < IHEX_DATA_MAX; i++) {
                uint8_t d = data[addr - in_OFFSET];
                chksum -= d;
                snprintf(buf_ptr, buf_end - buf_ptr, "%02X", d);
                buf_ptr += 2;
                addr++;
            }

            snprintf(buf_ptr, buf_end - buf_ptr, "%02X", chksum);
            fputs(buf, stdout);

            ssize_t cnt = write(in_TTY_FD, buf, strlen(buf));
            //            if (cnt > 0) printf("> %2zd: %s\n", cnt, buf);

            ret = waitForTTY(in_TTY_FD, "\tOK");

            if (!ret) {
                puts(" ERR");
                exit(EXIT_FAILURE);
            }

            puts(" OK");
        } while (addr < (p + 1) * PAGE_SIZE + in_OFFSET);

        buf_ptr = buf;
//        snprintf(buf_ptr, buf_end - buf_ptr, ":00000001FF");
        snprintf(buf_ptr, buf_end - buf_ptr, ":00000006FA");
        fputs(buf, stdout);

        ssize_t cnt = write(in_TTY_FD, buf, strlen(buf));
        //        if (cnt > 0) printf("> %2zd: %s\n", cnt, buf);

        ret = waitForTTY(in_TTY_FD, "\tOK");

        if (!ret) {
            puts(" ERR");
            exit(EXIT_FAILURE);
        }

        puts(" OK");
    }

    *out_CRC = crc;
    *out_NUM_PAGES = num_pages;
}



static void usage(const char in_EXEC[]) {
    puts("Usage:");
    puts(in_EXEC);
    puts(" -d <tty_device>");
    puts(" -m <file> <offset>  Binary <file> containing the data for the main OTA section which is located at <offset>. Must be used in combination with -w.");
    puts(" -w <file> <offset>  Binary <file> containing the data for the write OTA section which is located at <offset>. Must be used in combination with -m.");
    puts(" -b <file> <offset>  Binary <file> containing the data for the boot OTA section which is located at <offset>. Must NOT be used in combination with -w and/or -m.");
    exit(EXIT_FAILURE);
}



int main(int argc,char** argv)
{
    const char* execName = strrchr( argv[0], '/' );
    execName++;

    int ch;
    bool optD = false;
    bool optM = false;
    bool optW = false;
    bool optB = false;
    char *optDArg = NULL;
    char *optMArg = NULL;
    char *optMoffArg = NULL;
    char *optWArg = NULL;
    char *optWoffArg = NULL;
    char *optBArg = NULL;
    char *optBoffArg = NULL;

    while ((ch = getopt(argc, argv, "d:m:w:b:h?")) != -1) {
        switch ( ch ) {
            case 'd':
                optD = true;
                optDArg = optarg;
                break;

            case 'm':
                optM = true;
                optMArg = optarg;
                optMoffArg = argv[optind++];
                break;

            case 'w':
                optW = true;
                optWArg = optarg;
                optWoffArg = argv[optind++];
                break;

            case 'b':
                optB = true;
                optBArg = optarg;
                optBoffArg = argv[optind++];
                break;

            case 'h':
            case '?':
            default:
                usage( execName );
        }
    }

    argc -= optind;
    argv += optind;

//    printf("%s\n", optDArg);
//    printf("%s (%s)\n", optMArg, optMoffArg);

    int tty_fd = open(optDArg, O_RDWR | O_NONBLOCK);

    if (tty_fd < 0) {
        fprintf(stderr, "Could not open device \"%s\" \n", optDArg);
        exit(EXIT_FAILURE);
    }

    struct termios tio = { 0 };
    tio.c_iflag = 0;
    tio.c_oflag = 0;
    tio.c_cflag = CS8 | CREAD | CLOCAL;                                         // 8N1
    tio.c_lflag = 0;
    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 5;
    cfsetospeed(&tio, B19200);                                                  // TX 19200 baud
    cfsetispeed(&tio, B19200);                                                  // RX 19200 baud

    tcsetattr(tty_fd, TCSANOW, &tio);

    printf("Connecting to device \"%s\"... ", optDArg);
    fflush(stdout);

    sleep(3);

    bool ret = waitForTTY(tty_fd, "READY");

    if (!ret) {
        puts("FAILED!");
        return EXIT_FAILURE;
    }

    puts("OK!");


    MapFile_s bin;
    initMapFile(&bin, optMArg, MAP_RO);
    uint16_t main_offset = strtoul(optMoffArg, NULL, 0);
    uint16_t main_crc;
    uint8_t main_page_count;
    updateSection(&main_crc, &main_page_count, tty_fd, &bin, main_offset);
    printf("CRC-16: 0x%04X\n", main_crc);
    printf("Offset: 0x%04hx\n", main_offset);
    printf("Pages : %d\n", main_page_count);
    freeMapFile(&bin);

    initMapFile(&bin, optWArg, MAP_RO);
    uint16_t write_offset = strtoul(optWoffArg, NULL, 0);
    uint16_t write_crc;
    uint8_t write_page_count;
    updateSection(&write_crc, &write_page_count, tty_fd, &bin, write_offset);
    printf("CRC-16: 0x%04X\n", write_crc);
    printf("Offset: 0x%04hx\n", write_offset);
    printf("Pages : %d\n", write_page_count);
    freeMapFile(&bin);

    Update_t up = { 0 };
    up.main_crc = main_crc;
    up.main_page_count = main_page_count;
    up.write_crc = write_crc;
    up.write_page_count = write_page_count;
    up.boot_crc = 0xFFFF;
    up.boot_page_count = 0;
    up.lfuse = 0xFF;
    up.hfuse = 0xD2;
    up.efuse = 0xFA;
    up.lock = 0xFF;
    up.signature[0] = 0x1E;
    up.signature[1] = 0x95;
    up.signature[2] = 0x0F;


    uint16_t addr = 0x0000;
    char buf[80] = {'\0'};
    char *buf_end = &buf[78];
    char *buf_ptr = buf;

    do {
        uint8_t *data = (void*)&up;
        buf_ptr = buf;
        uint8_t chksum = 0x00 - sizeof(up) - (addr & 0xFF) - (addr >> 8) - 0x16;
        snprintf(buf_ptr, buf_end - buf_ptr, ":%02lX%04X16", sizeof(up), addr);
        buf_ptr += 9;

        for (uint8_t i = 0; i < sizeof(up); i++) {
            uint8_t d = data[addr];
            chksum -= d;
            snprintf(buf_ptr, buf_end - buf_ptr, "%02X", d);
            buf_ptr += 2;
            addr++;
        }

        snprintf(buf_ptr, buf_end - buf_ptr, "%02X", chksum);
        fputs(buf, stdout);

        ssize_t cnt = write(tty_fd, buf, strlen(buf));
//        if (cnt > 0) printf("> %2zd: %s\n", cnt, buf);

        ret = waitForTTY(tty_fd, "\tOK");

        if (!ret) {
            puts(" ERR");
            exit(EXIT_FAILURE);
        }

        puts(" OK");
    } while (false);

    close(tty_fd);
    return EXIT_SUCCESS;
}
