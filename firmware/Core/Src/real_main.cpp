// #include "pins.h"
#include "uart.h"
#include "message.h"
#include "gpio.h"
#include "version.h"
#include "programmer.h"

#include <stdio.h>
#include <tusb.h>

#include "physicalw65c02.h"

using gpio::status_t;

#define MAX_BREAKPOINTS 16
struct Breakpoint {
  uint16_t addr;
  bool enabled;
};

enum class DebuggerState {
  Wait, // don't run cpu
  // run until next ...
  StepHalfCycle,
  StepCycle,
  StepInstruction,
  // keep running
  Continue,
  FastMode,
};

#define MAX_COMMAND_BODY 1024
enum class CommandState {
  Header,
  Body,
};

struct State {
  uint32_t cycle = 0;
  bool phi2 = true;
  PhysicalW65C02 cpu = {};
  DebuggerState state = DebuggerState::Wait;
  Breakpoint breakpoints[MAX_BREAKPOINTS] = {{}};
  int8_t num_breakpoints = 0;

  CommandState cmd_state = CommandState::Header;
  size_t cmd_count;
  union StateCmd {
    uint8_t buf[4];
    struct {
      uint8_t magic;
      uint8_t type;
      uint16_t len;
    } fields;
  } cmd_header;
  uint8_t cmd_body[MAX_COMMAND_BODY];
};
static_assert(sizeof(State::StateCmd) == 4);

static void delay_loop(int amount) {
  volatile int i = 0;
  for (i = 0; i < amount; ++i);
}

static void delay_millis(int millis) {
  delay_loop(millis * 12); // TODO
}


static void init() {
  using gpio::Input, gpio::Output;
  gpio::write_nmib(true);
  gpio::write_resb(false);
  gpio::write_phi2(false);
  gpio::write_be(true);
  gpio::write_we(true);

  // gpio::set_nmib_dir(Output, false); // TODO: why
  // TODO: this is needed bc of my HSE skillissue
  // printf("%08lx %08lx\n", RCC->APB2ENR, AFIO->MAPR);
  RCC->APB2ENR |= 1; // turn on AFIO
  RCC->APB2ENR |= 1 << 5; // turn on GPIOD
  RCC->APB2ENR |= 1 << 4; // turn on GPIOC
  AFIO->MAPR |= (1 << 15); // set PD01_RM
  // printf("%04lx %08lx\n", RCC->APB2ENR, AFIO->MAPR);
  delay_loop(1000);
  gpio::setup_rgb();
  gpio::set_resb_dir(Output, false);
  // printf("%08lx\n", GPIOD->IDR);
  // printf("%i\n", gpio::read_resb());
  gpio::set_phi2_dir(Output, false);
  gpio::set_be_dir(Output, false);
  gpio::set_we_dir(Output, false);

  // Enable pull-up on RDY
  gpio::set_rdy_dir(Input, true);
  gpio::set_irqb_dir(Input, true);
  gpio::set_vpb_dir(Input, true);
  // PORTA.PIN3CTRL |= 0b1000;
  // TODO: seems to duplicate the rdy setup?
  // Or maybe part of "GO" button init

  uart::init();
  gpio::set_data_bus_dir(gpio::Direction::Output);
  gpio::write_data_bus(0xEA);

  gpio::set_phi2_dir(gpio::Direction::Output, false);
  gpio::write_phi2(false);

  gpio::set_progb_dir(gpio::Direction::Output, false);

  gpio::write_progb(true); // enable the EEPROM outputs
  gpio::write_we(true);

  gpio::write_be(false); // disable the 6502 buses

  gpio::set_addr_bus_dir(gpio::Direction::Output);
  gpio::set_data_bus_dir(gpio::Direction::Input);

  delay_loop(1000);

  gpio::write_resb(false);

  gpio::set_rwb_dir(gpio::Direction::Output, false);
  gpio::set_sync_dir(gpio::Direction::Output, false);
  gpio::set_vpb_dir(gpio::Direction::Output, false);

  // Do a single clock cycle with RESB held low to make sure all devices acknowledge it.
  gpio::write_phi2(false);
  delay_loop(50);
  gpio::write_phi2(true);
  delay_loop(50);
}

// Returns the index of the breakpoint we hit, or -1 if no breakpoint was hit.
// int has_hit_enabled_breakpoint(uint16_t addr) {
//   for (int8_t i = 0; i < NUM_BREAKPOINTS; ++i) {
//     if (BREAKPOINTS[i].addr == addr && BREAKPOINTS[i].enabled) {
//       return i;
//     }
//   }
//
//   return -1;
// }

static void send_msg(uint8_t type, uint16_t len, uint8_t* body) {
  tud_cdc_write("%", 1);
  tud_cdc_write(&type, 1);
  tud_cdc_write(&len, sizeof len);
  if (len > 0 && body != nullptr) {
    tud_cdc_write(body, len);
  }
  tud_cdc_write_flush();
}

static void handle_command(State* state) {
  switch (state->cmd_header.fields.type) {
    case 'P':
      send_msg('P', 0, nullptr);
      break;
  }
}

static void uart_task(State* state) {
  uint8_t buf[256];
  size_t n = tud_cdc_read(buf, sizeof buf);

  for (size_t i = 0; i < n; i++) {
    switch (state->cmd_state) {
      case CommandState::Header:
        // wait until % to begin header
        if (state->cmd_count == 0) {
          if (buf[i] != '%') continue;
        }

        state->cmd_header.buf[state->cmd_count] = buf[i];
        state->cmd_count++;

        if (state->cmd_count == 4) {
          state->cmd_count = 0;
          // retry header if invalid
          if (state->cmd_header.fields.magic != '%') {
            continue;
          }
          if (state->cmd_header.fields.len > MAX_COMMAND_BODY) {
            continue;
          }
          // get the rest of it or handle
          if (state->cmd_header.fields.len == 0) {
            handle_command(state);
          } else {
            state->cmd_state = CommandState::Body;
          }
        }
        break;
      case CommandState::Body:
        state->cmd_body[state->cmd_count] = buf[i];
        state->cmd_count++;
        if (state->cmd_count >= state->cmd_header.fields.len) {
          handle_command(state);
          state->cmd_state = CommandState::Header;
        }
        break;
    }
  }
}

#if 0
DebuggerState handle_commands(PhysicalW65C02 &cpu, bool phi2) {
  Command cmd;
  while (true) {
    tud_task();
    int result = get_command(cmd);
    if (result == ERR_NO_CMD) {
      return DebuggerState::None;
    } else if (result != 0) {
      continue;
    }

    switch (cmd.ty) {
    case CommandType::Ping:
      uart::put_bytes(reinterpret_cast<const uint8_t*>("Pong!"), 5);
      break;
    case CommandType::WriteEEPROM:
      gpio::set_addr_bus_mode(gpio::AddressBusState::DebuggerDriven);
      gpio::set_data_bus_dir(gpio::Direction::Output);

      for (int i = 0; i < 64; ++i) {
        tud_task();
        programmer::byte_program(cmd.write_eeprom.addr + i, cmd.write_eeprom.data[i]);
      }

      return DebuggerState::Stop;
    case CommandType::SectorErase:
      gpio::set_addr_bus_mode(gpio::AddressBusState::DebuggerDriven);
      gpio::set_data_bus_dir(gpio::Direction::Output);

      programmer::sector_erase(cmd.sector_erase.addr);

      break;
    case CommandType::ReadMemory:
      gpio::write_we(true);
      gpio::set_addr_bus_mode(gpio::AddressBusState::DebuggerDriven);
      gpio::set_data_bus_dir(gpio::Direction::Input);

      delay_loop(5);

      for (uint16_t i = 0; i < cmd.read_memory.len; ++i) {
        tud_task();
        uint16_t addr = i + cmd.read_memory.addr;

        gpio::write_addr_bus(addr);
        delay_loop(5);
        uint8_t data = gpio::read_data_bus();
        uart::put(data);
      }

      gpio::set_data_bus_dir(gpio::Direction::Input);
      gpio::set_addr_bus_mode(gpio::AddressBusState::CpuDriven);
      break;
    case CommandType::SetBreakpoint:
      if (NUM_BREAKPOINTS == MAX_BREAKPOINTS) {
        uart::put(0xFF);
        break;
      }

      uart::put(NUM_BREAKPOINTS);
      BREAKPOINTS[NUM_BREAKPOINTS].addr = cmd.set_breakpoint.addr;
      BREAKPOINTS[NUM_BREAKPOINTS].enabled = true;
      ++NUM_BREAKPOINTS;
      break;
    case CommandType::ResetCpu:
      gpio::write_resb(false);
      WAIT = Wait::HalfCycle;
      cpu.error = false;
      cpu.mode = physicalw65c02::DebuggerState::IMPLIED;
      cpu.oper = physicalw65c02::Oper::W65C02S_OPER_NOP;
      break;
    case CommandType::GetBusState: {
      BusState state = {
        gpio::read_addr_bus(),
        gpio::read_status(),
        gpio::read_data_bus(),
        0
      };

      uart::put_bytes(reinterpret_cast<const uint8_t*>(&state), sizeof(BusState));
      break;
    }
    case CommandType::StepHalfCycle: {
      return DebuggerState::StepHalfCycle;
    }
    case CommandType::StepCycle: {
      return DebuggerState::StepCycle;
    }
    case CommandType::Step: {
      return DebuggerState::StepInstruction;
    }
    case CommandType::Continue: {
      return DebuggerState::Continue;
    }
    case CommandType::PrintInfo: {
      uart::put_bytes(reinterpret_cast<const uint8_t*>(&VERSION), sizeof(VERSION));
      break;
    }
    case CommandType::GetCpuState: {
      physicalw65c02::BusState bus_state;
      cpu.get_bus_state(bus_state);

      CpuState state;
      state.addr = bus_state.addr;
      state.data = (bus_state.rwb ? gpio::read_data_bus() : bus_state.data);
      state.status =
        (bus_state.rwb << 0) | (bus_state.sync << 1) | (bus_state.vpb << 2) | (phi2 << 3) |
        (cpu.in_rst << 4) | (cpu.in_nmi << 5) | (cpu.in_rst << 6) | (bus_state.error << 7);

      state.pc = cpu.pc;
      state.a = cpu.a;
      state.x = cpu.x;
      state.y = cpu.y;
      state.s = cpu.s;
      state.p = cpu.p;

      state.mode = (uint8_t)cpu.mode;
      state.oper = (uint8_t)cpu.oper;
      state.seq_cycle = cpu.seq_cycle;

      uart::put_bytes(reinterpret_cast<const uint8_t*>(&state), sizeof(CpuState));
      break;
    }
    case CommandType::EnterFastState: {
      return DebuggerState::EnterFastState;
      break;
    }
    case CommandType::DebuggerReset: {
      // Wait for TX data register empty
      delay_millis(100);
      NVIC_SystemReset();
      break;
    }
    default:
      break;
    }

    break;
  }

  return DebuggerState::None;
}
#endif

inline void delay_microsecond() {
  asm volatile ("nop");
  asm volatile ("nop");
  asm volatile ("nop");
  asm volatile ("nop");
}


static void do_falling_edge(State* state) {
  // Interrupt signals (IRQB, NMIB, RESB, RDY) are latched on the falling edge.
  status_t status = gpio::read_status();

  state->cycle++;

  state->cpu.tick_cycle({ gpio::read_data_bus(), status.resb(), status.irqb(), status.nmib() });

  gpio::write_phi2(false);

  /* ---- PHI2 is low ---- */

  gpio::set_data_bus_dir(gpio::Direction::Input);

  delay_microsecond();

  gpio::write_resb(true);

  physicalw65c02::BusState cpu_bus_state;
  state->cpu.get_bus_state(cpu_bus_state);

  if (cpu_bus_state.error) {
    hit_breakpoint(0xFF);
  }

  // Address and control lines change immediately after the falling edge of PHI2.
  gpio::write_addr_bus(cpu_bus_state.addr);
  gpio::write_rwb(cpu_bus_state.rwb);
  gpio::write_sync(cpu_bus_state.sync);
  gpio::write_vpb(cpu_bus_state.vpb);

  // TODO
  // if (int8_t i = has_hit_enabled_breakpoint(cpu.pc); i != -1 && cpu_bus_state.sync) {
  // //if (int8_t i = has_hit_enabled_breakpoint(cpu_bus_state.addr); i != -1) {
  //   hit_breakpoint(i);
  // }
}

static void do_rising_edge(State* state) {
  (void)state;
  gpio::write_phi2(true);

  // TODO
  // if (!cpu_bus_state.rwb) {
  //   gpio::write_data_bus(cpu_bus_state.data);
  //   gpio::set_data_bus_dir(gpio::Direction::Output);
  // }
}

static void debugger_task(State* state) {
  // Don't do anything if we have nothing to do
  if (state->state == DebuggerState::Wait) {
    return;
  }

  // Do one half-cycle per iteration
  if (state->phi2) {
    do_falling_edge(state);
    state->phi2 = false;
  } else {
    do_rising_edge(state);
    state->phi2 = true;
  }

  // Change states
  switch (state->state) {
    case DebuggerState::Continue:
      break;
    case DebuggerState::StepHalfCycle:
      state->state = DebuggerState::Wait;
      break;
    case DebuggerState::StepCycle:
      if (state->phi2) {
        state->state = DebuggerState::Wait;
      }
      break;
    case DebuggerState::StepInstruction:
      if (state->phi2 && state->cpu.get_sync()) {
        state->state = DebuggerState::Wait;
      }
      break;
    case DebuggerState::FastMode:
      state->state = DebuggerState::Wait;
      // TODO
      break;
  }
}

extern "C" void real_main() {
  init();

  State state = {};

  while (true) {
    tud_task();
    uart_task(&state);
    debugger_task(&state);
  }
}
