//Author:Mohammed Saleeq K
//20:05:2022
#include<Servo.h>
#include<LiquidCrystal.h>
//Function initiations
void cls_read(void);
void AutoCalibrate(void);
void Run(void);
int find_min(int a,int b,int c){
if(a<b){if(a<c){return 0;}else{return 2;}}else{if(b<c){return 1;}else{return 2;}}
}

//lowering rate
int da =1;
//Pickup/drop strip
const int home=90;
const int pickup_strip=120;
const int red_zone =180;
const int green_zone =130;
const int blue_zone=40;
const int drop_strip[3]={red_zone,green_zone,blue_zone};
String inputString="";
bool stringComplete=false;
Servo baseServo;
Servo armServo;
//Potentiometer initiation
const int potPin = A1;
volatile int val;
//proximity
int proxy_sense = A0;
//Control buttons
const int button1=9,button2=8,button3=7;
//LCD connections
const int rs = 2, en = 3, d4 = 4, d5 = 8, d6 = 12, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
String lcdBuffer="";
int MenuDelay=1000;

//EM connection
const int EM = 7;

//color sensor pins
const int cls_r=9,cls_g=10,cls_b=11;
int cls_ldr = A2;
//Caliberation patch
const int ref_white = 178;
const int ref_black = 138;
const int ref_blue = 109;
const int ref_green = 75;
const int ref_red = 46;
int cls_Inty[3]={255,255,255};
int err_lim=10;
int dInty=3;
int cls_reflect_r,cls_reflect_g,cls_reflect_b=0;
String primary_colors[3] ={"Red","Green","Blue"};
//Servo presets
int armAngle=90;

//Setup
void setup() {
//control buttons initiation
pinMode(11,OUTPUT);
pinMode(7,OUTPUT);
//pinMode(button3,INPUT);
//lcd initiation
lcd.begin(16,2);
lcd.setCursor(0,0); 
lcd.print("Hello");
//Serial initiation
Serial.begin(9600);
//servo initiation
baseServo.attach(5);
armServo.attach(6);
//SerialEvent
inputString.reserve(200);
//color sensing initiation
pinMode(cls_r,OUTPUT);
pinMode(cls_g,OUTPUT);
pinMode(cls_b,OUTPUT);
lcd.setCursor(0,0);
lcd.print("Starting...");
//prompt user
 Serial.println("Select Mode:");
 Serial.println("1.Calibrate 2.Run");
}

//Mainloop

void loop() { 
  
//if input given proceed:
 if(stringComplete){
  
 //Serial.println(inputString[0]);
 if(inputString[0]=='1'){
  //caliberate
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Autocalibrating");
  //reset intensity
  cls_reflect_r=255;
  cls_reflect_b=255;
  cls_reflect_g=255;

  AutoCalibrate();
  }else if(inputString[0]=='2'){
    //Run
    Run();
    }else{
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Invalid input");
      Serial.println("Invalid input");
      delay(2*MenuDelay);
      
      }
 Serial.println("Select Mode:");
 Serial.println("1.Calibrate 2.Run");
 inputString="";
 stringComplete=false;
 
 }else{ 
    //else update lcd and check for manual input
    lcd.setCursor(0,0);
    lcd.print("Select Mode:");
    lcd.setCursor(0,1);
    lcd.print("1.Calibrate");
    delay(MenuDelay); 
   lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Select Mode:");
    lcd.setCursor(0,1);
    lcd.print("2.Run");
    delay(MenuDelay);

  }
  }

 
bool balance_check(int a,int b , int c){
  bool flag=false;
if(abs(a-b)<err_lim){
  if(abs(b-c)<err_lim){
    if(abs(c-a)<err_lim){
      flag=true;
      }
    }
}
return flag;
}
void AutoCalibrate(){
//Calibration
  baseServo.write(ref_white);
  delay(500);
  cls_read();

  while(!balance_check(cls_reflect_r,cls_reflect_g,cls_reflect_b)){
    cls_Inty[find_min(cls_reflect_r,cls_reflect_g,cls_reflect_b)]-=dInty;
    Serial.print("To reduce: ");
    Serial.println(primary_colors[find_min(cls_reflect_r,cls_reflect_g,cls_reflect_b)]);
    cls_read();
    
  }

//Test sensing
  //position over white
  Serial.println("On white");
  baseServo.write(ref_white);
  delay(500);
  cls_read();
  Serial.println("White");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Color:");
  lcd.setCursor(0,1);
  lcd.print("White");

  //position over black
  Serial.println("On black");
  baseServo.write(ref_black);
  delay(500);
  cls_read();
  Serial.println("Black");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Color:");
  lcd.setCursor(0,1);
  lcd.print("Black");
   //position over blue
  Serial.println("On Blue");
  baseServo.write(ref_blue);
  delay(500);
  cls_read();
  Serial.println(primary_colors[find_min(cls_reflect_r,cls_reflect_g,cls_reflect_b)]);
  lcd.setCursor(0,0);
  lcd.print("Color:");
  lcd.setCursor(0,1);
  lcd.print(primary_colors[find_min(cls_reflect_r,cls_reflect_g,cls_reflect_b)]);
  //position over green
  Serial.println("On Green");
  baseServo.write(ref_green);
  delay(500);
  cls_read();
  Serial.println(primary_colors[find_min(cls_reflect_r,cls_reflect_g,cls_reflect_b)]);
  lcd.setCursor(0,0);
  lcd.print("Color:");
  lcd.setCursor(0,1);
  lcd.print(primary_colors[find_min(cls_reflect_r,cls_reflect_g,cls_reflect_b)]);
    //position over red
  Serial.println("On Red");
  baseServo.write(ref_red);
  delay(500);
  cls_read();
  Serial.println(primary_colors[find_min(cls_reflect_r,cls_reflect_g,cls_reflect_b)]);
  lcd.setCursor(0,0);
  lcd.print("Color:");
  lcd.setCursor(0,1);
  lcd.print(primary_colors[find_min(cls_reflect_r,cls_reflect_g,cls_reflect_b)]);
 
 
}

void cls_read(void){
  analogWrite(cls_r,cls_Inty[0]);
  digitalWrite(cls_g,0);
  digitalWrite(cls_b,0);
  delay(100);
  Serial.print("R:");
  cls_reflect_r=analogRead(cls_ldr);
  Serial.print(cls_reflect_r);
  delay(30);
  digitalWrite(cls_r,0);
  analogWrite(cls_g,cls_Inty[1]);
  digitalWrite(cls_b,0);
  delay(100);
  Serial.print(" G:");
  cls_reflect_g=analogRead(cls_ldr);
  Serial.print(cls_reflect_g);
  delay(30);
  digitalWrite(cls_r,0);
  digitalWrite(cls_g,0);
  analogWrite(cls_b,cls_Inty[2]);
  delay(100);
  Serial.print(" B:");
  cls_reflect_b=analogRead(cls_ldr);
  Serial.print(cls_reflect_b);

  Serial.print(" Intensity: ");
  Serial.print(cls_Inty[0]);
  Serial.print(" ");
  Serial.print(cls_Inty[1]);
  Serial.print(" ");
  Serial.println(cls_Inty[2]);
 
  delay(30);  
}
void Run(void){
  //update lcd
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Running...");
  Serial.println("Running...");
  delay(700);
  //set sensor to position
  //update lcd
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Positioning");
  Serial.println("Postioning sensor");
  lcd.setCursor(0,1);
  lcd.print("Sensor");
  baseServo.write(pickup_strip);
  delay(500);
  //sense color
  cls_read();
  delay(100);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Color detected:");
  lcd.setCursor(0,1);
  int clr_idx=find_min(cls_reflect_r,cls_reflect_g,cls_reflect_b);
  lcd.print(primary_colors[clr_idx]);
  Serial.println(primary_colors[clr_idx]);
  delay(600);
  //pickup
  //update lcd
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Picking up");
  Serial.println("Picking up");
  baseServo.write(180-pickup_strip);
  delay(1000);
  int diff_angle=0;
  while(digitalRead(proxy_sense)){
      diff_angle+=da;
      if(diff_angle>50){diff_angle=50;
      break;
      }
      armServo.write(armAngle- diff_angle);
      delay(20); 
      

  }
  lcd.clear();
  Serial.println("Object found");
  lcd.setCursor(0,0);
  lcd.print("object found");
  delay(1600);
  digitalWrite(EM,HIGH);
  digitalWrite(11,1);
  lcd.clear();
  Serial.println("Magnet activated");
  lcd.setCursor(0,0);
  lcd.print("Magnet");
  lcd.setCursor(0,1);
  lcd.print("Activated");
  delay(1000);
  armServo.write(armAngle);
  delay(1600);
    lcd.clear();
  Serial.println("Dropping...");
  lcd.setCursor(0,0);
  lcd.print("Dropping...");
  baseServo.write(180-drop_strip[clr_idx]);
  delay(1000);
  armServo.write(armAngle-diff_angle);
  delay(1600);
  digitalWrite(EM,LOW);
  digitalWrite(11,LOW);
    lcd.clear();
  Serial.println("Magnet deactiavted");
  lcd.setCursor(0,0);
  lcd.print("Magnet");
  lcd.setCursor(0,1);
  lcd.print("Deactivated");
  delay(1000);
  armServo.write(armAngle);
  delay(1600);
    lcd.clear();
  Serial.println("Homing...");
  lcd.setCursor(0,0);
  lcd.print("Homing...");
  baseServo.write(home);
  armServo.write(home);
  delay(500);
}
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
