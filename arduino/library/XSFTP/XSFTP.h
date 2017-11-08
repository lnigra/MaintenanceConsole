/*
 * SD_xsftp.h - Library to provide SD card service that includes a simple File Transfer Protocol
 * through a selectable serial port to a custom serial terminal client in order to create a local 
 * file copy on the client machine. A standard ftp client terminal will NOT work as the protocol
 * is not compatible.
 * 
 * A simple terminal like Arduino IDE serial terminal can be used when files are text-only but
 * the file contents must be copied from the terminal window and pasted into a file.
 * 
 * The library activates the Serial and SD library objects, so these libraries should not be 
 * included nor their objects initialized in the user's code.
 */

#ifndef SD_xsftp_h
#define SD_xsftp_h

#include "Arduino.h"
#include <SD.h>
#include <SPI.h>

class SD_xsftp {
  
  public:

  // Need to detect class assignment to Serial (apparently) unique to Teensy 3
    #if defined USBserial_h_
      usb_serial_class& port = Serial; // = Serial; //Teensy3+ port0 only apparently
    #else
      HardwareSerial& port = Serial; //Arduino Serial and Teensy Serial1-Serialn
    #endif
    
    SD_xsftp( int sd_csel, int ser_port_no, int ser_port_bps, int blocking );
    boolean xsftp();
    

  private:
    File _root, _file;
    boolean _cmdModeFlg, _cmdFlg, _lineEndFlg, _ftpMode, _ftpGetFlg, _echoFlg;
    String _cmdStr;
    void _getRemoteByte();
    void _processCmd();
    void _listDirectory(File dir, int numTabs);
    void _getFileBlock();
    String _getField( String str, int fieldIndex, char delim );
     
};

#endif
