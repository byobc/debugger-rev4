#include "gpio.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal.h" // IWYU pragma: keep
#include "stm32f1xx_hal_gpio.h"
// #include "pins.h"

#define DATA_PINS_MASK 0b1111111100 // PB2:9
#define ADDR_PINS_MASK 0xffff // PC0:15

#define IO_DEF(_state, _port, _pin) \
	void set_ ## _state ## _dir(Direction dir, bool pullup) { \
		GPIO_InitTypeDef init { \
			.Pin = _pin, \
			.Mode = (dir == Direction::Input) ? GPIO_MODE_INPUT : GPIO_MODE_OUTPUT_PP, \
			.Pull = (pullup) ? GPIO_PULLUP : GPIO_NOPULL, \
			.Speed = GPIO_SPEED_FREQ_LOW, \
		}; \
		HAL_GPIO_Init(_port, &init); \
	} \
	bool read_ ## _state() { \
		return HAL_GPIO_ReadPin(_port, _pin); \
	} \
	void write_ ## _state(bool level) { \
		HAL_GPIO_WritePin(_port, _pin, static_cast<GPIO_PinState>(level)); \
	}


namespace gpio {
	void set_addr_bus_mode(AddressBusMode mode) {
		switch (mode) {
			case AddressBusMode::CpuDriven:
				set_addr_bus_dir(Direction::Input);
				write_be(true);
				break;
			case AddressBusMode::DebuggerDriven:
				write_be(false);
				set_addr_bus_dir(Direction::Output);
				break;
		}
	}

	static uint8_t reverse_u8(uint8_t x) {
		// TODO: rbit?
		x = ((x & 0xf0) >> 4) | ((x & 0x0f) << 4);
		x = ((x & 0xcc) >> 2) | ((x & 0x33) << 2);
		x = ((x & 0xaa) >> 1) | ((x & 0x55) << 1);
		return x;
	}

	void set_data_bus_dir(Direction dir) {
		GPIO_InitTypeDef init {
			.Pin = DATA_PINS_MASK,
			.Mode = (dir == Direction::Input) ? GPIO_MODE_INPUT : GPIO_MODE_OUTPUT_PP,
			.Pull = GPIO_NOPULL,
			.Speed = GPIO_SPEED_FREQ_LOW,
		};
		HAL_GPIO_Init(GPIOB, &init);
	}

	void write_data_bus(uint8_t data) {
		uint32_t out = GPIOB->ODR;
		out &= ~DATA_PINS_MASK;
		out |= reverse_u8(data) << 2;
		GPIOB->ODR = out;
	}

	uint8_t read_data_bus() {
		return reverse_u8(static_cast<uint8_t>(GPIOB->IDR >> 2));
	}

	void set_addr_bus_dir(Direction dir) {
		GPIO_InitTypeDef init {
			.Pin = ADDR_PINS_MASK,
			.Mode = (dir == Direction::Input) ? GPIO_MODE_INPUT : GPIO_MODE_OUTPUT_PP,
			.Pull = GPIO_NOPULL,
			.Speed = GPIO_SPEED_FREQ_LOW,
		};
		HAL_GPIO_Init(GPIOC, &init);
	}

	void write_addr_bus(uint16_t data) {
		GPIOC->ODR = static_cast<uint32_t>(data);
	}

	uint16_t read_addr_bus() {
		return static_cast<uint16_t>(GPIOC->IDR);
	}

	status_t read_status() {
		return {
			static_cast<uint16_t>(
				(read_mlb() << 15) |
				(read_irqb() << 14) |
				(read_rdy() << 11) |
				(read_vpb() << 10) |
				(read_progb() << 9) |
				(read_we() << 8) |
				(read_rwb() << 5) |
				(read_be() << 4) |
				(read_phi2() << 3) |
				(read_resb() << 2) |
				(read_sync() << 1) |
				read_nmib()
			)
		};
	}

	IO_DEF(mlb,   GPIOA, GPIO_PIN_6)
	IO_DEF(irqb,  GPIOA, GPIO_PIN_5)
	IO_DEF(progb, GPIOD, GPIO_PIN_1)
	IO_DEF(phi2,  GPIOA, GPIO_PIN_8)
	IO_DEF(rwb,   GPIOD, GPIO_PIN_2)
	IO_DEF(sync,  GPIOB, GPIO_PIN_0)
	IO_DEF(vpb,   GPIOA, GPIO_PIN_1)
	IO_DEF(resb,  GPIOD, GPIO_PIN_0)
	IO_DEF(nmib,  GPIOA, GPIO_PIN_7)
	IO_DEF(be,    GPIOA, GPIO_PIN_15)
	IO_DEF(we,    GPIOA, GPIO_PIN_0)
	IO_DEF(rdy,   GPIOA, GPIO_PIN_4)
}
