/*
 TweetBox
 

 created 2011
 by Guitool <@Guitool>
 for CoWorkingLille <http://coworkinglille.com>
 */
#if defined(ARDUINO) && ARDUINO > 18
#include <SPI.h>
#endif
#include <Ethernet.h>

// IP functions headers
const char* ip_to_str(const uint8_t*);

// Utility function
void p(char *fmt, ... );

// LCD functions headers
void LcdCharacter(char character);
void LcdClear(void);
void LcdInitialise(void);
void LcdString(char *characters);
void LcdWrite(byte dc, byte data);
void gotoXY(int x, int y);
void drawBox(void);
void Scroll(String message);

// RFID functions headers
void SendRfidTag(char* szRfidTag);

// The pins to use on the arduino
const int buttonPin 		= 8;      // pushbutton pin number
const int ledYellowPin 		= 2;     // yellow led pin number
const int potentiometerPin	= A0;	  // potentiometer analog port

// LCD pins
#define PIN_SCE   7
#define PIN_RESET 6
#define PIN_DC    5
#define PIN_SDIN  4
#define PIN_SCLK  3 

// Configuration for the LCD
#define LCD_C     LOW
#define LCD_D     HIGH
#define LCD_CMD   0

// Size of the LCD
#define LCD_X     84
#define LCD_Y     48

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
int oldeventSelection = 0;
boolean blinkYellow = true;
boolean blinkRed = false;
unsigned long previousMillis = 0;     // will store last time LED was updated
int interval = 5;
int currentLedStep = 1;
int intervalLedStep = 1;
int scrollInterval = 300;
unsigned long previousScrollMillis = 0;
int scrollPosition = -10;
unsigned long rfidDataCount = 0;

char rfidBuffer[12];
String tweetMessage;

static const byte ASCII[][5] =
{
    {0x00, 0x00, 0x00, 0x00, 0x00} // 20
    ,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
    ,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
    ,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
    ,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
    ,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
    ,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
    ,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
    ,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
    ,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
    ,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
    ,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
    ,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
    ,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
    ,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
    ,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
    ,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
    ,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
    ,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
    ,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
    ,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
    ,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
    ,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
    ,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
    ,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
    ,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
    ,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
    ,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
    ,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
    ,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
    ,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
    ,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
    ,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
    ,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
    ,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
    ,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
    ,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
    ,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
    ,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
    ,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
    ,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
    ,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
    ,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
    ,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
    ,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
    ,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
    ,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
    ,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
    ,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
    ,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
    ,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
    ,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
    ,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
    ,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
    ,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
    ,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
    ,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
    ,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
    ,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
    ,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
    ,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c ¥
    ,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
    ,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
    ,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
    ,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
    ,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
    ,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
    ,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
    ,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
    ,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
    ,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
    ,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
    ,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
    ,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
    ,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j
    ,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
    ,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
    ,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
    ,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
    ,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
    ,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
    ,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
    ,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
    ,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
    ,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
    ,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
    ,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
    ,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
    ,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
    ,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
    ,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
    ,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
    ,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
    ,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
    ,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e ←
    ,{0x00, 0x06, 0x09, 0x09, 0x06} // 7f →
};

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xDD };
byte server[] = { 192,168,1,103 }; // HTTP Local Server !
byte ip[] = { 192, 168, 1, 253 }; 
byte gateway[] = { 192, 168, 1, 254 }; 
byte subnet[] = { 255, 255, 255, 0 }; 

EthernetClient client;

///////////////////////////////////////////////////////////////////////////////////////////
//

/*
 * Setup
 */
void setup() {
  // initialize the LED pin as an output:
  pinMode(ledYellowPin, OUTPUT);      
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);  
  // Serial communication for Debug/RFID
  Serial.begin(9600);
  memset(rfidBuffer, '\0', 12);
  // LCD bootstrap
  LcdInitialise();
  LcdClear();
  gotoXY(0,0);
  LcdString("  TweetBox  ");
  // Ethernet bootstrap
  Ethernet.begin(mac, ip);
}

///////////////////////////////////////////////////////////////////////////////////////////
//

/*
 *	Main Loop
 */
void loop(){
    unsigned long currentMillis = millis();
    
    ////////////////////////////////////////////////////////////////////////////////////////
    // ETHERNET State Polling
/*    static DhcpState prevState = DhcpStateNone;
    static unsigned long prevTime = 0;
    DhcpState state = EthernetDHCP.poll();
    if (prevState != state) {
        OutputIpAdress();
        prevState = state;
    }
*/  
    ////////////////////////////////////////////////////////////////////////////////////////
    // Blinking led
    if (blinkYellow) {
        if(currentMillis - previousMillis > interval) {
            // save the last step
            previousMillis = currentMillis;
            // change the led intensity
            analogWrite(ledYellowPin, (currentLedStep<0?0:currentLedStep)/6);
            //Serial.println(currentLedStep);
            // calc our next intensity step
            currentLedStep += intervalLedStep;
            if (currentLedStep >= 400) {
                intervalLedStep = -1;
            }
            if (currentLedStep <= -200) {
                intervalLedStep = 1;
            }
        }    
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////
    // Text Scrolling
    if(currentMillis - previousScrollMillis > scrollInterval) {
        // save the lest step
        previousScrollMillis = currentMillis;
        // Text scrolling
        gotoXY(0,5);
        Scroll("CoworkingLille / www.coworkinglille.com");
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////
    // HARDWARE Potentiometer Stuff
    // Work on approx PI angle
    int potarLevel = analogRead(potentiometerPin);
    potarLevel = constrain(potarLevel, 75, 900);
    // Transform level to event array index
    int eventSelection = map(potarLevel, 70, 950, 1, 12);
    // Debug output
    if(eventSelection != oldeventSelection) {
        Serial.print("Selected Message : ");
        Serial.println(eventSelection);
        oldeventSelection = eventSelection;
        // output selection to screen
        gotoXY(0,0);
        LcdString("  TweetBox  ");
        gotoXY(0,2);
        switch(eventSelection) {
          case 1:
             LcdString("Ouverture   ");
             break;
          case 2:
             LcdString("Cafe        ");
             break;
          case 3:
             LcdString("Silence     ");
             break;
          case 4:
             LcdString("Musique     ");
             break;
          case 5:
             LcdString("Lunching    ");
             break;
          case 6:
             {
             // Read temp sensor
             int reading = analogRead(1); 
             float voltage = reading * 5.0;
             voltage /= 1024.0; 
             float temperatureC = (voltage - 0.5) * 100;
             Serial.print(reading); Serial.println(" value");
             Serial.print(voltage); Serial.println(" volts");
             Serial.print(temperatureC); Serial.println(" degrees C");
             }
             LcdString("            ");
             break;
          case 7:
             LcdString("Conference  ");
             break;
          case 8:
             LcdString("Apero       ");
             break;
          case 9:
             LcdString("Reunion     ");
             break;
          case 10:
             LcdString("Atelier     ");
             break;
          case 11:
             LcdString("Fermeture   ");
             break;
        }
    }
    // Convert String to char array
    char pszTweetContent[146];
    tweetMessage.toCharArray(pszTweetContent, sizeof(pszTweetContent));
  
  ////////////////////////////////////////////////////////////////////////////////////////
  // TWITTER BUTTON
  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
        gotoXY(4,3);
        LcdString("   Tweet !  ");
  	Serial.println("Pushed button state...");
    // turn LED on
    analogWrite(ledYellowPin, 250);
    tone(9, 1500, 500);
    // Send Tweet
    if (client.connect(server, 3002)) {								// trying with the HTTP server...
		Serial.println("connected to the HTTP server...");
		// Make our HTTP request:
		String url = "GET /signal/";
		url.concat(eventSelection);
		url.concat(" HTTP/1.0");
		Serial.println(url);
		// and send it to the HTTP server
		client.println(url);
		client.println();
		client.stop();
		// wait a litle bit in case the user fall asleep on the button ;)
		delay(800);
	} else {											// ... else show blinking red led
        // light red led
        analogWrite(ledYellowPin, LOW);
	}
  } else {
    // Return to waiting state
        gotoXY(6,3);
        LcdString("                ");
    analogWrite(ledYellowPin, LOW);
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////
  // RFID SERIAL
  if (Serial.available()) {
      char c = Serial.read();
      char newByte[2];
      newByte[0] = c;
      newByte[1] = '\0';
      // add the last chars to the rfid buffer
      strcat(rfidBuffer, newByte);
      // check for RFID signature
      if (strlen(rfidBuffer) == 11) {
        // did it start with 0x01 0x0b 0x03 ?
        if (rfidBuffer[0] == 0x1 && rfidBuffer[1] == 0xb && rfidBuffer[2] == 0x3) {
          // well formed RFID tag
          Serial.print("RFID data : ");
          Serial.println(rfidBuffer);
          // format the RFID String
          char rfidString[20];
          memset(rfidString, '\0', 15);
            unsigned char b1 = rfidBuffer[3];
            unsigned char b2 = rfidBuffer[4];
            unsigned char b3 = rfidBuffer[5];
            unsigned char b4 = rfidBuffer[6];
            unsigned char b5 = rfidBuffer[7];
          snprintf(rfidString,20,"%02x%02x%02x%02x%02x",b1, b2, b3, b4, b5);
          // show RFID tag on LCD screen
          gotoXY(0,2);
          LcdString(rfidString);
          // send RFID tag to server
          tone(9, 1047, 300);
          SendRfidTag(rfidString);
          // cleanup stuff
          memset(rfidBuffer, '\0', 12);
        } else { // shift current buffer
          char tempBuffer[12];
          memset(tempBuffer, '\0', 12);
          strcpy(tempBuffer, rfidBuffer);
          strcpy(rfidBuffer, tempBuffer+1);
          Serial.println("Rotation du buffer rfid");
        }
      }
      rfidDataCount++;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////
//

// Send RFID tag to server
void SendRfidTag(char* szRfidTag)
{
	// connect ethernet shiel to server
    if (client.connect(server, 3002)) {
		Serial.println("connected to the HTTP server...");
		// Make our HTTP request:
		String url = "GET /member/";
		url.concat(szRfidTag);
		url.concat(" HTTP/1.0 \nAccept: text/plain\n");
		Serial.println(url);
		// and send it to the HTTP server
		client.println(url);
		client.println();
		client.stop();
		// wait a litle bit in case the user fall asleep on the button ;)
		delay(800);
	}
}

// Just a utility function to nicely format an IP address.
const char* ip_to_str(const uint8_t* ipAddr)
{
  static char buf[16];
  sprintf(buf, "%d.%d.%d.%d\0", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
  return buf;
}

///////////////////////////////////////////////////////////////////////////////////////////
//

void p(char *fmt, ... ){
        char tmp[128]; // resulting string limited to 128 chars
        va_list args;
        va_start (args, fmt );
        vsnprintf(tmp, 128, fmt, args);
        va_end (args);
        Serial.print(tmp);
}

///////////////////////////////////////////////////////////////////////////////////////////
//
 
void LcdCharacter(char character)
{
    LcdWrite(LCD_D, 0x00);
    for (int index = 0; index < 5; index++)
    {
        LcdWrite(LCD_D, ASCII[character - 0x20][index]);
    }
    LcdWrite(LCD_D, 0x00);
}

void LcdClear(void)
{
    for (int index = 0; index < LCD_X * LCD_Y / 8; index++)
    {
        LcdWrite(LCD_D, 0x00);
    }
}

void LcdInitialise(void)
{
    pinMode(PIN_SCE,   OUTPUT);
    pinMode(PIN_RESET, OUTPUT);
    pinMode(PIN_DC,    OUTPUT);
    pinMode(PIN_SDIN,  OUTPUT);
    pinMode(PIN_SCLK,  OUTPUT);
    
    digitalWrite(PIN_RESET, LOW);
    digitalWrite(PIN_RESET, HIGH);
    
    LcdWrite(LCD_CMD, 0x21);  // LCD Extended Commands.
    LcdWrite(LCD_CMD, 0xBf);  // Set LCD Vop (Contrast). //B1
    LcdWrite(LCD_CMD, 0x04);  // Set Temp coefficent. //0x04
    LcdWrite(LCD_CMD, 0x14);  // LCD bias mode 1:48. //0x13
    LcdWrite(LCD_CMD, 0x0C);  // LCD in normal mode. 0x0d for inverse
    LcdWrite(LCD_C, 0x20);
    LcdWrite(LCD_C, 0x0C);
}

void LcdString(char *characters)
{
    while (*characters)
    {
        LcdCharacter(*characters++);
    }
}

void LcdWrite(byte dc, byte data)
{
    digitalWrite(PIN_DC, dc);
    digitalWrite(PIN_SCE, LOW);
    shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, data);
    digitalWrite(PIN_SCE, HIGH);
}

/**
 * gotoXY routine to position cursor
 * x - range: 0 to 84
 * y - range: 0 to 5
 */
void gotoXY(int x, int y)
{
    LcdWrite( 0, 0x80 | x);  // Column.
    LcdWrite( 0, 0x40 | y);  // Row.
}

void drawBox(void)
{
    int j;
    for(j = 0; j < 84; j++) // top
    {
        gotoXY(j, 0);
        LcdWrite(1, 0x01);
    }     
    
    for(j = 0; j < 84; j++) //Bottom
    {
        gotoXY(j, 5);
        LcdWrite(1, 0x80);
    }     
    
    for(j = 0; j < 6; j++) // Right
    {
        gotoXY(83, j);
        LcdWrite(1, 0xff);
    }     
    
    for(j = 0; j < 6; j++) // Left
    {
        gotoXY(0, j);
        LcdWrite(1, 0xff);
    }
}

void Scroll(String message)
{
    for (int i = scrollPosition; i < scrollPosition + 11; i++)
    {
        if ((i >= message.length()) || (i < 0))
        {
            LcdCharacter(' ');
        }
        else
        {
            LcdCharacter(message.charAt(i));
        }
    }
    scrollPosition++;
    if ((scrollPosition >= message.length()) && (scrollPosition > 0))
    {
        scrollPosition = -10;
    }
}
