// ****** INCLUDES ****** https://github.com/4-20ma/ModbusMaster
#include <ModbusMaster.h>
#include <LiquidCrystal.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// ****** DEFINES ******
//#define MAX485_DE    3
//#define MAX485_RE_NEG  2

#define MAX485_RE_NEG  4 //D4 RS485 has a enable/disable pin to transmit or receive data. Arduino Digital Pin 2 = Rx/Tx 'Enable'; High to Transmit, Low to Receive
#define Slave_ID       1
#define RX_PIN      16 //RX2 
#define TX_PIN      17  //TX2 
#define ONBOARD_LED 22

/* Comment this out to disable prints and save space */
//#define BLYNK_PRINT Serial

// ****** GLOBALS ******
ModbusMaster node;  // instantiate ModbusMaster object
float reg1;   // will store the float value found at 3910/3911
float reg2;   // will store the float value found at 3914/3915
int low_word;   // this will hold the low 16-bit resgter value after a read
int high_word;  // this will hold the high 16-bit resgter value after a read

//***** Blynk ******/
char auth[] = "AUTH";
char ssid[] = "SSID";
char pass[] = "PASSWOD";

BlynkTimer timer;

// converting to floating point - union structure
union u_tag {
  int bdata[2];
  float flotvalue;
} uniflot;


void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, HIGH); //Switch to transmit data
}
void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, LOW); //Switch to receive data
}


// ****** STANDARD ARDUINO SETUP FUNCTION ******
void setup() {
 pinMode(MAX485_RE_NEG, OUTPUT);
 pinMode(ONBOARD_LED , OUTPUT);
 digitalWrite(ONBOARD_LED, LOW);

  // use Serial (port 0); initialize Modbus communication baud rate
  //Serial.begin(9600);

  //****** Blynk *********
  Blynk.begin(auth, ssid, pass);

  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  // communicate with Modbus slave ID 2 over Serial (port 0)
  node.begin(1, Serial2);



  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  timer.setInterval(2000L, myTimerEvent);

}

void myTimerEvent()
{
  uint8_t result;

  
   result = node.readInputRegisters(3000, 50);
   //result = node.readHoldingRegisters(3000, 50);
   if (result == node.ku8MBSuccess)
  {
    digitalWrite(ONBOARD_LED, HIGH);
    int pv1InputVoltage = node.getResponseBuffer(0x00);
    int pv2InputVoltage = node.getResponseBuffer(0x01);
    int pv3InputVoltage = node.getResponseBuffer(0x02);  

    int pv1InputCurrent = node.getResponseBuffer(0x03);
    int pv2InputCurrent = node.getResponseBuffer(0x04);
    int pv3InputCurrent = node.getResponseBuffer(0x05); 

    //INPUT POWER
    int pv1InputPower1 = node.getResponseBuffer(0x06);
    int pv1InputPower2 = node.getResponseBuffer(0x07);
    
    int pv2InputPower1 = node.getResponseBuffer(0x08);
    int pv2InputPower2 = node.getResponseBuffer(0x09);
    
    int pv3InputPower1 = node.getResponseBuffer(0x0A); 
    int pv3InputPower2 = node.getResponseBuffer(0x0B); 
    //End INPUT POWER
    
    int rsPhaseGridVoltage = node.getResponseBuffer(0x0E);
    int stPhaseGridVoltage = node.getResponseBuffer(0x0F);
    int trPhaseGridVoltage = node.getResponseBuffer(0x10); 

    int rsPhaseGridFrequency = node.getResponseBuffer(0x11);
    int stPhaseGridFrequency = node.getResponseBuffer(0x12);
    int trPhaseGridFrequency = node.getResponseBuffer(0x13); 

    int rPhaseGridTeedCurrent = node.getResponseBuffer(0x14);
    int sPhaseGridTeedCurrent = node.getResponseBuffer(0x15);
    int tPhaseGridTeedCurrent = node.getResponseBuffer(0x16); 

    //GRID-TIED POWER
    int gridTiedPower1 = node.getResponseBuffer(0x17);
    int gridTiedPower2 = node.getResponseBuffer(0x18); 

    //TEMPERATURE
    int radiadorTemperature = node.getResponseBuffer(0x19);
    int moduleTemperature = node.getResponseBuffer(0x1A); 

    //OPERATING MODE OF THE INVERTER
    int operationMode = node.getResponseBuffer(0x1E);

     //TOTAL ENERGY YIELD
    int totalEnergyYield1 = node.getResponseBuffer(0x26);
    int totalEnergyYield2 = node.getResponseBuffer(0x27); 

    //ANNUAL ENERGY YEILD
    int annualEnergy1 = node.getResponseBuffer(0x28);
    int annualEnergy2 = node.getResponseBuffer(0x29); 

    //DAILY ENERGY YIELD
    int dailyEnergy1 = node.getResponseBuffer(0x2A);
    int dailyEnergy2 = node.getResponseBuffer(0x2B); 


    Blynk.virtualWrite(V1,                 (pv1InputVoltage / 10.0));
    Blynk.virtualWrite(V2,                 (pv2InputVoltage / 10.0));
    //Blynk.virtualWrite(V3,                 pv3InputVoltage);

    float pv1InputCurrentFloat = (1000 /100.0);
    //Serial.println(pv1InputCurrentFloat)
    Blynk.virtualWrite(V4,                 (pv1InputCurrent /100.0));
    Blynk.virtualWrite(V5,                 (pv2InputCurrent /100.0));
    //Blynk.virtualWrite(V6,                 pv3InputCurrent);

    //fazer o calculo de pv1InputVoltage * pv1InputCurrent

    
    Blynk.virtualWrite(V7,               ((pv1InputVoltage / 10.0) * (pv1InputCurrent /100.0)));
    Blynk.virtualWrite(V8,               ((pv2InputVoltage / 10.0) * (pv2InputCurrent /100.0)));
    //Blynk.virtualWrite(V9,               (pv1InputVoltage * pv1InputCurrent));

    Blynk.virtualWrite(V10,                 (rsPhaseGridVoltage / 10.0));
    Blynk.virtualWrite(V11,                 (stPhaseGridVoltage / 10.0));
    Blynk.virtualWrite(V12,                 (trPhaseGridVoltage / 10.0));

    Blynk.virtualWrite(V13,                 (rPhaseGridTeedCurrent / 100.0));
    Blynk.virtualWrite(V14,                 (sPhaseGridTeedCurrent / 100.0));
    Blynk.virtualWrite(V15,                 (tPhaseGridTeedCurrent / 100.0));

    //GRID-TIED POWER ()
    Blynk.virtualWrite(V16,                 ((rsPhaseGridVoltage * rPhaseGridTeedCurrent ) + ( stPhaseGridVoltage * sPhaseGridTeedCurrent ) + ( trPhaseGridVoltage * tPhaseGridTeedCurrent ))/ 1000);

    //Temperature
    Blynk.virtualWrite(V17,                 (radiadorTemperature / 10.0));
    Blynk.virtualWrite(V18,                 (moduleTemperature / 10.0 ));

    Blynk.virtualWrite(V19,                 operationMode);

    Blynk.virtualWrite(V20,                 dailyEnergy2);    
    
    node.clearResponseBuffer();
 }  
}

// ****** STANDARD ARDUINO LOOP FUNCTION ******
void loop() {   

  Blynk.run();
  timer.run(); // Initiates BlynkTimer
 
}