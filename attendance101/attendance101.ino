#include <SPI.h>            //Communicate with SPI devices
#include <SoftwareSerial.h> //Communicate with SPI devices

#include <EEPROM.h>     //SD Card
#include <SD.h>         //SD Card


#include <Adafruit_Fingerprint.h>

#include <Adafruit_GFX.h>     //Graphics LCD
#include <LiquidCrystal.h>    //Graphics LCD

#include "RTClib.h"       //Clock

LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

//SoftwareSerial mySerial(A12, A13);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);

File myFile;

RTC_DS1307 rtc;
int min_old = 0;

struct UserData
{
  int Enter_Hour;
  int Enter_Min;
  String username;
  String WT;
};
UserData userTable[128];
int present[128] = {0};

DateTime now;

const int BUT = 31;

int up_data = 1;
int upload_data = 1;
int query = 0;

int Finger_ID = 0;



// void LCD_Print(int x, int y, int col, int sz, String msg)
// {
//   Serial.print(msg); 
// }

// void LCD_Setup_Menu()
// {
//   Serial.begin(9600);
//   //lcd.begin(16,2);
//   Serial.print("Attendance\n"); 
//   // for (int j = 0; j < 20; j++) {
    
//   //   Serial.print("Attendance"); 
//   //   delay(250); 
//   // }
// }

// void LCD_Wich_User(int wid)
// {
//   switch (wid)
//   {
//     case 1:
//       LCD_Print(0, 0, 0, 1,  Name_ID1);
//       break;
//     case 2:
//       LCD_Print(0, 0, 0, 1,  Name_ID2);
//       break;
//     case 3:
//       LCD_Print(0, 0, 0, 1,  Name_ID3);
//       break;
//     case 4:
//       LCD_Print(0, 0, 0, 1,  Name_ID4);
//       break;
//     case 5:
//       LCD_Print(0, 0, 0, 1,  Name_ID5);
//       break;
//     case 6:
//       LCD_Print(0, 0, 0, 1,  Name_ID6);
//       break;
//     case 7:
//       LCD_Print(0, 0, 0, 1,  Name_ID7);
//       break;
//     case 8:
//       LCD_Print(0, 0, 0, 1,  Name_ID8);
//       break;
//     case 9:
//       LCD_Print(0, 0, 0, 1,  Name_ID9);
//       break;
//     case 10:
//       LCD_Print(0, 0, 0, 1,  Name_ID10);
//       break;
//     case 11:
//       LCD_Print(0, 0, 0, 1,  Name_ID11);
//       break;
//     case 12:
//       LCD_Print(0, 0, 0, 1,  Name_ID12);
//       break;
//     case 13:
//       LCD_Print(0, 0, 0, 1,  Name_ID13);
//       break;
//     case 14:
//       LCD_Print(0, 0, 0, 1,  Name_ID14);
//       break;
//     case 15:
//       LCD_Print(0, 0, 0, 1,  Name_ID15);
//       break;
//   }
// }

// void LCD_Enter_User(int f_ID)
// {
//   now = rtc.now();
//   String En = String(now.hour()) + ":" + String(now.minute());
  
  
  
//   LCD_Wich_User(f_ID);
// }

// void LCD_Exit_User(int f_ID)
// {
//   now = rtc.now();
//   String En = String(now.hour()) + ":" + String(now.minute());
  
  
//   LCD_Wich_User(f_ID); 
  
  
//   Serial.print(" GOODBYE\n");
  
//   Serial.print(En);
  
//   Serial.print(WT);
// }

// void showmsgXY(int x, int y, int sz, const char *msg)
// {

//   Serial.print(msg); 
  
//   delay(1000); 
// }

// void LCD_Main_Menu()
// {
//   int p = 0;
//   String watch = "";
//   String today = "";

//   now = rtc.now();
//   for (int i = 0; i < 16; i++)
//   {
//     if (present[i] == 1) p++;
//   }

//   watch = String(now.hour()) + ":" + String(now.minute());
//   today = String(now.year()) + "/" + String(now.month()) + "/" + String(now.day());
  
//   Serial.print("\nTime: ");
//   Serial.print(watch);
//   Serial.print(" \nDate: ");
//   Serial.print(today);
//   Serial.print(" \nPresent: ");
//   Serial.print(p);
// }


// void LCD_Finger_Error()
// { 
//   Serial.print(" Image is too messy \n");
//   delay(1500); 
//   LCD_Main_Menu(); 
// }

int getFingerprintIDez()
{
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return 255;

  Serial.print("\nFound ID #"); 
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); 
  Serial.println(finger.confidence);
  return finger.fingerID;
} 

void Enroll()
{
  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the ID # (from 1 to 255) you want to save this finger as...");
  uint8_t id = readnumber();
  if (id == 0)
  {
    return;
  }
  Serial.print("Enrolling ID #");
  Serial.println(id);
  while (!getFingerprintEnroll(id) );
}

int getFingerprintEnroll(uint8_t id)
{
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }
  // OK success!
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID ");
  Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }
  // OK success!
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK, got 2 image of finger and converted!
  Serial.print("Creating model for #");
  Serial.println(id);
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
  Serial.print("ID ");
  Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) 
  {
    Serial.println("Stored!");
    Serial.println("Type your user name: ");
    Get_User_Name(id);
  } 
  else if (p == FINGERPRINT_PACKETRECIEVEERR) 
  {
    Serial.println("Communication error");
    return p;
  } 
  else if (p == FINGERPRINT_BADLOCATION) 
  {
    Serial.println("Could not store in that location");
    return p;
  }
  else if (p == FINGERPRINT_FLASHERR) 
  {
    Serial.println("Error writing to flash");
    return p;
  }
  else 
  {
    Serial.println("Unknown error");
    return p;
  }
  Serial.println("Successful Enroll");
}

int readnumber(void)
{
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void Get_User_Name(int finger_id)
{
  int myTimeout = 5000;
  Serial.setTimeout(myTimeout);
  while (!Serial.available());
  userTable[finger_id].username = Serial.readString();
}


void Enter_User(int uid)
{
  present[uid] = 1;
  Serial.print("Entering user:");
  Serial.println(userTable[uid].username);
  //LCD_Enter_User(uid);
  DateTime Time_Enter = rtc.now();
  userTable[uid].Enter_Hour = Time_Enter.hour();
  userTable[uid].Enter_Min = Time_Enter.minute();
  EEPROM.put(100+uid*10, userTable[uid].Enter_Hour);
  EEPROM.put(105+uid*10, userTable[uid].Enter_Min);
  delay(4000);
  //LCD_Main_Menu();
}

void Exit_User(int uid)
{
  int work_time = 0, wh = 0, wm = 0;
  present[uid] = 0;
  Serial.print("Exitting user:");
  Serial.println(userTable[uid].username);
  DateTime Time_Exit = rtc.now();

  work_time = (Time_Exit.hour() * 60 + Time_Exit.minute()) - ((userTable[uid].Enter_Hour * 60) + userTable[uid].Enter_Min);
  wh = work_time / 60;
  wm = work_time % 60;
  userTable[uid].WT = String(wh) + ":" + String(wm);

  String Final = Make_String(uid, Time_Exit);
  Serial.println(Final);

  Writ_to_Main_File(Final, uid);

  query++;
  if (query > 255) query = 1;

  //Write_to_Upload_File(query, Final);
  //LCD_Exit_User(uid);

  EEPROM.put(100+uid*10, 0);
  EEPROM.put(105+uid*10, 0);
  delay(3000);

  //LCD_Main_Menu();
}

String Make_String(int ID, DateTime CurrentDate)
{
  String Final =  String(CurrentDate.day()) + "/" + String(CurrentDate.month()) + "/" + String(CurrentDate.year());
  Final += ",";
  Final += userTable[ID].username;
  Final += ",";
  Final += String(userTable[ID].Enter_Hour) + ":" + String(userTable[ID].Enter_Min);
  Final += ",";
  Final += String(CurrentDate.hour()) + ":" + String(CurrentDate.minute());
  Final += ",";
  Final += userTable[ID].WT;
  return Final;
}

void Writ_to_Main_File(String str, int fid)
{
  String initials = "atten";
  String file_name = initials + ".csv";
  myFile = SD.open(file_name, FILE_WRITE);

  String a = "";
  a = myFile.readString();
  String filecontent = "";
  filecontent += a + str;
  
  if (myFile)
  {
    myFile.println(filecontent);
    myFile.close();
  }
  else
  {
    Serial.println("error opening atten.csv");
    // beep(1500);
  }

}

void loop() {
  int flag = 0;
  int upload_queue = 1;
  String upload_str = "";
  String n_watch = "";

  now = rtc.now();
  
  Finger_ID = getFingerprintIDez();

  if (Finger_ID < 127 && Finger_ID > 0) {
    if (present[Finger_ID] == 0) {
      Enter_User(Finger_ID);
    }
    else if (present[Finger_ID] == 1) {
      delay(150);
      Exit_User(Finger_ID);
    }
  }
  else if (Finger_ID == 255) {
    //LCD_Finger_Error();
  }
  
  // if (now.hour() > 20 || now.hour() < 6) {
  //   for (upload_queue = 1; upload_queue < 256; upload_queue++) {
  //     upload_str = String(upload_queue) + ".txt";
  //     if (SD.exists(upload_str)) {
  //       //LCD_Upload_Menu();
  //       Uploading();
  //       LCD_Main_Menu();
  //     }
  //   }
  // }
  
  if (digitalRead(BUT) == 0) {
    flag = 1;
    if (flag == 1) {
      Enroll();
      flag = 0;
    }
  }
}

void setup() {
  Serial.begin(9600);
  finger.begin(57600);
  pinMode(BUT, INPUT_PULLUP);
  rtc.begin();
  Get_from_EEPROM();
  Check_present();
  
  if (!SD.begin(53)) {
    Serial.println("Initialization failed!");
    
    while (1) {
      delay(200); 
    }
  }
  else {
    Serial.println("SD OK");
  }
 
  Serial.print("Initializing...");
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  }
  else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }
  
  finger.getTemplateCount();
  // LCD_Setup_Menu();
  // LCD_Main_Menu();
  
  now = rtc.now();
  min_old = now.minute();
}

void Get_from_EEPROM()
{
  for(int i = 0; i < 126; i++)
  {
    EEPROM.get(100+i*10, userTable[i].Enter_Hour);
    EEPROM.get(105+i*10, userTable[i].Enter_Min);
  }
  Serial.println("EEPROM Update");
}

void Check_present()
{
  for(int i = 0; i < 127; i++)
    if (userTable[i].Enter_Hour > 0) present[i] = 1;
  Serial.println("present Update");
}
