#include "SD_xsftp.h"
File myFile;
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
  // Teensy 3.5-6 internal SD card
  const int sdChipSelect = BUILTIN_SDCARD;
#else
  // External SD card: Need to specify the pin used
  const int sdChipSelect = <IF SD NOT built-in, change this to the assigned pin>;
#endif

SD_xsftp mySDftp( sdChipSelect, 0, 9600, 0 );

void setup() {
  /* 
   * the xsftp port can be written to as usual... 
   */
  mySDftp.port.println( "hello 'mySDftp.port'" );

  /*
   * ...or you can writeto that terminal by its default name: 
   */
  Serial.println( "hello 'Serial'" );
  
  /* 
   * To use the SD card in your program, do it the usual way: 
   */
  if ( !SD.begin( sdChipSelect ) ) {
    Serial.println("SD initialization failed!");
    return; 
  }
  myFile = SD.open( "myfile.txt", FILE_WRITE );
  if ( myFile ) {
    Serial.println( "myFile is ready to go" );
  } else {
    mySDftp.port.println( "myFile is no-go" );
  }
  myFile.println( "this is the first line" );
  myFile.flush();

  /*
   * NOTE:
   * Instead of closing and opening the SD card or the file 
   * between writes (as in most SD library examples), the flush()
   * method is used after each write and the SD and file are kept
   * open.
   * 
   * This is the proper and efficient way to make sure the data is 
   * written to the file before moving on to the next line of code.
   */
  
}

void loop() {
  mySDftp.xsftp();
}


