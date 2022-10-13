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
#include "./spiBridgeInteraction.cpp"
// #include "./Peripherals.cpp"
using namespace std;


int main() {

    stdio_init_all();

    const uint LED = PICO_DEFAULT_LED_PIN;

    gpio_init(LED);

    gpio_set_dir(LED, GPIO_OUT);

    Peripherals periphers_state;

    SpiInterface com(&periphers_state);

    // com.clear_write_buffer();
    // com.clear_read_buffer();    

    while (true) {
        // com.setWriteBufferWithString("I am pico");
        // com.clear_read_buffer();
        // com.terminateWriteBuf();
        uint baud = spi_get_baudrate(spi0);
        int bytesWritten = com.exchangeByteMessage();
        gpio_put(LED, periphers_state.L[0]);       
    }

    return 0;
}
