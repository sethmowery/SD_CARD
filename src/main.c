#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "ff.h"
#include "diskio.h"
#include <stdio.h>
#include <string.h>

/*******************************************************************/

#define SD_MISO 28
#define SD_CS 29
#define SD_SCK 26
#define SD_MOSI 27

/*******************************************************************/

void init_spi_sdcard() {
    // Configure SPI pins
    gpio_set_function(SD_SCK, GPIO_FUNC_SPI);
    gpio_set_function(SD_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(SD_MISO, GPIO_FUNC_SPI);
    
    // Configure CS as GPIO (for manual control)
    gpio_init(SD_CS)
    ;
    gpio_set_dir(SD_CS, GPIO_OUT);
    gpio_put(SD_CS, 1);  // Set CS high (inactive)
    
    // Initialize SPI1 with 400 kHz baudrate, 8-bit data, CPOL=0, CPHA=0
    spi_init(spi1, 400000);
    spi_set_format(spi1, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
}

void disable_sdcard() {
    // Set CS high (inactive)
    gpio_put(SD_CS, 1);
    
    // Send 0xFF to give the SD card clock cycles to finish
    uint8_t dummy = 0xFF;
    spi_write_blocking(spi1, &dummy, 1);
    
    // Release MOSI line by making it a GPIO and setting it high
    gpio_init(SD_MOSI);
    gpio_set_dir(SD_MOSI, GPIO_OUT);
    gpio_put(SD_MOSI, 1);
}

void enable_sdcard() {
    // Take control of MOSI line by making it an SPI pin again
    gpio_set_function(SD_MOSI, GPIO_FUNC_SPI);
    
    // Set CS low (active)
    gpio_put(SD_CS, 0);
}

void sdcard_io_high_speed() {
    // Change SPI baudrate to 12 MHz
    spi_set_baudrate(spi1, 12000000);
}

void init_sdcard_io() {
    init_spi_sdcard();
    disable_sdcard();
}

/*******************************************************************/

void init_uart();
void init_uart_irq();
void date(int argc, char *argv[]);
void command_shell();

int main() {
    // Initialize the standard input/output library
    init_uart();
    init_uart_irq();
    
    init_sdcard_io();
    
    // SD card functions will initialize everything.
    command_shell();

    for(;;);
}