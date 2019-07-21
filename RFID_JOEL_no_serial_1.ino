// for PN532 in I2C mode
#include <PN532_I2C.h>
#include <PN532.h>
#include <Wire.h>
#define RGB_OFF LOW
#define RGB_ON HIGH
#include<LiquidCrystal.h>
#include <SoftwareSerial.h>

LiquidCrystal lcd(12, 11, A0, A1, A2, A3);
SoftwareSerial ArduinoUno(7, 8);
String f;
int i = 0;
int j = 0;
int k = 0;

PN532_I2C pn532i2c( Wire ) ;                                          // initialize PN532 over I2C
PN532 nfc(pn532i2c) ;

// Indicate good or bad scan with blue or red led
const byte bluePin = 5; bool bluePinState = false ;
const byte redPin = 6; bool redPinState = false ;

const byte ledDelay = 50 ;                                            // led blink delay

// variables for denoting student state
bool student1in = false ;
bool student2in = false ;
bool student3in = false ;

uint8_t count = 0 ;

uint8_t success = 0;                                                   // Correct read?
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };                               // Buffer to store the returned UID
uint8_t uidLength = 0 ;                                                // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

// uid required to access masterMode() and add/remove tags and cards

const uint8_t uidsKnown[3][4]                                            // Change uidKnownCount(3) to the number of UIDs in the multidimensional array
{
  0x5D, 0xE0, 0xD0, 0x83, // student 1
  0x87, 0x6A, 0x06, 0xBE, // student 2
  0x45, 0x9C, 0x0E, 0x2B  // student 3
} ;
// Match found?
bool uidMatch = false ;
const uint8_t uidKnownCount = 3 ;                                       // no of students
const uint8_t uidKnownLength = 4 ;
const int DOUTpin = 9;   // alcohol sensor pin
const int swpin = 10;    // motor switch pin

int digital;
int Latch;
int sw1 = 0;
int enA = 2;
int in1 = 3;
int in2 = 4;

// toggle student functions for switching the status (in/out) for each student
void togglestudent1( void )
{
  if ( student1in )
  {
    student1in = false ;
 //   Serial.println("Anil out");
    lcd.setCursor(0, 1);
    lcd.print("Anil out");
    i = 0;
    delay (700);
  }
  else if ( !student1in )
  {
    student1in = true ;
 //   Serial.println("Anil in");
    lcd.setCursor(0, 1);
    lcd.print("Anil in");
    i = 1;
    delay (700);
  }
}


void togglestudent2( void )
{
  if ( student2in )
  {
    student2in = false ;
  //  Serial.println("Tresa out");
    lcd.setCursor(0, 1);
    lcd.print("Tresa out");
    j = 0;
    delay (700);
  }
  else if ( !student2in )
  {
    student2in = true ;
  //  Serial.println("Tresa in");
    lcd.setCursor(0, 1);
    lcd.print("Tresa in");
    j = 1;
    delay (700);
  }
}


void togglestudent3( void )
{
  if ( student3in )
  {
    student3in = false ;
 //   Serial.println("Aiswarya out");
    lcd.setCursor(0, 1);
    lcd.print("Aiswarya out");
    k = 0;
    delay (700);
  }
  else if ( !student3in )
  {
    student3in = true ;
//    Serial.println("Aiswarya in");
    lcd.setCursor(0, 1);
    lcd.print("Aiswarya in");
    k = 1;
    delay (700);
  }
}

void blinkRed( byte blinkCount )
{
  for ( byte it = 0 ; it < blinkCount ; it++ )
  {
    digitalWrite( redPin, RGB_ON ) ;
    delay( ledDelay ) ;
    digitalWrite( redPin, RGB_OFF ) ;
    delay( ledDelay ) ;
  }
}

void blinkBlue( byte blinkCount )
{
  for ( byte it = 0 ; it < blinkCount ; it++ )
  {
    digitalWrite( bluePin, RGB_ON ) ;
    delay( ledDelay ) ;
    digitalWrite( bluePin, RGB_OFF ) ;
    delay( ledDelay ) ;
  }
}

void setup( void )
{
  // i = 0;
  // j = 0;
  // k = 0;
  pinMode(enA, OUTPUT);  // pins for motor driver
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(13, OUTPUT);
  Latch = 1;
  pinMode(DOUTpin, INPUT);
  pinMode(swpin, INPUT);

  lcd.begin(16, 2);
  lcd.print("Bus Monitoring");
  lcd.setCursor(0, 1);
  lcd.print("     System      ");
  delay(500);
  
  pinMode( bluePin, OUTPUT ) ; digitalWrite( bluePin, RGB_OFF ) ;
  pinMode( redPin, OUTPUT ) ; digitalWrite( redPin, RGB_OFF ) ;

  Serial.begin( 9600 ) ;      
  ArduinoUno.begin(9600);       //for starting serial communication with node mcu
  delay( 300 ) ;

//nfc 
  nfc.begin() ;
  uint32_t versiondata = nfc.getFirmwareVersion() ;
  if ( ! versiondata)
  {
  Serial.print( F( "Didn't find PN53x board\n" ) ) ;
    //   delay(500);
    while ( 1 ) {
      lcd.clear();
      lcd.print("Chip not found!");
      blinkRed( 1 ) ;  // Halt
    }
  }
  lcd.clear();
  lcd.print("Found chip");
  delay( 400 ) ;
  
//to print details of chip
Serial.print( F( "Found chip PN5") ) ;
Serial.println( ( versiondata >> 24 ) & 0xFF, HEX ) ;
Serial.print( F( "Firmware ver. ") ) ;
Serial.print((versiondata >> 16 ) & 0xFF, DEC ) ;
Serial.print( F( "." ) ) ; Serial.println( ( versiondata >> 8 ) & 0xFF, DEC ) ;

  // Configure board to read RFID tags
  nfc.SAMConfig() ;
  return ;
}

void loop( void )
{
  digital = digitalRead(DOUTpin);    //for reading the output of alcohol sensor
  // once the alcohol sensor returns 0, then the latch is set(latch = 0) until the arduino is reset
  if (digital == 0) {
    Latch = 0;                       
  }
  sw1 = digitalRead(swpin);
//  Serial.print("digital: ");
//  Serial.print(digital);
// Serial.print("sw: ");
// Serial.print(sw1);

  if (Latch == 1 && sw1 == 1)   
  {
    // motor on
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(enA, HIGH);
    digitalWrite(13, HIGH);
  }
  else
 //if (Latch == 0 || sw1 == 0)
  {
    //motor off
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(13, LOW);
  }

  success = 0 ;

  uidLength = 0 ; // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  for ( int it = 0 ; it < 7 ; it++ ) // Clear UID buffer on each iteration of loop()
  uid[it] = 0 ;
  uidMatch = false ;
  lcd.clear();
  lcd.print("Reader ready");
  success = nfc.readPassiveTargetID( PN532_MIFARE_ISO14443A, uid, &uidLength ) ;

  if ( success )
  {
// Display some basic information about the card
    Serial.println( F( "Found an ISO14443A card") ) ;
    Serial.print( F("  UID Length: ") ) ; Serial.print(uidLength, DEC) ; Serial.println( F(" bytes" ) ) ;
    Serial.print( F("  UID Value: " ) ) ; nfc.PrintHex( uid, uidLength ) ; Serial.print( F("\n") ) ;
    lcd.clear();
    lcd.print("Found a card");

    if ( uidLength == 4 )
    {

 //     Serial.println( F( "Seems to be a Mifare Classic card (4 byte UID)" ) ) ;
      lcd.setCursor(0, 1);
      lcd.print("Authenticating..");

      // To authenticate it for read/write access
      // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      // Serial.println( F( "Trying to authenticate block 4 with default KEYA value" ) ) ;
      PROGMEM uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

      // Start with block 4 (the first block of sector 1) since sector 0
      // contains the manufacturer data and it's probably better just
      // to leave it alone unless you know what you're doing
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya) ;

      if ( success )
      {
  //      Serial.println( F("Sector 1 (Blocks 4..7) has been authenticated" ) ) ;
        uint8_t data[16];

        // If you want to write something to block 4 to test with, uncomment
        // the following line and this text should be read back in a minute
        // data = { 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0};
        // success = nfc.mifareclassic_WriteDataBlock (4, data) ;

        // Try to read the contents of block 4
        success = nfc.mifareclassic_ReadDataBlock( 4, data ) ;

        if ( success )
        {

          lcd.home();
          lcd.setCursor(0, 1);
          lcd.print("Authenticated!");
          // Data seems to have been read ... spit it out
          // Serial.println( F( "Reading Block 4:" ) ) ;
          // nfc.PrintHexChar( data, 16 ) ;
          // Serial.print( F( "\n" ) ) ;

          // Wait a bit before reading the card again
          delay( 500 ) ;
        }
        else
        {
          //Serial.println( F( "Ooops ... unable to read the requested block.  Try another key?" ) ) ;
        }
      }
      else
      {
        lcd.home();
        lcd.setCursor(0, 1);
        lcd.print("Failed!         ");
        //Serial.println( F( "Ooops ... authentication failed: Try another key?") ) ;
      }
    }

    // Check each UID in uidsKnown array against the read UID, un/lock and break if one is found
    if ( uidKnownLength == uidLength )
    {
      for ( byte it = 0 ; it < uidKnownCount ; it++ )
      {
        Serial.print( F( "Check for user #" ) ) ;
        Serial.print( it + 1, DEC ) ;
        Serial.print( F( ", UID = " ) ) ;
        Serial.print( uidsKnown[it][0], HEX ) ;
        Serial.print( F( " " ) ) ;
        Serial.print( uidsKnown[it][1], HEX ) ;
        Serial.print( F( " " ) ) ;
        Serial.print( uidsKnown[it][2], HEX ) ;
        Serial.print( F( " " ) ) ;
        Serial.print( uidsKnown[it][3], HEX ) ;
        Serial.print( F( "\n" ) ) ;

        // Check each row in uidsKnown array here, break from block if a match is found
        if ( uidsKnown[it][0] == uid[0] && uidsKnown[it][1] == uid[1] && uidsKnown[it][2] == uid[2] && uidsKnown[it][3] == uid[3] )
        {
       Serial.print( F( "Match found!...\n" ) ) ;
          blinkBlue( 4 ) ;
          uidMatch = true ;
          lcd.clear();
          lcd.print("Match Found");
          delay (700);

          if ( uidsKnown[0][0] == uid[0] && uidsKnown[0][1] == uid[1] && uidsKnown[0][2] == uid[2] && uidsKnown[0][3] == uid[3] )
          {
            togglestudent1() ;
          }

          if ( uidsKnown[1][0] == uid[0] && uidsKnown[1][1] == uid[1] && uidsKnown[1][2] == uid[2] && uidsKnown[1][3] == uid[3] )
          {
            togglestudent2() ;
          }

          if ( uidsKnown[2][0] == uid[0] && uidsKnown[2][1] == uid[1] && uidsKnown[2][2] == uid[2] && uidsKnown[2][3] == uid[3] )
          {
            togglestudent3() ;
          }

          count = (student1in + student2in + student3in);
          f = String('a') + String(i) + String('b') + String(j) + String('c') + String(k) + String('T') + String(count);
          Serial.print("data send= ");
          Serial.println(f);
          ArduinoUno.println(f);

          lcd.clear();
          lcd.print("students in bus=");
          lcd.setCursor(0, 1);
          lcd.print(count);
          delay (700);
          break ;
        }
      }
      if ( ! uidMatch )
      { blinkRed( 5 ) ;
        lcd.clear();
        lcd.print("Unknown Card");
        delay (700);
      }
    }

  } // end if( success )


  return ;
}

void toggleblueLED( void )
{
  if ( redPinState )
  {
    digitalWrite( redPin, RGB_OFF ) ;
    redPinState = ! redPinState ;
  }
  if ( ! bluePinState )
  {
    digitalWrite( bluePin, RGB_ON ) ;
    bluePinState = ! bluePinState ;
  }
  else
  {
    digitalWrite( bluePin, RGB_ON ) ;
    bluePinState = ! bluePinState ;
  }
}

void toggleRedLED( void )
{
  if ( bluePinState )
  {
    digitalWrite( bluePin, RGB_OFF ) ;
    bluePinState = ! bluePinState ;
  }
  if ( ! redPinState )
  {
    digitalWrite( redPin, RGB_ON ) ;
    redPinState = ! redPinState ;
  }
  else
  {
    digitalWrite( redPin, RGB_OFF ) ;
    redPinState = ! redPinState ;
  }
}
