/*
 * SPI Worker for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author     Justin Berkshire
 * @copyright   2022
 * @licence     MIT
 *
 */
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
using namespace std;

class SpiInterface {

    private: uint8_t write_buffer[256];
    private: uint8_t read_buffer[256];

    public: SpiInterface(uint tx = 19, uint sck = 18, uint csn = 17, uint rx = 16, uint baudrate = 1000 * 100, uint CPOL = 0, uint CPHA = 0) {
        //initiatialize spi worker interface
        spi_init(spi0, 1000 * 100);
        spi_set_slave(spi0, true);
        spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
        gpio_set_function(rx, GPIO_FUNC_SPI);
        gpio_set_function(sck, GPIO_FUNC_SPI);
        gpio_set_function(tx, GPIO_FUNC_SPI);
        gpio_set_function(csn, GPIO_FUNC_SPI);
        // Make the SPI pins available to picotool
        bi_decl(bi_4pins_with_func(rx, tx, sck, csn, GPIO_FUNC_SPI));
    }

    public: void setWriteBuffer(string dataToTranscode) {
        try {
            
            for(int i = 0; i < dataToTranscode.length(); i++) {
                write_buffer[i] = int(dataToTranscode[i]);
            };


        } catch (...) {
            printf("Couldn't set up new write buffer");
            return;
        }
    }
    public: int writeToBus() {
        try {

            bool ready = spi_is_writable(spi0);

            if(ready == false) return 0;

            int bytesWritten = spi_write_blocking(spi0, write_buffer, 255);

            return bytesWritten;

        } catch(...) {
            printf("Failure to write to bus");
            return 0;
        }
    }
    public: int readFromBus() {
        try {

            bool ready = spi_is_readable(spi0);

            if(ready == false) return 0;

            int bytesRead = spi_read_blocking(spi0, 0, read_buffer, 255);

            return bytesRead;

        } catch (...) {
            printf("Failure to read from bus");
            return 0;
        }
    }
};


int main() {

    stdio_init_all();

    const uint LED = PICO_DEFAULT_LED_PIN;

    gpio_init(LED);

    gpio_set_dir(LED, GPIO_OUT);

    SpiInterface com;

    while (true) {
        com.setWriteBuffer("Eat the rich;");
        gpio_put(LED, 1);
        sleep_ms(50);
        gpio_put(LED, 0);
        sleep_ms(50);
    }

    return 0;
}
