#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <Adafruit_BMP280.h>

#define BLYNK_TEMPLATE_ID           "TMPL3HvRQ5n99"
#define BLYNK_TEMPLATE_NAME         "IntelliCap"
#define BLYNK_AUTH_TOKEN "BTkct42lPcXLysRb8JZPd35V9pBRQVMV"

char ssid[] = "Tryme";
char pass[] = "bruhhhhh";

static const int MAIN_LED = 32;
static const int VOLT_PIN = 35;
static const int SENSOR_PIN = 5;
static const int IRLED_PIN = 19;
static const int GAS_PIN = 33;
static const int BUZZ_PIN = 18;


static const float R1 = 220000.0; 
static const float R2 = 100000.0; 
static const float Vin = 3.7; 

float temperature;
float pressure;
float sensorValue;
float VBattery;
float gas;
int state=0;
int state2=0;

int switchState = 0;


Adafruit_BMP280 bmp;

BLYNK_WRITE(V6)  
{
  switchState = param.asInt();  
}

void setup() {

  Serial.begin(9600);
  delay(1000);

  bmp.begin(0x76);
  
  pinMode(MAIN_LED,OUTPUT);
  pinMode(IRLED_PIN,OUTPUT);
  pinMode(VOLT_PIN,INPUT);
  pinMode(SENSOR_PIN, INPUT);
  pinMode(GAS_PIN, INPUT);

  ledcSetup(0, 0, 10); 
  ledcAttachPin(BUZZ_PIN, 0);

  ledcSetup(1, 0, 10); 
  ledcAttachPin(MAIN_LED, 1);

  Blynk.begin(BLYNK_AUTH_TOKEN,ssid, pass);
  ledcWrite(1, 1024);
  digitalWrite(IRLED_PIN ,LOW);

}

void loop() {

  Blynk.run();  

  for(int i=0;i<1000;i++){
     sensorValue += analogRead(VOLT_PIN);  
     gas += analogRead(GAS_PIN);       
  }
  sensorValue=sensorValue/1000;
  gas = gas/1000;

  float Vout = sensorValue * Vin / 4095.0; 
  VBattery = Vout * (R1 + R2) / R2;
  
  temperature = bmp.readTemperature();
  pressure = bmp.readPressure()/1000;  

  state = digitalRead(SENSOR_PIN);
   if(state==1){
    digitalWrite(IRLED_PIN ,HIGH);
  }else{
    digitalWrite(IRLED_PIN ,LOW);
  }

  if(gas>500){
    ledcWrite(0, 512);
    state2=1;
  }
  
  if (switchState == 1)  // If the Switch is turned ON
  {
    ledcWrite(0, 0);    
    state2=0;
  }

  if(VBattery<3.2){
    ledcWrite(1, 512); 
  }else{
    ledcWrite(1, 1024);
  }
  
  Serial.println(temperature,2);
  Serial.println(pressure,2);
  Serial.println(VBattery,2);
  Serial.println(gas); 

  
  Blynk.virtualWrite(V5,state2);
  Blynk.virtualWrite(V1,temperature);
  Blynk.virtualWrite(V2,pressure);
  Blynk.virtualWrite(V3,VBattery);
  Blynk.virtualWrite(V4,state);
}
