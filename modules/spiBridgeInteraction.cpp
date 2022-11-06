#include "../main.h"
using namespace std;

class SpiInterface {

    private: uint8_t write_buffer[256];
    public: uint8_t read_buffer[256];
    private: int writeBufLength;
    private: Peripherals *peripherals;

    public: SpiInterface(Peripherals *perf, uint tx = 19, uint sck = 18, uint csn = 17, uint rx = 16, uint baudrate = 1000*100, uint CPOL = 0, uint CPHA = 0) {
        try {
            peripherals = perf;
            memset(write_buffer, 0, 32);
            memset(read_buffer, 0, 32);
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

            // clear_read_buffer();
            // clear_write_buffer();
        } catch (...) {
            printf("Failure to initialize spi bus");
        }
        
    }
    public: void setToSlave() {
        spi_set_slave(spi0, true);
    }
    public: void clear_write_buffer() {
        for(int i = 0; i < 255;i++) {
            write_buffer[i] = 0;
        };
    }
    public: void clear_read_buffer() {
        for(int i = 0; i < 255;i++) {
            read_buffer[i] = 0;
        };
    }
    public: void construct_new_write() {
        try {

            peripherals->pause_updates = true;

            while(peripherals->updates_occurring == true) {
                tight_loop_contents();
            }

            string newMessage = peripherals->generate_new_response();

            peripherals->pause_updates = false;

            char newCommand[newMessage.length()]; 

            strcpy(newCommand, newMessage.c_str());

            for(int i = 0;i < newMessage.length() - 1;i++) {
                write_buffer[i] = newCommand[i];
            }
            
        } catch (...) {
            printf("Couldn't set up new write buffer");
            return;
        }
    }
    public: int writeToBus() {
        try {

            bool ready = spi_is_writable(spi0);

            if(ready == false) return 0;

            int bytesWritten = spi_write_blocking(spi0, write_buffer, 1);
            
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

            int bytesRead = spi_read_blocking(spi0, 0, read_buffer, 32);

            return bytesRead;

        } catch (...) {
            printf("Failure to read from bus");
            return 0;
        }
    }
    public: int exchangeByteMessage() {
        try {

            bool readable = spi_is_readable(spi0);
            bool writeable = spi_is_writable(spi0);

            if(writeable == false || readable == false) return 0;

            construct_new_write();

            uint8_t check_read[1];
            uint8_t check_write[1] = {1};

            spi_write_read_blocking(spi0, check_write, check_read, 1);

            if(check_read[0] != 1) return 0;

            int bytesExchanged = spi_write_read_blocking(spi0, write_buffer, read_buffer, 256);
            
            clear_write_buffer();

            int previous_min = 0;

            for(int i = 0; i < 256; i++) {
                if(read_buffer[i] == ';') {
                    uint8_t command[i - previous_min];
                    int command_index = 0;
                    for(int c = previous_min; c <= i; c++) {
                        command[command_index] = read_buffer[c];
                        command_index++;
                        previous_min = c;
                    }
                    previous_min++;
                    peripherals->process_command(command, previous_min);
                }
            }

            clear_read_buffer();

            return 1;

        } catch (...) {
            printf("Single byte exchange failed");
            return -1;
        }
    }
};
