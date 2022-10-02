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
#include "hardware/spi.h"
#include "./spiBridgeInteraction.cpp"
using namespace std;


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
        com.setWriteBufferWithString("a");
        com.clear_read_buffer();
        com.terminateWriteBuf();
        int bytesWritten = com.readAndWriteFromBus();
        if(bytesWritten > 0) {
            gpio_put(LED, 0);
            sleep_ms(200);
        }
       
    }

    return 0;
}
