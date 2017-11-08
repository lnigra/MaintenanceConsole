#include "Arduino.h"
#include <SD.h>
#include <SPI.h>
#include "SD_xsftp.h"

SD_xsftp::SD_xsftp( int sd_csel, int ser_port_no, int ser_port_bps, int blocking ) {
  
  port.begin( ser_port_bps );
  while (!port);
  while ( port.available() ) port.read();

  port.print("Initializing SD card for xsftp...");

  if ( !SD.begin( sd_csel ) ) {
    port.println("failed!");
    return;
  } else {
    port.println("done." );
  }
  _root = SD.open( "/" );
  _ftpGetFlg = false;
  _cmdFlg = false;
  _echoFlg = false; 
}

boolean SD_xsftp::xsftp() {
  if ( _ftpGetFlg ) _getFileBlock();
  if ( _cmdFlg ) _processCmd();
  if ( port.available() ) _getRemoteByte();
  return false;
}


void SD_xsftp::_getRemoteByte() {
  char inByte = port.read();
  if ( !_cmdFlg ) {
    if ( inByte == '\r' || inByte == '\n' ) {
      if ( _lineEndFlg ) {
        _lineEndFlg = false;
      } else {
        _lineEndFlg = true;
        _cmdFlg = true;
        if ( _echoFlg ) port.write( '\n' );
      }
    } else {
      _cmdStr += inByte;
      if ( _echoFlg ) port.write( inByte );
      _lineEndFlg = false;
    }
  }
}

void SD_xsftp::_processCmd() {
  String remCode = _getField( _cmdStr, 0, ' ' );
  String args = _getField( _cmdStr, 1, ' ' );
  char tmp[32];
  args.toCharArray( tmp, 32 );
  _cmdFlg = false;
  _cmdStr = "";
  remCode.toUpperCase();
  if ( _cmdModeFlg ) {
    if ( _ftpMode ) {
      if ( remCode == "LS" ) {
        port.print("\n");
        _listDirectory( _root, 0 );
        port.print( "\nFTP> " );
        
  /* CD doesn't work right */
  //    } else if ( remCode == "CD" ) {
  //      if ( SD.open( tmp ).isDirectory() ) {
  //        root = SD.open( tmp );
  //      } else {
  //        port.println( "" );port.print( tmp );port.println( "is not a directory." );
  //      }
  //      port.print( "\nFTP> " );
  
      } else if ( remCode == "BYE" ) {
        port.println( "Leaving FTP Mode\n" );
        port.print( "\n> " );
        _ftpMode = false;
      } else if ( remCode == "GET" ) {
        _file = SD.open( tmp );
        if ( _file ) {
          unsigned long fileSize = _file.size();
          port.write( 1 ); // Send file exists flag to client
          for ( int i = 0 ; i < 4 ; i++ ) {
            port.write( (byte)( ( fileSize >> ( 8 * (3 - i) ) ) & 255 ) );
          }
          _ftpGetFlg = true;
        } else {
          port.write( 0 ); // Send file not found flag to client
          port.print( "\nCan't open file :" ); port.println( tmp );
          port.print( "\nFTP> " );
        }
      } else {      
        port.print( "\nFTP> " );
      }
    }

    if ( !_ftpMode ) {
      if ( remCode == "FTP" ) {
        port.println( "\n\nEntering FTP Mode..." );
        _ftpMode = true;
        _root = SD.open( "/" );
        port.print( "\nFTP> " );
      } else if ( remCode == "ECHO" ) {
        if ( strcmp( tmp, "on" ) == 0 ) _echoFlg = true;
        if ( strcmp( tmp, "off" ) == 0 ) _echoFlg = false;
        port.print( "\n> " );
      } else if ( remCode == "EXIT" ) {
        _cmdModeFlg = false;
        _echoFlg = false;
      } else {
        port.print( "\n> " );
      }
    }
  } else {
    if ( remCode == "CMD" ) {
      _cmdModeFlg = true;
      port.println( "\nEntering command mode" );
      port.print( "\n> " );
    }
  }
}

void SD_xsftp::_listDirectory(File dir, int numTabs) {
  dir.rewindDirectory();
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      port.print('\t');
    }
    port.print(entry.name());
    if (entry.isDirectory()) {
      port.println("/");
    } else {
      // files have sizes, directories do not
      port.print("\t\t");
      port.println(entry.size(), DEC);
    }
    entry.close();
  }
  dir.rewindDirectory();
}

void SD_xsftp::_getFileBlock() {
  for ( int i = 0 ; i < 512 ; i++ ) {
    if ( _file.available() ) {
      port.write( _file.read() );
    } else {
      _ftpGetFlg = false;
      _file.close();
      port.print( "\nFTP> " );
      break;
    }
  }
}

String SD_xsftp::_getField( String str, int fieldIndex, char delim ) {
  int startIndex = 0;
  int endIndex = -1;
  for ( int i = 0 ; i <= fieldIndex ; i++ ) {
    startIndex = endIndex + 1;
    endIndex = str.indexOf( delim, startIndex );
  }
  if ( endIndex == -1 ) endIndex = str.length();
  return str.substring( startIndex, endIndex );
}

