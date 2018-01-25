/*
 * TO-DO:
 * - Convert to library
 */

#include <TimerOne.h>
#include "Maint.h"

boolean intFlg, sdPresentFlg, outputFlg;
int intCount;
int sdChipSelect = BUILTIN_SDCARD;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  while ( Serial.available() ) Serial.read();
  sdPresentFlg = MaintInit( Serial, sdChipSelect, outputFlg );
  Timer1.initialize( 1000000 );
  Timer1.attachInterrupt( intSvc );
  _maintEchoFlg = false;
  outputFlg = true;
}

void loop() {
  maintCheck();
  if ( intFlg ) procInterrupt();
}

void procInterrupt() {
  intCount++;
  if ( outputFlg ) {
    Serial.print( "Interrupt count:");Serial.println( intCount );
  }
  intFlg = false;
}

void intSvc() {
  intFlg = true;
}

