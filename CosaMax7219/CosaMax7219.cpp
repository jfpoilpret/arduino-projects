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
	Watchdog::delay(1);
}

void loop()
{
//	out << PSTR("Arduino!");
	out << PSTR("Arduino!") << endl;
	Watchdog::delay(512);
//	out << PSTR("The best");
	out << PSTR("The best") << endl;
	Watchdog::delay(512);
}
