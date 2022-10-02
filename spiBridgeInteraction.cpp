#include "main.h"
#include <stdio.h>
#include <string.h>
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
    public: void setToSlave() {
        spi_set_slave(spi0, true);
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
    public: void setWriteBufferWithString(string dataToTranscode) {
        try {



            copy(dataToTranscode.begin(), dataToTranscode.end(), write_buffer);

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
                writeBufLength++;
                return;
            };
        };
        write_buffer[255] = ';';
    }
    // public: void setWriteBufferWithIntSet(uint8_t *listOfValues, int listLength) {
    //     try {

    //         for (int i = 0; i < listLength; i++) {
    //             write_buffer[i] = listOfValues[i];
    //         };

    //     } catch (...) {
    //         printf("Could not set write buffer with uintList");
    //         return;
    //     }
    // }
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

            int bytesRead = spi_read_blocking(spi0, 0, read_buffer, 255);

            return bytesRead;

        } catch (...) {
            printf("Failure to read from bus");
            return 0;
        }
    }
    public: int readAndWriteFromBus() {
        try {

            bool readable = spi_is_readable(spi0);
            bool writeable = spi_is_writable(spi0);
            bool busy = spi_is_busy(spi0);

            if(writeable == false || readable == false || busy == true) return 0;

            int bytesWrittenAndRead = spi_write_read_blocking(spi0, write_buffer, read_buffer, 4);

            return bytesWrittenAndRead;
            
        } catch (...) {
            printf("Failure to exchange data with host");
            return 0;
        }
    }
};
