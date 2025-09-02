#pragma once

#include <stdint.h>
#include "sim.h"

#define IO_DECL(_state) \
	void set_ ## _state ## _dir(Direction dir, bool pullup); \
	bool read_ ## _state(); \
	void write_ ## _state(bool level);

namespace gpio {
	enum class AddressBusMode {
		// Enables the 6502 and causes the debugger to read from the address bus
		CpuDriven,
		// Disables the 6502 and allows the debugger to read from/write to the address bus
		DebuggerDriven,
	};

	void set_addr_bus_mode(AddressBusMode);

	enum Direction {
		Input = 0b0,
		Output = 0b1,
	};

	void set_data_bus_dir(Direction dir);
	void write_data_bus(uint8_t data);
	uint8_t read_data_bus();

	void set_addr_bus_dir(Direction dir);
	void write_addr_bus(uint16_t data);
	uint16_t read_addr_bus();

	void set_progb_dir(Direction dir);

	using status_t = ::bus_status_t;

	status_t read_status();

	IO_DECL(mlb)
	IO_DECL(irqb)
	IO_DECL(progb)
	IO_DECL(phi2)
	IO_DECL(rwb)
	IO_DECL(sync)
	IO_DECL(vpb)
	IO_DECL(resb)
	IO_DECL(nmib)
	IO_DECL(be)
	IO_DECL(we)
	IO_DECL(rdy)
}
