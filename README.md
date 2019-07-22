# MaintenanceConsole

A library for Teensy 3+ that implements a serial port interface that acts as a maintenance console with features:

* A built-in, extremely simple ftp service for SD card file management.
* Assign console to any non-software serial interface
* Pass custom commands to main program for processing
* Optional flag to use for suppressing main program output when in maintenance mode.
* Two initialization function forms:
  * One only requires the name of the serial port to be used for maintenance interface.
    * Main program must initialize SD card and maintenance port.
  * Another also requires serial port baud rate, SD chip select pin, and a boolean flag for output enabling (need not be used)
    * Main program doesn't initialize SD card or maintenance port.

For project status see the [Wiki](https://github.com/lnigra/MaintenanceConsole/wiki)
