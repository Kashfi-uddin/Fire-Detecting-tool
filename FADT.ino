#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27,16,2);   
#define DHTPIN 11   
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);
int sol = A3;
int ledPin = 7;//LED PIN.
int greenled = 6;
int buzzerPin = 10;                          //BUZZER INPUT PIN.
int pirPin = 9;                            //MOTION SENSOR INPUT PIN.
int sensorValue = LOW;
int flamesensvalue=0;
int flamepin=8;
int h; 
int t;

static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
// The TinyGPS++ object
TinyGPSPlus gps;
int temp=0,i;
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);
String stringVal = "";


void setup() { 
     Serial.begin(9600);
     ss.begin(GPSBaud);
     gsm_init();
     Serial.println("AT+CNMI=2,2,0,0,0");
  
    Serial.begin(9600);
    Serial.println("Temperature and Humidity Sensor Test");
    dht.begin();
    lcd.init(); 
    lcd.backlight();
  pinMode(sol, OUTPUT);                              
  pinMode(buzzerPin, OUTPUT);               //SET BUZZER AS OUTPUT.
  pinMode(ledPin, OUTPUT);                  //SET LED AS OUTPUT.
  pinMode(pirPin, INPUT); 
  pinMode(flamepin,INPUT);
  pinMode(greenled,OUTPUT);
}
void loop() {  
    //serialEvent();
 
      while(temp)
      {
        while (ss.available() > 0)
        {
          gps.encode(ss.read());
                if (gps.location.isUpdated())
                {
                 temp=0;
                 digitalWrite(13,HIGH);
                 tracking();
                } 
          if(!temp)
          break;  
        }
      }
      digitalWrite(13,LOW);
  
    h = dht.readHumidity();
    t = dht.readTemperature();
    
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %, Temp: ");
    Serial.print(t);
    Serial.println(" ° Celsius");
        

    lcd.setCursor(0, 0);
    lcd.print("Temp:");
    lcd.print(t);
    lcd.print("C");
    lcd.setCursor(9, 0);
    lcd.print("Hum:");
    lcd.print(h);
    lcd.print("%");
    
  delay(1000);  
  digitalWrite(greenled, HIGH);
  sensorValue = digitalRead(pirPin);
  flamesensvalue=digitalRead(flamepin);
  while( flamesensvalue== HIGH) {               //IF MOTION IS DETECTED.
    digitalWrite(greenled, LOW);
    digitalWrite(sol, HIGH);
    tone(buzzerPin, 1200);                  //BUZZ THE BUZZER. 
    digitalWrite(ledPin, HIGH);             //ON LED.
    delay(100);                             //TIME DIFFERENCE BETWEEN HIGH(ON)& LOW(OFF).                      .
    digitalWrite(ledPin, LOW);              //OFF LED.
    delay(100);
    lcd.setCursor(0,1);
    lcd.print("Fire Detected!!");
  }

  void serialEvent()
{
  while(Serial.available()>0)
  { 
    if(Serial.find("Fire"))
    
      temp=1;
      break;
    }
    else
    {
    temp=0;
    }
  }
}
void gsm_init()
{
  lcd.clear();
  lcd.print("Finding Module..");
  boolean at_flag=1;
  while(at_flag)
  {
    Serial.println("AT");
    delay(1);
    while(Serial.available()>0)
    {
      if(Serial.find("OK"))
      at_flag=0;
    }
    
    delay(1000);
  }
  lcd.clear();
  lcd.print("Module Connected..");
  delay(1000);
  lcd.clear();
  lcd.print("Disabling ECHO");
  boolean echo_flag=1;
  while(echo_flag)
  {
    Serial.println("ATE0");
    while(Serial.available()>0)
    {
      if(Serial.find("OK"))
      echo_flag=0;
    }
    delay(1000);
  }
  lcd.clear();
  lcd.print("Echo OFF");
  delay(1000);
  lcd.clear();
  lcd.print("Finding Network..");
  boolean net_flag=1;
  while(net_flag)
  {
    Serial.println("AT+CPIN?");
    while(Serial.available()>0)
    {
      if(Serial.find("+CPIN: READY"))
      net_flag=0;
    }
    delay(1000);
  }
  lcd.clear();
  lcd.print("Network Found..");
  
  delay(1000);
  lcd.clear();
}
void init_sms()
{
  Serial.println("AT+CMGF=1");
  delay(400);
  Serial.println("AT+CMGS=\"+8801989107903\"");   
  delay(400);
}
void send_data(String message)
{
  Serial.print(message);
 delay(200);
}
void send_sms()
{
  Serial.write(26);
}
void lcd_status()
{
  lcd.clear();
  lcd.print("Message Sent");
  delay(2000);
  lcd.clear();
  lcd.print("System Ready");
  return;
}
void tracking()
{
    init_sms();
    send_data("Fire location detected :\n CRVC+PCR Fatehabad");
    Serial.println(" "); 
    send_data("Get location(See on map)");
    Serial.println(" "); 
    Serial.print("Latitude: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print("\n Longitude: ");
    Serial.println(gps.location.lng(), 6);
 
   // https://www.google.com/maps/@8.2630696,77.3022699,14z
   Serial.print("https://www.google.com/maps/@");
   Serial.print(gps.location.lat(), 6);
   Serial.print(',');
   Serial.print(gps.location.lng(), 6);
   Serial.print(",14z");
    send_sms();
    delay(2000);
    lcd_status();
}

}
