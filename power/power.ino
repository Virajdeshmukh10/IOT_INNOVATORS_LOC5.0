#include <SoftwareSerial.h>
const int relayPin = 4;
bool relayStatus = 1;
bool relayOFF = 0;
const String phone = "+919370051316"; // white list phone include +91 or your country code

SoftwareSerial gsmSerial(7, 8);

String responseData;
String senderNumber;


double sensorValue1 = 0;
double sensorValue2 = 0;
int crosscount = 0;
int climb_flag = 0;
int val[100];
int max_v = 0;
double VmaxD = 0;
double VeffD = 0;
double Veff = 0;
const int currentPin = A1;
int sensitivity = 200;
int adcValue = 0;
int offsetVoltage = 2500;
double adcVoltage = 0;
double currentValue = 0;

void setup() {
  Serial.begin(9600);
  responseData.reserve(200);
  phone.reserve(20);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
  Serial.begin(9600);
  gsmSerial.begin(9600);
  delay(100);
  gsmSerial.write("AT\r");// because of SIM800L autobounding mode
  delay(100);
  gsmSerial.write("AT+IPR=9600\r");
  delay(100);
  gsmSerial.write("AT+CMGF=1\r");
  delay(500);
  gsmSerial.write("AT+CNMI=2,2,0,0,0\r");
  delay(100);
  Serial.print(gsmSerial.readString());
  gsmSerial.write("AT+CLIP=1\r\n");
  delay(100);
}



void loop()
{
  if (gsmSerial.available() > 0) {
    responseData = gsmSerial.readStringUntil('\n');
    Serial.println(responseData);
    parse();
    delay(1000);
  }
}

void parse() {
  if (responseData.indexOf("CLIP:") > 0) {
    senderNumber = responseData.substring(responseData.indexOf("+CLIP: ") + 8, responseData.indexOf("+CLIP: ") + 21); //PARSE CALLER ID
    Serial.println("Caller number   :");
    Serial.println(senderNumber);
    if (senderNumber == phone) {
      Serial.println("Sender number White list : ok");
      relayStatus = !relayStatus;
      digitalWrite(relayPin, relayStatus);
      gsmSerial.write("ATH\r");  // disconnect call & then send SMS
      gsmSerial.print("AT+CMGS=\"" + phone + "\"\r");
      delay(1000);
      gsmSerial.print("Relay status 1:off 0:on  ");
      gsmSerial.print(relayStatus);
      delay(200);
      gsmSerial.print(relayOFF);
      delay(3000);
      gsmSerial.write(0x1A); //  ctrl+z to send message
      delay(100);

    }
    gsmSerial.write("ATH\r");
    delay(500);

  }

  for ( int i = 0; i < 100; i++ ) {
    sensorValue1 = analogRead(A0);
    if (analogRead(A0) > 511) {
      val[i] = sensorValue1;
    }
    else {
      val[i] = 0;
    }
    delay(1);
  }

  max_v = 0;

  for ( int i = 0; i < 100; i++ )
  {
    if ( val[i] > max_v )
    {
      max_v = val[i];
    }
    val[i] = 0;
  }
  if (max_v != 0) {


    VmaxD = max_v;
    VeffD = VmaxD / sqrt(2);
    Veff = (((VeffD - 420.76) / -90.24) * -210.2) + 210.2;
  }
  else {
    Veff = 0;
  }
  Serial.println("Voltage: ");
  Serial.println(Veff);
  VmaxD = 0;
  delay(500);
  adcValue = analogRead(currentPin);
  adcVoltage = (adcValue / 1024.0) * 5000;
  currentValue = ((adcVoltage - offsetVoltage) / sensitivity);

  Serial.println(" Current = ");
  Serial.println(currentValue, 3);
  Serial.print("A");
  Serial.println("         ");

  delay(500);

}
