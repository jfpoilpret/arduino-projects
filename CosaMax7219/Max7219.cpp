#include "Max7219.hh"

void Max7219Driver::transfer(uint8_t chip, uint8_t opcode, uint8_t data) {
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

void Max7219::clear() {
	m_driver->clear(m_chip);
	m_x = 0;
}

int Max7219::putchar(char c) {
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
