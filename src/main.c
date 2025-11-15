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
    // fill in.
    gpio_set_function(SD_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(SD_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(SD_MISO, GPIO_FUNC_SPI);

    // -------- 2. Configure CS as normal GPIO --------
    gpio_init(SD_CS);
    gpio_set_dir(SD_CS, GPIO_OUT);
    gpio_put(SD_CS, 1);   // CS high (inactive)

    // -------- 3. Configure SPI peripheral --------
    // Initialize SPI at 400 kHz for SD init
    spi_init(spi1, 400 * 1000);

    // Explicitly configure SPI format:
    // data_bits = 8
    // CPOL = 0 -> clock idle low
    // CPHA = 0 -> sample on rising edge
    spi_set_format(
        spi1,
        8,          // data bits
        SPI_CPOL_0, // CPOL = 0
        SPI_CPHA_0, // CPHA = 0
        SPI_MSB_FIRST
    );
}

void disable_sdcard() {
    // fill in.
    uint8_t ff = 0xFF;

    // 1. Raise CS (deselect card)
    gpio_put(SD_CS, 1);

    // 2. Send one dummy byte to give SD card some final clocks
    spi_write_blocking(spi1, &ff, 1);

    // 3. Temporarily "release" MOSI by switching it to GPIO high
    gpio_set_function(SD_MOSI, GPIO_FUNC_SIO);  // MOSI becomes GPIO
    gpio_set_dir(SD_MOSI, GPIO_OUT);
    gpio_put(SD_MOSI, 1); // force high (idle state)
}

void enable_sdcard() {
    // fill in.
    // 1. Bring MOSI back under SPI control
    gpio_set_function(SD_MOSI, GPIO_FUNC_SPI);

    // 2. Pull CS low to activate SD card
    gpio_put(SD_CS, 0);
}

void sdcard_io_high_speed() {
    // fill in.
    // Change the SPI baudrate to 12 MHz
    spi_set_baudrate(spi1, 12 * 1000 * 1000);
}

void init_sdcard_io() {
    // fill in.
    // Initialize SPI pins and configure SPI peripheral
    init_spi_sdcard();

    // Set CS high and release MOSI (inactive state)
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