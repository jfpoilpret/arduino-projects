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
	void transfer(uint8_t chip, uint8_t opcode, uint8_t data);

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

class Max7219 : public IOStream::Device {
public:
	Max7219(Max7219Driver* driver, uint8_t chip = 0):m_driver(driver), m_chip(chip), m_x(0) {}

	void clear();

	virtual int putchar(char c);

protected:
	Max7219Driver* const m_driver;
	uint8_t m_chip;
    uint8_t m_x;
};

#endif /* MAX7219_HH_ */
