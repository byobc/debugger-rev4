#include "programmer.h"
#include "gpio.h"
// #include "pins.h"

static void delay_loop(int amount) {
    amount *= 3;
  volatile int i = 0;
  for (i = 0; i < amount; ++i);
}

namespace programmer {

static void do_write(uint16_t addr, uint8_t data) {
    gpio::write_addr_bus(addr | 0x8000);
    delay_loop(5);
    gpio::write_we(false);
    gpio::write_data_bus(data);
    delay_loop(5);
    gpio::write_we(true);
    delay_loop(1);
}

void sector_erase(uint16_t sector) {
    gpio::write_progb(false); // disable EEPROM output

    do_write(0x5555, 0xAA);
    do_write(0x2AAA, 0x55);
    do_write(0x5555, 0x80);
    do_write(0x5555, 0xAA);
    do_write(0x2AAA, 0x55);
    do_write(sector, 0x30);

    delay_loop(50000);
    delay_loop(50000);

    gpio::write_progb(true); // enable EEPROM output
}

void byte_program(uint16_t addr, uint8_t value) {
    gpio::write_progb(false); // disable EEPROM output

    do_write(0x5555, 0xAA);
    do_write(0x2AAA, 0x55);
    do_write(0x5555, 0xA0);

    do_write(addr | 0x8000, value);

    delay_loop(20);

    gpio::write_progb(true); // re-enable EEPROM output
}

} // namespace programmer
