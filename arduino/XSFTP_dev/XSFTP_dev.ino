/*
 * TO-DO:
 * - Convert to library
 */

#include <SPI.h>
#include <SD.h>
File sdRoot, sdFile;

boolean maintCmdFlg, maintLineEndFlg, maintFtpModeFlg, maintFtpGetFlg, maintEchoFlg,maintModeFlg;

String maintCmdStr;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  while ( Serial.available() ) Serial.read();

  Serial.print("Initializing SD card...");

  if ( !SD.begin( BUILTIN_SDCARD ) ) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
}

void loop() {
  if ( maintFtpGetFlg ) getFileBlock();
  if ( maintCmdFlg ) procMaintCmd();
  if ( Serial.available() ) getMaintByte();
}

void getMaintByte() {
  char inByte = Serial.read();
  if ( !maintCmdFlg ) {
    if ( inByte == '\r' || inByte == '\n' ) {
      if ( maintLineEndFlg ) {
        maintLineEndFlg = false;
      } else {
        maintLineEndFlg = true;
        maintCmdFlg = true;
        if ( maintEchoFlg ) Serial.write( '\n' );
      }
    } else {
      maintCmdStr += inByte;
      if ( maintEchoFlg ) Serial.write( inByte );
      maintLineEndFlg = false;
    }
  }
}

void procMaintCmd() {
  String remCode = getField2( maintCmdStr, 0, ' ' );
  String args = getField2( maintCmdStr, 1, ' ' );
  char tmp[32];
  args.toCharArray( tmp, 32 );
  maintCmdFlg = false;
  maintCmdStr = "";
  remCode.toUpperCase();
  if ( maintModeFlg ) {
    if ( maintFtpModeFlg ) {
      if ( remCode == "LS" ) {
        Serial.print("\n");
        listDirectory( sdRoot, 0 );
        Serial.print( "\nFTP> " );
  //    } else if ( remCode == "CD" ) {
  //      if ( SD.open( tmp ).isDirectory() ) {
  //        sdRoot = SD.open( tmp );
  //      } else {
  //        Serial.println( "" );Serial.print( tmp );Serial.println( "is not a directory." );
  //      }
  //      Serial.print( "\nFTP> " );
      } else if ( remCode == "BYE" ) {
        Serial.println( "Leaving FTP Mode\n" );
        Serial.print( "\n> " );
        maintFtpModeFlg = false;
      } else if ( remCode == "GET" ) {
        sdFile = SD.open( tmp );
        if ( sdFile ) {
          unsigned long fileSize = sdFile.size();
          Serial.write( 1 ); // Send file exists flag to client
          for ( int i = 0 ; i < 4 ; i++ ) {
            Serial.write( (byte)( ( fileSize >> ( 8 * (3 - i) ) ) & 255 ) );
          }
          maintFtpGetFlg = true;
        } else {
          Serial.write( 0 ); // Send file not found flag to client
          Serial.print( "\nCan't open file :" ); Serial.println( tmp );
          Serial.print( "\nFTP> " );
        }
      } else {      
        Serial.print( "\nFTP> " );
      }
    }
  
    if ( !maintFtpModeFlg ) {
      if ( remCode == "FTP" ) {
        Serial.println( "\n\nEntering FTP Mode..." );
        maintFtpModeFlg = true;
        sdRoot = SD.open("/");
        Serial.print( "\nFTP> " );
      } else if ( remCode == "ECHO" ) {
        if ( strcmp( tmp, "on" ) == 0 ) maintEchoFlg = true;
        if ( strcmp( tmp, "off" ) == 0 ) maintEchoFlg = false;
        Serial.print( "\n> " );
      } else if ( remCode == "EXIT" ) {
        maintModeFlg = false;
        maintEchoFlg = false;
      } else {
        Serial.print( "\n> " );
      }
    }
  } else {
    if ( remCode == "CMD" ) {
      maintModeFlg = true;
      Serial.println( "\nEntering maintenance mode" );
      Serial.print( "\n> " );
    }
  }
}

void listDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
  dir.rewindDirectory();
}

void getFileBlock() {
  for ( int i = 0 ; i < 512 ; i++ ) {
    if ( sdFile.available() ) {
      Serial.write( sdFile.read() );
    } else {
      maintFtpGetFlg = false;
      sdFile.close();
      Serial.print( "\nFTP> " );
      break;
    }
  }
}

String getField2( String str, int fieldIndex, char delim ) {
  int startIndex = 0;
  int endIndex = -1;
  for ( int i = 0 ; i <= fieldIndex ; i++ ) {
    startIndex = endIndex + 1;
    endIndex = str.indexOf( delim, startIndex );
  }
  if ( endIndex == -1 ) endIndex = str.length();
  return str.substring( startIndex, endIndex );
}

