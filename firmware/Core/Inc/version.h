#pragma once

#include <stdint.h>

#define VERSION_YEAR  2025
#define VERSION_MONTH 8
#define VERSION_DAY   1

#define BOARD_REV 3

struct VersionInfo {
	uint16_t year;
	uint8_t  month;
	uint8_t  day;
};

const VersionInfo VERSION = { VERSION_YEAR, VERSION_MONTH, VERSION_DAY };
