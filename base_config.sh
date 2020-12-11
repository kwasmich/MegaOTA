#!/usr/bin/env bash

export MAIN_START=0x0000
export MAIN_END=0x3BFE
export MAIN_OTA_START=0x3C00
export MAIN_OTA_END=0x77FE
export BOOT_OTA_START=0x7800
export BOOT_OTA_END=0x7B7E
export WRITE_OTA_START=0x7B80
export WRITE_OTA_END=0x7BFE
export BOOT_START=0x7C00
export BOOT_END=0x7F7E
export WRITE_START=0x7F80
export WRITE_END=0x7FFE
export FLASH_END=0x8000


#MAIN_START= 0x0000
#MAIN_END = $(shell printf "0x%04x\n" $$(($(MAIN_OTA_START) - 2)))
#MAIN_OTA_START= 0x3800
#MAIN_OTA_END= $(shell printf "0x%04x\n" $$(($(WRITE_OTA_START) - 2)))
#WRITE_OTA_START= 0x7000
#WRITE_OTA_END= $(shell printf "0x%04x\n" $$(($(BOOT_OTA_START) - 2)))
#BOOT_OTA_START= 0x7400
#BOOT_OTA_END= $(shell printf "0x%04x\n" $$(($(BOOT_START) - 2)))
#BOOT_START= 0x7800
#BOOT_END = $(shell printf "0x%04x\n" $$(($(WRITE_START) - 2)))
#WRITE_START= 0x7C00
#WRITE_END= $(shell printf "0x%04x\n" $$(($(FLASH_END) - 2)))
#FLASH_END= 0x8000

export EXTERNAL_MEMORY=false

export LFUSE=0xFF #0xE2
export HFUSE=0xD4
export EFUSE=0xFF
export LOCKFUSE=0xFF
