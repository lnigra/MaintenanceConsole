/*
 * TO-DO:
 */


/* 
 * Before including Maint.h, #define is needed to let Maint.h header
 * know the class name of the serial port being used for the maintenance
 * interface.
 * 
 * For Teensy 3+ There are two classes:
 * 
 *    usb_serial_class --> USB Serial Port ("Serial")
 *    HardwareSerial --> Numbered Serial Port ("Serial1","Serial2",...) 
*/
#define MAINT_PORT_CLASS HardwareSerial

/*
 * There's no need to #include the SPI.h or SD.h libraries here in the 
 * main program as Maint.h does that for you. However, standard libraries
 * ignore duplicate #includes, so it doesn't hurt to include them here
 * to make your code more understandable for instance.
 */
#include "Maint.h"

int loopCount;
boolean outputFlg;
// Hard-wired on Teensy 3.5, 3.6. If < 3.5, select a pin
const int sdChipSelect = BUILTIN_SDCARD;

void setup() {
  
  Serial.begin(9600);
  while (!Serial);
  while ( Serial.available() ) Serial.read();

  Serial1.begin( 115200 );
  while (!Serial1);
  while ( Serial.available() ) Serial1.read();
  
  Serial.print( "Initializing SD card..." );
  if ( !SD.begin( sdChipSelect ) ) {
    Serial.println( "initialization failed" );
    Serial.println( " NO SD Card or bad configuration " );
  } else {
    Serial.println( "done" );
  }

  /* 
   *  The Maintenance interface form used here is the simplest, but
   *  assumes that you've set up the chosen serial port and the SD card
   *  here in the main program first. There's another form that does that
   *  for you, but we'll use the simpler form in this example.
   */
  boolean mntStatus = MaintInit( Serial1 );
  if (!mntStatus) {
    Serial.println( _mntStatusMsg );
  }
}

void loop() {

/* 
 *  maintCheck() is a task-sampling function, processing one serial port command 
 *  input byte (if any are available) and returning: 
 *    -- null String.
 *    -- unrecognized internal command String while in maintenance mode so main
 *       program can process custom commands.
 *    -- The String "MNT" indicating it's entered maintenance mode.
 *    -- The String "EXIT" indicating it's left maintenance mode.
 *       
 *  The function only adds a delay of about one character length to the loop
 *  or 10/baudrate. So, the function can be used quite simply without much impact 
 *  if the main program loop() is of short duration. The length of the loop will
 *  create corresponding lag in response to command entry and text output. So, if
 *  loop is short, simply insert this somewhere in your loop :
*/

//  String cmd;
//  if ( !( cmd = maintCheck()).equals( "" ) ) {
//    // Here's where you can process a custom command.
//    // If an unrecognized string is entered, cmd will
//    // contain the uppercase version of it.
//    Serial.println( cmd );
//  }
  
/*
 *  If you can't tolerate the lag, and you can afford to indefinitely interrupt
 *  your loop when command mode is entered and resume when you're done you
 *  can use this "trap" to detect whether you've entered maintenance mode 
 *  by entering "mnt".
 *  
 *  It delays your loop by about 5 character times, or 50/baudrate each cycle.
 *  If it detects that you've entered maintenance mode, it doesn't return to the 
 *  loop() sequence until you've left by entering "exit"
*/
  String cmd;
  for ( int i = 0 ; i < 5 ; i++ ) { // 5 characters including NL+CR for "MNT"
    cmd = maintCheck();
    if ( cmd.equals( "MNT" ) ) {
      //Serial.println( "maint mode" );
      while ( !( cmd.equals( "EXIT" ) ) ) {
        if ( !( cmd = maintCheck() ).equals( "" ) ) {
          // Here's where you can process a custom command
          // if an unrecognized string is entered, cmd will
          // contain the uppercase version of it.
          Serial.println( cmd );
        }
      }
    }
  }
  delay( 200 ); // Simulates other stuff in the loop()
  loopCount++;
  if ( loopCount % 10 == 0 ) Serial.println( loopCount );
}
