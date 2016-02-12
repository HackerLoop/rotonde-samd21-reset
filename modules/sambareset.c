/*
 * samba_reset.c
 *
 * Created: 2/5/2016 12:37:22 PM
 *  Author: stant
 */ 

#include "./sambareset.h"

#include <string.h>

#include "nvm.h"

#include "registrar/registrar.h"
#include "tasks/modules.h"

#define BOOTLOADER_START_ADDRESS 0x0000
#define BOOTLOADER_END_ADDRESS 0x2000

#define SAMBARESET "SAMBARESET"
#define DATA_SIZE 0

data_name sambareset_names[DATA_SIZE] = {};
data_type sambareset_types[DATA_SIZE] = {};
	
definition *reset_def;

void samba_reset(instance action);

void SAMBARESET_init(void) {
	reset_def = register_definition(ACTION , SAMBARESET , sambareset_names ,sambareset_types , DATA_SIZE , samba_reset, NULL , 0);
}

void samba_reset(instance action) {
	enum status_code error_code;
	int row = BOOTLOADER_END_ADDRESS / (NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE);
	int row_address = row_address;
	int page = BOOTLOADER_END_ADDRESS / NVMCTRL_PAGE_SIZE;
	int page_in_row = page - (row * NVMCTRL_ROW_PAGES);
	uint8_t row_buffer[NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE];
	
	for (int i = 0; i < NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE; i += NVMCTRL_PAGE_SIZE) {
		do
		{
			error_code = nvm_read_buffer(row_address + i, row_buffer + i, NVMCTRL_PAGE_SIZE);
		} while (error_code == STATUS_BUSY);
	}
	
	uint8_t *page_buffer = &(row_buffer[page_in_row * NVMCTRL_PAGE_SIZE]);
	*((uint32_t *)(page_buffer + 4)) = 0xffffffff;
	do
	{
		error_code = nvm_erase_row(row_address);
	} while (error_code == STATUS_BUSY);
	for (int i = 0; i < NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE; i += NVMCTRL_PAGE_SIZE) {
		do
		{
			error_code = nvm_write_buffer(row_address + i, row_buffer + i, NVMCTRL_PAGE_SIZE);
		} while (error_code == STATUS_BUSY);
	}
	system_reset();
}