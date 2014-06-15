#include "Cosa/Types.h"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Max7219.hh"

//SPI spi;
//Max7219Driver::SPI3W maxSpi;
//Max7219Driver driver(&maxSpi);
Max7219Driver::Serial3W maxS3W;
Max7219Driver driver(&maxS3W);
Max7219 display(&driver);
IOStream out(&display);

void setup()
{
	uart.begin(115200);
	trace.begin(&uart);
	Watchdog::begin();
	driver.intensity(0, 8);
	driver.shutdownMode(0, false);
	Watchdog::delay(2048);
}

const char TESTS[] = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
		'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
		'"', '`', '\'', '[', ']', '|', '_', '-', '=', '?', '!', '.'
};

const uint8_t TESTS_SIZE = (sizeof(TESTS) / sizeof(char));

void loop()
{
	static uint8_t index = 0;

	trace << TESTS[index] << endl;
	display.putchar(TESTS[index]);
//	out << TESTS[index];
	index = (index + 1) % TESTS_SIZE;

	Watchdog::delay(256);
}
