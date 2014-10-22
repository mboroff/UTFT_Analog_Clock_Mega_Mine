// DS1307_Serial_Easy (C)2010 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// A quick demo of how to use my DS1307-library to 
// quickly send time and date information over a serial link
//
// I assume you know how to connect the DS1307.
// DS1307:  SDA pin   -> Arduino Digital 4
//          SCL pin   -> Arduino Digital 5

#include <UTFT.h>
#include <UTouch.h>
#include <UTFT_Buttons.h>
#include <DS1307.h>
#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile

RH_ASK driver(2000, 17);
// RH_ASK driver(2000, 4, 3); // 200bps, TX on D3 (pin 2), RX on D4 (pin 3)
// then:  165 /// Connect D3 (pin 2) as the output to the transmitter  
// Connect D4 (pin 3) as the input from the receiver.

// Init the DS1307
DS1307 rtc(20, 21);
// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];

UTFT myGLCD(ITDB24,38,39,40,41);     //mega 

UTouch        myTouch(6,5,4,3,2);

// Finally we set up UTFT_Buttons :)
UTFT_Buttons  myButtons(&myGLCD, &myTouch);

/**************************************
           General
***************************************/
int but1, but2, butX, butY, pressed_button;
int tx, ty;
int clockCenterX=119;
int clockCenterY=119;

int menuCtr = 0;
boolean digitalDisplay = false;
boolean reVerse = false;
boolean blinkSw = false;
int blankCtr = 0;
/********************************************************
                time
********************************************************/
byte oldsec=0;
int mySecond;
int myMinute;
int myHour;
int mYweekDay;
int mYmonthDay;
int mYmonth;
int mYyear;
int prevSec;
void setup()
{
  // Set the clock to run-mode
  rtc.halt(false);
    rtc.setDate(10, 19, 2014);   // Set the date to October 3th, 2010
   myGLCD.InitLCD();
   myGLCD.setFont(SmallFont); 
   myGLCD.setColor(VGA_WHITE);
     myGLCD.clrScr();
     getDate();
 drawDisplay(); 
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_LOW);
  
  myButtons.setTextFont(BigFont);

 

  // Setup Serial connection
    Serial.begin(115200);	// Debugging only
  
    if (!driver.init())
         Serial.println("init failed");
    Serial.println("Device ready");     


  // The following lines can be commented out to use the values already stored in the DS1307
 // rtc.setDOW(SUNDAY);        // Set Day-of-Week to SUNDAY
 // rtc.setTime(20, 55, 0);     // Set the time to 12:00:00 (24hr format)
 // rtc.setDate(9, 6, 2014);   // Set the date to October 3th, 2010
}

void loop()
{
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t buflen = sizeof(buf);

    blankCtr++;
    if (blankCtr > 2000) {
        blankCtr = 0;
        myGLCD.setColor(VGA_BLACK);
        myGLCD.fillRect(210, 217, 319, 239);
        myGLCD.setColor(VGA_WHITE);
        }

    if (driver.recv(buf, &buflen)){ // Non-blocking
	// Message with a good checksum received, dump it.
        blankCtr = 0;
        if (blinkSw == false){
            blinkSw = true;
            myGLCD.setColor(VGA_RED);
            for (int i=0; i<5; i++){
                 myGLCD.drawCircle(215, 225, i);
                 }
            }  else {
                     blinkSw = false;
                     myGLCD.setColor(VGA_BLACK);
                     for (int i=0; i<5; i++){
                         myGLCD.drawCircle(215, 225, i);
                         }
                     }
        driver.printBuffer("Got:", buf, buflen);
        myGLCD.setColor(VGA_WHITE);
        char recv[5] = "    ";
        if (buf[0] == 'P') {
            if (buf[1] == 'C') {
                Serial.print("Probe tempC: "); 
                for (int i = 2; i < buflen; i++) {
                     recv[i - 2] = buf[i];
                     }
                String y = "P ";
                y.concat(recv);
                y.concat(" C  ");
                myGLCD.print(y, 240, 5);
                String myDate = rtc.getDateStr();
                Serial.println(myDate);  
//                myGLCD.setBackColor(VGA_BLACK);
                myGLCD.drawCircle(290, 5, 2);
                }
            if (buf[1] == 'F') {
              Serial.print("Probe tempF: "); 
                for (int i = 2; i < buflen; i++) {
                     recv[i - 2] = buf[i];
                     }
                String y = "";
                y.concat(recv);
                y.concat(" F");
                myGLCD.print(y, 230,  220);
                String myDate = rtc.getDateStr();
                Serial.println(myDate);  
//                myGLCD.setBackColor(VGA_BLACK);
                myGLCD.drawCircle(265, 220, 2);
                }
            }
        if (buf[0] == 'D') {
            if (buf[1] == 'C') {
                Serial.print("DHT22 tempC: "); 
                for (int i = 2; i < buflen; i++) {
                     recv[i - 2] = buf[i];
                     }
                String y = "D ";
                y.concat(recv);
                y.concat(" C  ");
                myGLCD.print(y, 240, 20);
                String myDate = rtc.getDateStr();
                Serial.println(myDate);  
//                myGLCD.setBackColor(VGA_BLACK);
                myGLCD.drawCircle(290, 20, 2);
                }
            if (buf[1] == 'F') {
                Serial.print("DHT22 tempF: "); 
                for (int i = 2; i < buflen; i++) {
                     recv[i - 2] = buf[i];
                     }
                String y = "";
                y.concat(recv);
                y.concat(" F  ");
                myGLCD.print(y, 257, 35);
                String myDate = rtc.getDateStr();
                Serial.println(myDate);  
//                myGLCD.setBackColor(VGA_BLACK);
                myGLCD.drawCircle(290, 35, 2);
                }
            if (buf[1] == 'H') {
                Serial.print("Probe humidity: "); 
                for (int i = 2; i < buflen; i++) {
                     recv[i - 2] = buf[i];
                     }
                String y = "";
                y.concat(recv);
                y.concat("%H  ");
                myGLCD.print(y, 257, 50);
                String myDate = rtc.getDateStr();
                Serial.println(myDate);  
                }
            }
       Serial.println(recv);
       Serial.print(myHour); Serial.print(":"); Serial.print(myMinute); Serial.print(":"); Serial.println(mySecond);        
       Serial.println();
       }



  getTouch();
  while (ty != 0) {
         if (ty > 55 && ty < 100) {
             drawDisplay();  
           if (reVerse == false) {
                 reVerse = true;
             } else {
                reVerse = false;
             }
             if (menuCtr == 2) {
                 myGLCD.setColor(VGA_WHITE);               
                 myGLCD.print("Min", 250,  30);
                 but1 = myButtons.addButton( 250,  60, 50,  40, "Adv");
                 but2 = myButtons.addButton( 250,  140, 50,  40, "Rev");
                 myButtons.drawButtons();
                 }
 Serial.print("menuCtr: "); Serial.println(menuCtr);
             if (menuCtr == 1) {
                 myGLCD.setColor(VGA_WHITE);
                 myGLCD.print("Hr", 260,  30);
                 but1 = myButtons.addButton( 250,  60, 50,  40, "Adv");
                 but2 = myButtons.addButton( 250,  140, 50,  40, "Rev");
                 myButtons.drawButtons();
                 }

         }
         if (ty > 110 && ty < 199) {
             if (menuCtr == 1) {
                 bumpHr();
             }
             if (menuCtr == 2) {
                 bumpMin();
             }
         }
         if (ty > 0 && ty < 50 || ty > 200) {
             ty = 0;
             menuCtr++;
             reVerse = false;
             if (menuCtr > 2) {
                 menuCtr = 0;
                 digitalDisplay = false;
                 }
 Serial.print("menuCtr: "); Serial.println(menuCtr);
             if (menuCtr == 1) {
                 myGLCD.setColor(VGA_WHITE);
                 myGLCD.print("Hr", 260,  30);
                 but1 = myButtons.addButton( 250,  60, 50,  40, "Adv");
                 but2 = myButtons.addButton( 250,  130, 50,  40, "Rev");
                 myButtons.drawButtons();
                 digitalDisplay = true;
                 }
     
              if (menuCtr == 2) {
                  myGLCD.setColor(VGA_WHITE);
                  myGLCD.print("Min", 250,  30);
                  digitalDisplay = true;                  
                  }
      
              if (menuCtr == 0){
                  myButtons.deleteButton(but1);
                  myButtons.deleteButton(but2);            
                  drawDisplay();           
                }
              }
              break;
          } 
  getDate();
  if (oldsec != mySecond) {
      oldsec = mySecond;
      drawMin(myMinute);
      drawHour(myHour, myMinute);
      drawSec(mySecond);
      myGLCD.setFont(SmallFont); 
      myGLCD.setColor(VGA_WHITE);
      String x = "";
      int wrkHr = myHour;
      if (myHour == 0) {
          wrkHr = 12;
          }
      if (myHour > 12) {
          wrkHr = myHour - 12; 
          }
      if (wrkHr < 10) {
          x.concat("0");
          }
      x.concat(wrkHr);
      x.concat(":");
      if (myMinute < 10) {
          x.concat("0");
          }
      x.concat(myMinute);
      x.concat(":");
      if (mySecond < 10) {
          x.concat("0");
          }
      x.concat(mySecond);
      x.concat("  ");
      myGLCD.print(x, 230,  200);
      String myDate = rtc.getDateStr();
      int searchIndex = 0;
      int searchIndex2 = 0;
      searchIndex = myDate.indexOf('.');
      searchIndex2 = myDate.indexOf('.', searchIndex + 1);
      String myMonthday = myDate.substring(0, searchIndex);
 //     Serial.println(myMonthday);
      String myMonth = myDate.substring(searchIndex+1, searchIndex2);
 //     Serial.println(myMonth);
      String myYear = myDate.substring(searchIndex2 +1, myDate.length());
 //     Serial.println(myYear);
      String d = "";
      d.concat(myMonth);
      d.concat("/");
      d.concat(myMonthday);
      d.concat("/");
      d.concat(myYear);
      myGLCD.print(d, 230, 180);
      
   //   myGLCD.setFont(BigFont);
  }
}

void getDate(){
  // Send time
  //Serial.println(rtc.getTimeStr());
  String timeString = rtc.getTimeStr();
  int searchIndex = 0;
  int foundIndex = 0;
  String foundValue = "";
  foundIndex = timeString.indexOf(':');
  foundValue = timeString.substring(searchIndex, foundIndex);
  char temphour[foundValue.length() + 1];  
  foundValue.toCharArray(temphour, sizeof(temphour));
  myHour = atoi(temphour);
  searchIndex = foundIndex + 1;
  foundIndex = timeString.lastIndexOf(':');
  foundValue = timeString.substring(searchIndex, foundIndex);
  char tempminute[foundValue.length() + 1];  
  foundValue.toCharArray(tempminute, sizeof(tempminute));
  myMinute = atoi(tempminute);
  searchIndex = foundIndex +1;
  foundIndex = timeString.length();
  foundValue = timeString.substring(searchIndex, foundIndex+1);
  char tempsecond[foundValue.length() + 1];  
  foundValue.toCharArray(tempsecond, sizeof(tempsecond));
  mySecond = atoi(tempsecond);
//  Serial.print(myHour); Serial.print(":"); Serial.print(myMinute); Serial.print(":"); Serial.println(mySecond);
  // Wait one second before repeating :)

}
void drawDisplay()
{
  // Clear screen
  myGLCD.clrScr();
  
  // Draw Clockface
  myGLCD.setColor(0, 0, 255);
  myGLCD.setBackColor(0, 0, 0);
  for (int i=0; i<5; i++)
  {
    myGLCD.drawCircle(clockCenterX, clockCenterY, 119-i);
  }
  for (int i=0; i<5; i++)
  {
    myGLCD.drawCircle(clockCenterX, clockCenterY, i);
  }
  
  myGLCD.setColor(192, 192, 255);
  myGLCD.print("3", clockCenterX+92, clockCenterY-8);
  myGLCD.print("6", clockCenterX-8, clockCenterY+95);
  myGLCD.print("9", clockCenterX-109, clockCenterY-8);
  myGLCD.print("12", clockCenterX-16, clockCenterY-109);
  for (int i=0; i<12; i++)
  {
    if ((i % 3)!=0)
      drawMark(i);
  }  
 getDate();
  drawMin(myMinute);
  drawHour(myHour, myMinute);
  drawSec(mySecond);
  oldsec=mySecond;

}
void drawMark(int h)
{
  float x1, y1, x2, y2;
  
  h=h*30;
  h=h+270;
  
  x1=110*cos(h*0.0175);
  y1=110*sin(h*0.0175);
  x2=100*cos(h*0.0175);
  y2=100*sin(h*0.0175);
  
  myGLCD.drawLine(x1+clockCenterX, y1+clockCenterY, x2+clockCenterX, y2+clockCenterY);
}


void drawSec(int s)
{
  float x1, y1, x2, y2;
  int ps = s-1;
  
  myGLCD.setColor(0, 0, 0);
  if (ps==-1)
    ps=59;
  ps=ps*6;
  ps=ps+270;
  
  x1=95*cos(ps*0.0175);
  y1=95*sin(ps*0.0175);
  x2=80*cos(ps*0.0175);
  y2=80*sin(ps*0.0175);
  
  myGLCD.drawLine(x1+clockCenterX, y1+clockCenterY, x2+clockCenterX, y2+clockCenterY);

  myGLCD.setColor(255, 0, 0);
  s=s*6;
  s=s+270;
  
  x1=95*cos(s*0.0175);
  y1=95*sin(s*0.0175);
  x2=80*cos(s*0.0175);
  y2=80*sin(s*0.0175);
  
  myGLCD.drawLine(x1+clockCenterX, y1+clockCenterY, x2+clockCenterX, y2+clockCenterY);
}

void drawMin(int m)
{
  float x1, y1, x2, y2, x3, y3, x4, y4;
  int pm = m-1;
  
  myGLCD.setColor(0, 0, 0);
  if (pm==-1)
    pm=59;
  pm=pm*6;
  pm=pm+270;
  
  x1=80*cos(pm*0.0175);
  y1=80*sin(pm*0.0175);
  x2=5*cos(pm*0.0175);
  y2=5*sin(pm*0.0175);
  x3=30*cos((pm+4)*0.0175);
  y3=30*sin((pm+4)*0.0175);
  x4=30*cos((pm-4)*0.0175);
  y4=30*sin((pm-4)*0.0175);
  
  myGLCD.drawLine(x1+clockCenterX, y1+clockCenterY, x3+clockCenterX, y3+clockCenterY);
  myGLCD.drawLine(x3+clockCenterX, y3+clockCenterY, x2+clockCenterX, y2+clockCenterY);
  myGLCD.drawLine(x2+clockCenterX, y2+clockCenterY, x4+clockCenterX, y4+clockCenterY);
  myGLCD.drawLine(x4+clockCenterX, y4+clockCenterY, x1+clockCenterX, y1+clockCenterY);

  myGLCD.setColor(0, 255, 0);
  m=m*6;
  m=m+270;
  
  x1=80*cos(m*0.0175);
  y1=80*sin(m*0.0175);
  x2=5*cos(m*0.0175);
  y2=5*sin(m*0.0175);
  x3=30*cos((m+4)*0.0175);
  y3=30*sin((m+4)*0.0175);
  x4=30*cos((m-4)*0.0175);
  y4=30*sin((m-4)*0.0175);
  
  myGLCD.drawLine(x1+clockCenterX, y1+clockCenterY, x3+clockCenterX, y3+clockCenterY);
  myGLCD.drawLine(x3+clockCenterX, y3+clockCenterY, x2+clockCenterX, y2+clockCenterY);
  myGLCD.drawLine(x2+clockCenterX, y2+clockCenterY, x4+clockCenterX, y4+clockCenterY);
  myGLCD.drawLine(x4+clockCenterX, y4+clockCenterY, x1+clockCenterX, y1+clockCenterY);
}

void drawHour(int h, int m)
{
  float x1, y1, x2, y2, x3, y3, x4, y4;
  int ph = h;
  
  myGLCD.setColor(0, 0, 0);
  if (m==0)
  {
    ph=((ph-1)*30)+((m+59)/2);
  }
  else
  {
    ph=(ph*30)+((m-1)/2);
  }
  ph=ph+270;
  
  x1=60*cos(ph*0.0175);
  y1=60*sin(ph*0.0175);
  x2=5*cos(ph*0.0175);
  y2=5*sin(ph*0.0175);
  x3=20*cos((ph+5)*0.0175);
  y3=20*sin((ph+5)*0.0175);
  x4=20*cos((ph-5)*0.0175);
  y4=20*sin((ph-5)*0.0175);
  
  myGLCD.drawLine(x1+clockCenterX, y1+clockCenterY, x3+clockCenterX, y3+clockCenterY);
  myGLCD.drawLine(x3+clockCenterX, y3+clockCenterY, x2+clockCenterX, y2+clockCenterY);
  myGLCD.drawLine(x2+clockCenterX, y2+clockCenterY, x4+clockCenterX, y4+clockCenterY);
  myGLCD.drawLine(x4+clockCenterX, y4+clockCenterY, x1+clockCenterX, y1+clockCenterY);

  myGLCD.setColor(255, 255, 0);
  h=(h*30)+(m/2);
  h=h+270;
  
  x1=60*cos(h*0.0175);
  y1=60*sin(h*0.0175);
  x2=5*cos(h*0.0175);
  y2=5*sin(h*0.0175);
  x3=20*cos((h+5)*0.0175);
  y3=20*sin((h+5)*0.0175);
  x4=20*cos((h-5)*0.0175);
  y4=20*sin((h-5)*0.0175);
  
  myGLCD.drawLine(x1+clockCenterX, y1+clockCenterY, x3+clockCenterX, y3+clockCenterY);
  myGLCD.drawLine(x3+clockCenterX, y3+clockCenterY, x2+clockCenterX, y2+clockCenterY);
  myGLCD.drawLine(x2+clockCenterX, y2+clockCenterY, x4+clockCenterX, y4+clockCenterY);
  myGLCD.drawLine(x4+clockCenterX, y4+clockCenterY, x1+clockCenterX, y1+clockCenterY);
}
/**********************************************
            get_touch
***********************************************/

void getTouch() 
{

  // clear the coordinates
  tx = 0;
  ty = 0;
  while (myTouch.dataAvailable()) {
         myTouch.read();
         tx = myTouch.getX();
         ty = myTouch.getY();
         delay(50);
  Serial.print("tx: "); Serial.print(tx); Serial.print(" ty: "); Serial.println(ty);
  }

}
void bumpHr() {
  if (reVerse == false) {
      myHour++;
  } else {
      myHour--;
  }
  if (myHour > 23 ){
     myHour = 0;
  }
  if (myHour < 0 ){
     myHour = 23;
  }
  rtc.setTime(myHour, myMinute, mySecond);
  myGLCD.setColor(VGA_BLACK);
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.fillRect(0, 0, 240, 320);
  drawDisplay();
  getDate();
  myGLCD.setColor(VGA_WHITE);
  myGLCD.print("Hr", 260,  30);
  myButtons.drawButtons();

}

void bumpMin() {
  if (reVerse == false) {
      myMinute++;
  } else {
    myMinute--;
  }
  if (myMinute > 59){
      myMinute = 0;
  }
  if (myMinute < 0){
      myMinute = 59;
  }

  mySecond = 0;
  rtc.setTime(myHour, myMinute, mySecond); 
  drawDisplay();
  getDate();
  myGLCD.setColor(VGA_WHITE);
  myGLCD.print("Min", 250,  30);
  myButtons.drawButtons();
}

