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
#include <cstddef>
#include <cstring>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
using namespace std;

class SpiInterface {

    private: uint8_t write_buffer[256];
    private: uint8_t read_buffer[256];
    private: int writeBufLength;

    public: SpiInterface(uint tx = 19, uint sck = 18, uint csn = 17, uint rx = 16, uint baudrate = 5000, uint CPOL = 0, uint CPHA = 0) {
        try {
           //initiatialize spi worker interface
        spi_init(spi0, baudrate);
        spi_set_slave(spi0, true);
        spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
        gpio_set_function(rx, GPIO_FUNC_SPI);
        gpio_set_function(sck, GPIO_FUNC_SPI);
        gpio_set_function(tx, GPIO_FUNC_SPI);
        gpio_set_function(csn, GPIO_FUNC_SPI);
        // Make the SPI pins available to picotool
        bi_decl(bi_4pins_with_func(rx, tx, sck, csn, GPIO_FUNC_SPI));

        clear_write_buffer();
        clear_read_buffer(); 
        } catch (...) {
            printf("Failure to initialize spi bus");
        }
        
    }
    public: void clear_write_buffer() {
        for(int i = 0; i < writeBufLength;i++) {
            write_buffer[i] = 0;
        };
    }
    public: void clear_read_buffer() {
        for(int i = 0; i < 255;i++) {
            read_buffer[i] = 0;
        };
    }
    public: void setWriteBuffer(string dataToTranscode) {
        try {



            copy(dataToTranscode.begin(), dataToTranscode.end(), write_buffer);

            // byte bytes[dataToTranscode.size()];
            
            // for(int i = 0; i < dataToTranscode.length(); i++) {
            //     if(dataToTranscode[i] != ';') bytes[i] = std::byte(dataToTranscode[i]);
            // };

            // for (auto &b: bytes) {
            //     write_buffer[b] std::to_integer<int>(b) << ' ';
            //  }


            writeBufLength = dataToTranscode.length();

        } catch (...) {
            printf("Couldn't set up new write buffer");
            return;
        }
    }
    public: void terminateWriteBuf() {
        for(int i = 0; i < 255; i++) {
            if(write_buffer[i] == '\000') {
                write_buffer[i] = ';';
                write_buffer[i+1] = ' ';
                return;
            };
        };
        write_buffer[255] = ';';
    }
    public: int appendDataToWriteBuf(string dataToTranscode) {
        try {

            for(int i = 0; i < 255; i++) {
                if(write_buffer[i] == 0x00) {
                    for(int s = i; s < i + dataToTranscode.length() ;s++ ) {
                        write_buffer[s] = dataToTranscode[s];
                    }
                    break;
                }
            }

            return dataToTranscode.length();

        } catch (...) {
            printf("Data could not be appended to write buffer");
            return 0;
        }
    }
    public: int writeToBus() {
        try {

            bool ready = spi_is_writable(spi0);

            if(ready == false) return 0;

            int bytesWritten = spi_write_blocking(spi0, write_buffer, writeBufLength);
            
            clear_write_buffer();

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

            int bytesRead = spi_read_blocking(spi0, 0, read_buffer, 256);

            return bytesRead;

        } catch (...) {
            printf("Failure to read from bus");
            return 0;
        }
    }
    public: int readAndWriteFromBus() {
        try {

            bool actionReady = spi_is_readable(spi0);
            actionReady = spi_is_writable(spi0);
            bool busy = spi_is_busy(spi0);

            if(actionReady == false && busy == true) return 0;

            int bytesWrittenAndRead = spi_write_read_blocking(spi0, write_buffer, read_buffer, 256);
            
        } catch (...) {
            printf("Failure to exchange data with host");
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

    // com.clear_write_buffer();
    // com.clear_read_buffer();    

    while (true) {
        gpio_put(LED, 1);
        sleep_ms(50);
        com.setWriteBuffer("aabbccdd");
        com.terminateWriteBuf();
        int bytesWritten = com.writeToBus();
        if(bytesWritten > 0) {
            gpio_put(LED, 0);
            sleep_ms(200);
        }
       
    }

    return 0;
}
