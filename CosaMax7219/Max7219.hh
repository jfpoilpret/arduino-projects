/*
 * Max7219.hh
 *
 *  Created on: 9 juin 2014
 *      Author: Jean-François
 */

#ifndef MAX7219_HH_
#define MAX7219_HH_

#include <ctype.h>
#include "Cosa/Types.h"
#include "Cosa/SPI.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/Trace.hh"

// First impl of Max7219 driver:
// - SPI based first
// - hardcoded font
class Max7219Driver {
public:
	class IO;

	Max7219Driver(IO *io, int numChips = 1):m_io(io), m_numChips(numChips) {}

	void shutdownMode(uint8_t chip, bool shutdown) {
		transfer(chip, SHUTDOWN, shutdown ? 0x00 : 0x01);
	}

	void displayTestMode(uint8_t chip, bool test) {
		transfer(chip, DISPLAY_TEST, test ? 0x01 : 0x00);
	}

	void scanLimit(uint8_t chip, uint8_t limit) {
		transfer(chip, SCAN_LIMIT, limit < 8 ? limit : 7);
	}

	void intensity(uint8_t chip, uint8_t intensity) {
		transfer(chip, INTENSITY, intensity <= 0x0F ? intensity : 0x0F);
	}

	void decodeMode(uint8_t chip, uint8_t decode) {
		transfer(chip, DECODE_MODE, decode);
	}

	void display(uint8_t chip, uint8_t digit, uint8_t value) {
		if (digit > 7) digit = 7;
		transfer(chip, DIGIT0 + digit, value);
	}

	void clear(uint8_t chip) {
		for (uint8_t opcode = DIGIT0; opcode <= DIGIT7; opcode++) {
			transfer(chip, opcode, 0x00);
		}
	}

	class IO {
	public:
		virtual void begin() = 0;
		virtual void end() = 0;
		virtual void write(uint8_t data) = 0;
		virtual void write(void* buf, size_t size) = 0;
	};

	class Serial3W : public IO {
	public:
		/**
		 * Construct display device driver adapter with given pins.
		 * @param[in] sdin screen data pin (default D6/D0).
		 * @param[in] sclk screen clock pin (default D7/D1).
		 * @param[in] sce screen chip enable pin (default D9/D3).
		 */
#if !defined(BOARD_ATTINY)
		Serial3W(	Board::DigitalPin sdin = Board::D6,
					Board::DigitalPin sclk = Board::D7,
					Board::DigitalPin sce = Board::D9) : m_sdin(sdin, 0), m_sclk(sclk, 0), m_sce(sce, 1) {}
#else
		Serial3W(	Board::DigitalPin sdin = Board::D0,
					Board::DigitalPin sclk = Board::D1,
					Board::DigitalPin sce = Board::D3) : m_sdin(sdin, 0), m_sclk(sclk, 0), m_sce(sce, 1) {}
#endif

		virtual void begin() {
			m_sce.clear();
		}

		virtual void end() {
			m_sce.set();
		}

		virtual void write(uint8_t data) {
			m_sdin.write(data, m_sclk);
		}

		virtual void write(void* buf, size_t size) {
			uint8_t* dp = (uint8_t*) buf;
			while (size--) m_sdin.write(*dp++, m_sclk);
		}

	protected:
		// Display pins and state
		OutputPin m_sdin;		//<! Serial data input
		OutputPin m_sclk;		//<! Serial clock input
		OutputPin m_sce;		//<! Chip enable
	};

	class SPI3W: public IO, public SPI::Driver {
	public:

#if !defined(BOARD_ATTINY)
		SPI3W(Board::DigitalPin sce = Board::D9) : IO(), SPI::Driver(sce) {}
#else
		SPI3W(Board::DigitalPin sce = Board::D3) : IO(), SPI::Driver(sce) {}
#endif

		virtual void begin() {
			spi.begin(this);
		}

		virtual void end() {
			spi.end();
		}

		virtual void write(uint8_t data) {
			spi.transfer(data);
		}

		virtual void write(void* buf, size_t size) {
			spi.write(buf, size);
		}
	};

protected:
	void transfer(uint8_t chip, uint8_t opcode, uint8_t data) {
		if (chip < m_numChips) {
			trace	<< PSTR("transfer(") << chip << PSTR(", ")
					<< hex << opcode << PSTR(", ")
					<< hex << data << PSTR(")") << endl;
			m_io->begin();
			for (uint8_t i = 0; i < chip; i++) {
				m_io->write(NO_OP);
				m_io->write(0x00);
			}
			m_io->write(opcode);
			m_io->write(data);
			m_io->end();
		}
	}

	enum {
		NO_OP = 0x00,
		DIGIT0 = 0x01,
		DIGIT1 = 0x02,
		DIGIT2 = 0x03,
		DIGIT3 = 0x04,
		DIGIT4 = 0x05,
		DIGIT5 = 0x06,
		DIGIT6 = 0x07,
		DIGIT7 = 0x08,
		DECODE_MODE = 0x09,
		INTENSITY = 0x0A,
		SCAN_LIMIT = 0x0B,
		SHUTDOWN = 0x0C,
		DISPLAY_TEST = 0x0F
	} __attribute__((packed));

	IO* const m_io;
	const uint8_t m_numChips;
};

static const uint8_t font[] __PROGMEM = {
	// First ten entries are for 0..9 digits
	0b01111110,
	0b00110000,
	0b01101101,
	0b01111001,
	0b00110011,
	0b01011011,
	0b01011111,
	0b01110000,
	0b01111111,
	0b01111011,

	// Next 26 entries are for a..z letters (lower or upper all the same)
	0b01110111,	// A
	0b00011111,	// b
	0b00001101,	// c
	0b00111101,	// d
	0b01001111,	// E
	0b01000111,	// F
	0b01011110,	// G
	0b00010111,	// h
	0b00010000,	// i
	0b00111000,	// j
	0b01010111,	// k
	0b00001110,	// L
	0b01110110,	// M
	0b00010101,	// n
	0b00011101,	// o
	0b01100111,	// P
	0b01110011,	// q
	0b00000101,	// r
	0b01011011,	// S
	0b00001111,	// t
	0b00011100,	// u
	0b00100111,	// V
	0b00111111,	// W
	0b00010011,	// X
	0b00111011,	// y
	0b01101101,	// Z

	// Next entries are for special characters: " ` ' [ ] | _ - = ? ! .
	0b00100010,	// "
	0b00000010,	// `
	0b00100000,	// '
	0b01001110,	// [
	0b01111000,	// ]
	0b00000110,	// |
	0b00001000,	// _
	0b00000001,	// -
	0b00001001,	// =
	0b11100101,	// ?
	0b10110000,	// !
	0b10000000	// .
};

const uint8_t DIGIT_INDEX = 0;
const uint8_t ALPHA_INDEX = 10;
const uint8_t SPECIAL_INDEX = 10 + 26;

class Max7219 : public IOStream::Device {
public:
	class IO;

	Max7219(Max7219Driver* driver, uint8_t chip = 0):m_driver(driver), m_chip(chip), m_x(0) {
	}

	void clear() {
		m_driver->clear(m_chip);
		m_x = 0;
	}

	virtual int putchar(char c) {
		int8_t index = -1;

		switch (c) {
		case '\n':
		case '\f':
			// clear display
			clear();
			return c;

		case '\b':
			// change cursor
			if (m_x != 0) m_x--;
			return c;

		case ' ':
			m_driver->display(m_chip, m_x, 0x00);
			m_x = (m_x + 1) % 8;
			return c;

		// Next entries are for special characters: " ` ' [ ] | _ - = ? ! .
		case '`':
			index = SPECIAL_INDEX;
			break;

		case '\'':
			index = SPECIAL_INDEX + 1;
			break;

		case '[':
			index = SPECIAL_INDEX + 2;
			break;

		case ']':
			index = SPECIAL_INDEX + 3;
			break;

		case '|':
			index = SPECIAL_INDEX + 4;
			break;

		case '_':
			index = SPECIAL_INDEX + 5;
			break;

		case '-':
			index = SPECIAL_INDEX + 6;
			break;

		case '=':
			index = SPECIAL_INDEX + 7;
			break;

		case '?':
			index = SPECIAL_INDEX + 8;
			break;

		case '!':
			index = SPECIAL_INDEX + 9;
			break;

		case '.':
			index = SPECIAL_INDEX + 10;
			break;

		default:
			if (isalpha(c)) {
				index = ALPHA_INDEX + tolower(c) - 'a';
			} else if (isdigit(c)) {
				index = DIGIT_INDEX + c - '0';
			}
			break;
		}
//		trace << PSTR("putchar(") << c << PSTR(") index = ") << index << PSTR(", m_x = ") << m_x << endl;
		if (index >= 0) {
			uint8_t value = pgm_read_byte(font + index);
//			trace << PSTR("value = ") << hex << value << endl;
			m_driver->display(m_chip, m_x, value);
			m_x = (m_x + 1) % 8;
		}
		return c;
	}

protected:
	Max7219Driver* const m_driver;
	uint8_t m_chip;
    uint8_t m_x;
};

#endif /* MAX7219_HH_ */
