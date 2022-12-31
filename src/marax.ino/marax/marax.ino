
#include <Arduino.h>
#include "wiring_private.h"
#include "EasyNextionLibrary.h"
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <dimmable_light.h>

// nunununununununununununununununununununununununununununununun
// nunununununununununununu Wifi
// nunununununununununununununununununununununununununununununun
// Define MQTT / WIFI
#define wifi_ssid ""
#define wifi_password ""

WiFiClient wifiClient;
bool wifiConnected = false;

// nunununununununununununununununununununununununununununununun
// nunununununununununununu Timers
// nunununununununununununununununununununununununununununununun
long serialMaraxUpdateMillis = millis();
long updateMqttTimer = millis();
long readSettigsRefreshTimer = millis();
unsigned long pageRefreshTimer = millis();
unsigned long refresh_timer = millis();
unsigned long activeBrewingStart = millis();

bool POWER_ON = false;

// Serial for Nex
Uart nexSerial(&sercom0, 5, 6, SERCOM_RX_PAD_1, UART_TX_PAD_0);
// EasyNex myNex(nexSerial);
EasyNex myNex(nexSerial);

void SERCOM0_Handler()
{
  nexSerial.IrqHandler();
}

// nunununununununununununununununununununununununununununununun
// nunununununununununununu MQTT Settings
// nunununununununununununununununununununununununununununununun
#define mqtt_server ""
#define mqtt_user ""
#define mqtt_password ""

#define brewtemp_topic "marax/sensor/brewtemp"
#define steamtemp_topic "marax/sensor/steamtemp"
#define steamtargettemp_topic "marax/sensor/steamtargettemp"
#define fastheat_topic "marax/sensor/fastheat_timer"
#define heatingElement_topic "marax/sensor/heatingelement"
#define debug_topic "marax/sensor/debug"
#define shots_topic "marax/sensor/shots"
#define power_topic "marax/sensor/power_state"
#define remoteProfileEnabled_topic "marax/sensor/pressureProfilingEnabled"
#define pressureProfileEnabled_topic "marax/sensor/remoteProfileEnabled"

PubSubClient mqttClient(mqtt_server, 1883, callbackfun, wifiClient);

// nunununununununununununununununununununununununununununununun
// nunununununununununununu CONSTS
// nunununununununununununununununununununununununununununununun
#define REFRESH_SCREEN_EVERY 150 // Screen refresh interval (ms)
#define PUMP_RANGE 255;

const int REFRESH_TIME = 100;

bool brewActive = false;
bool brewTimerActive = false; // active if brewing or descaling

// nunununununununununununununununununununununununununununununun
// nunununununununununununu MaraxSerialStuff
// nunununununununununununununununununununununununununununununun
const byte numCharsSerialMarax = 32;
char receivedCharsFromMarax[numCharsSerialMarax];
static byte ndx = 0;
char endMarker = '\n';
char rc;
int noSerialCount = 0;

// Serial Marax Sensors
int brewTemp = 0;
int steamTemp = 0;
int steamTargetTemp = 0;
int maraxMode = 0;
int fastHeatingCountDown = 0;
bool heatingElementOn = false;

// nunununununununununununununununununununununununununununununun
// nunununununununununununu Pins
// nunununununununununununununununununununununununununununununun
int brewSwitchRelayPin = 10; // Relay to tell MaraxCard that a brew is active
int brewSwitchPin = 11;      // Brew Switch

float i = 0;
float V, P, B;
uint32_t number = 0;
int inPin = 2;
int pinOutput = 0;

int shotCount = 0;

float voltage;
float bar;
uint32_t barGraphValue;
int analog = 0;
float pressure;
int analogPressurePin = A1;

bool pressureProfilingEnabled = false;
bool remoteProfilingEnabled = false;

float remoteProfileArray[50];

bool cleaningModeActive = 0;
bool cleaningRunActive = 0;
int cleaningShots = 0;
int cleaningShotsWater = 0;

int brewSwitchAnalogValue = 0;

uint32_t currentPageId;
int lastPageId;
bool displayIsInSleep = true;

// nunununununununununununununununununununununununununununununun
// nunununununununununununu Pressure Profile
// nunununununununununununununununununununununununununununununun
int t1p = 0, t1t = 0, t2p = 0, t2t = 0, t3p = 0, t3t = 0, t4p = 0, t4t = 0;
int t1pWave = 0, t2pWave = 0, t3pWave = 0, t4pWave = 0;

// nunununununununununununununununununununununununununununununun
// nunununununununununununu LowPassFilterStuff
// nunununununununununununununununununununununununununununununun
const float alpha = 0.97; // Low Pass Filter alpha (0 - 1 )
float filteredVal = 512.0;
float sensorVal;

DimmableLight pump(3);

// nunununununununununununununununununununununununununununununun
// nunununununununununununu Setup
// nunununununununununununununununununununununununununununununun
void setup()
{
  // Set Marax Brew Switch Relay to off on startup
  pinMode(brewSwitchRelayPin, INPUT_PULLUP);
  delay(20);

  // AC DimmerStuff
  DimmableLight::setSyncPin(2);
  DimmableLight::begin();
  // Set pump 100% to allow mcu to still control
  pump.setBrightness(255);

  pinMode(brewSwitchPin, INPUT_PULLUP);
  // Marax Brew Switch Relay output
  pinMode(brewSwitchRelayPin, OUTPUT);

  // start serial port at 960                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                0 bps:
  // Reassign pins 5 and 6 to SERCOM alt
  pinPeripheral(5, PIO_SERCOM_ALT);
  pinPeripheral(6, PIO_SERCOM_ALT);

  delay(200);
  Serial1.begin(9600);
  myNex.begin(115200);
  delay(2000);
  Serial1.write(0x11);

  // Serial Marax
  memset(receivedCharsFromMarax, 0, numCharsSerialMarax);
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD)
  {
    // Serial.println("Debug: NO WIFI");
    // Serial.println("WiFi shield not present");
    while (true)
      ;
  }
  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0")
  {
    // Serial.println("Please upgrade the firmware");
  }
  WiFi.setHostname("MaraXController");

  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED)
  {
    // Serial.println("Debug: NO d WIFI");
    delay(500);
    // Serial.print(".");
    WiFi.begin(wifi_ssid, wifi_password);
  }
  delay(2000);
}

// nunununununununununununununununununununununununununununununun
// nunununununununununununu Main Loop
// nunununununununununununununununununununununununununununununun
void loop()
{
  if (!mqttClient.connected())
  {
    reconnect();
  }
  mqttClient.loop();
  myNex.NextionListen();

  getMaschineInput();
  updateDisplay();
  updateMqtt();
  brewDetect();

  liveData();
  pressureProfile();

  refresh_timer = millis();
}
void reconnect()
{
  // Loop until we're reconnected
  while (!mqttClient.connected())
  {
    // Attempt to connect
    if (mqttClient.connect("MaraXMod", mqtt_user, mqtt_password))
    {
      mqttClient.subscribe("marax/remoteProfile");
    }
    else
    {
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void callbackfun(char *topic, byte *payload, unsigned int length)
{

  String topicFromCallback = topic;
  if (topicFromCallback == "marax/remoteProfile")
  {
    int pos = 0;
    int payloadSize = sizeof(payload);
    char chars[payloadSize];
    memcpy(chars, payload, payloadSize);

    char temp[5];
    int charTempIndex = 0;
    int index = 0;
    for (int i = 0; i < strlen(chars); i++)
    {
      if (chars[i] != ',')
      {
        temp[charTempIndex] = chars[i];
        charTempIndex++;
      }
      else if (chars[i] == ',')
      {
        remoteProfileArray[index] = strtod(temp, NULL);
        charTempIndex = 0;
        index++;
      }
    }
    index = 0;
  }
}
void updateMqtt()
{
  if ((millis() - updateMqttTimer > 5000) && !brewActive && POWER_ON)
  {
    mqttClient.publish(debug_topic, toCharArray(String(receivedCharsFromMarax)), true);
    mqttClient.publish(brewtemp_topic, toCharArray(String(brewTemp)), true);
    mqttClient.publish(steamtemp_topic, toCharArray(String(steamTemp)), true);
    mqttClient.publish(steamtargettemp_topic, toCharArray(String(steamTargetTemp)), true);
    mqttClient.publish(fastheat_topic, toCharArray(String(fastHeatingCountDown)), true);
    mqttClient.publish(heatingElement_topic, toCharArray(String(heatingElementOn)), true);
    mqttClient.publish(shots_topic, toCharArray(String(shotCount)), true);
    mqttClient.publish(power_topic, toCharArray(String(POWER_ON)), true);

    updateMqttTimer = millis();
  }
}
char *toCharArray(String str)
{
  return &str[0];
}
// Gets "live" Info  during brew
void liveData()
{
  if (brewActive)
  {
    // Write Brew Temp
    myNex.writeNum("brewTemp", brewTemp);
    // Write BrewTimer
    myNex.writeNum("brewTime", (int)((millis() - activeBrewingStart) / 1000));
    // Write Live Pressure as normal number
    float pressure = getPressure();
    int pressureInt = pressure * 100;
    myNex.writeNum("barvar.val", pressureInt);
    // Map the pressure to wave pixels in graph
    barGraphValue = map(pressureInt, 0, 1000, 0, 164);
    myNex.writeNum("barwave.val", barGraphValue);
  }
}

float getPressure()
{
  // returns sensor pressure data
  // set up voltage divider
  //  3.3V/1024
  //  voltageZero = 0.3V
  //  voltageMax = 3V 940 Max
  //  pressure gauge range 0-12 bar

  sensorVal = (float)analogRead(A1);                                 // Read pressure sensor val (AA)
  filteredVal = (alpha * filteredVal) + ((1.0 - alpha) * sensorVal); // Low Pass Filter
  voltage = (filteredVal / 1024.0) * 3.3;                            // calculate voltage
  float Pressure = (voltage - 0.3) / 0.212;

  if (Pressure < 0)
  {
    return 0;
  }
  else
  {
    return Pressure;
  }
}

void readSettigs()
{
  if ((millis() - readSettigsRefreshTimer > 4000) && !brewActive)
  {

    // Just read some times
    getPressure();

    pressureProfilingEnabled = myNex.readNumber("pPEnabled");
    remoteProfilingEnabled = myNex.readNumber("remoteEnabled");

    // Profiles
    int temp = myNex.readNumber("t1p");
    if (temp != t1p)
    {
      t1p = temp;
      t1pWave = map(temp, 0, 10, 0, 164);
    }
    temp = myNex.readNumber("t2p");
    if (temp != t2p)
    {
      t2p = temp;
      t2pWave = map(temp, 0, 10, 0, 164);
    }
    temp = myNex.readNumber("t3p");
    if (temp != t3p)
    {
      t3p = temp;
      t3pWave = map(temp, 0, 10, 0, 164);
    }
    temp = myNex.readNumber("t4p");
    if (temp != t4p)
    {
      t4p = temp;
      t4pWave = map(temp, 0, 10, 0, 164);
    }

    t1t = myNex.readNumber("t1t");
    t2t = myNex.readNumber("t2t");
    t3t = myNex.readNumber("t3t");
    t4t = myNex.readNumber("t4t");

    readSettigsRefreshTimer = millis();
  }
}
void updateDisplay()
{

  if ((millis() > pageRefreshTimer) && !brewActive)
  {
    if (POWER_ON && displayIsInSleep)
    {
      myNex.writeNum("sleep", 0);
      displayIsInSleep = false;
    }
    // Set Controller to sleep if Machine is off
    else if (!POWER_ON && !displayIsInSleep)
    {
      myNex.writeStr("page home");
      myNex.writeNum("sleep", 1);
      displayIsInSleep = true;
    }

    // Send global vars always
    myNex.writeNum("brewTemp", brewTemp);
    myNex.writeNum("steamTemp", steamTemp);

    currentPageId = myNex.readNumber("dp");

    // Save Settings im Page changes
    // Refresh Pages that only need one time refresh
    if (currentPageId != lastPageId)
    {
      lastPageId = currentPageId;
    }

    // We Changed to More Settings
    if (currentPageId == 4278190086 || currentPageId == 6)
    {

      myNex.writeNum("tarsteam.val", steamTargetTemp);
      delay(5);
      myNex.writeNum("fastheattimer.val", fastHeatingCountDown);
      delay(5);
      myNex.writeNum("heatingel.val", heatingElementOn);
      delay(5);
    }

    // Dont try to read setting when we are brewing to save time
    if (currentPageId != 4278190082 || currentPageId != 2)
    {
      readSettigs();
    }

    // Cleaing Mode Settings
    if (currentPageId == 4278190085 || currentPageId == 5)
    {
      cleaningModeActive = true;
    }
    else
    {
      cleaningShots = 0;
      cleaningShotsWater = 0;
      cleaningModeActive = false;
    }

    pageRefreshTimer = millis() + REFRESH_SCREEN_EVERY;
  }
}

void setPressure(float targetValue)
{

  int pumpValue;
  float currentPressure = (getPressure() - 1.7f);
  float diff = targetValue - currentPressure;

  if (targetValue == 0 || currentPressure > targetValue)
  {
    pumpValue = 0;
  }
  else
  {
    float diff = targetValue - currentPressure;
    pumpValue = 255 / (1.f + exp(2.f - diff / 0.9f));
  }
  pump.setBrightness(pumpValue);
}

void pressureProfile()
{
  // Check if Brew is active and pressure Profiling is enabled
  if (brewActive && pressureProfilingEnabled)
  {
    int brewSecs = (int)((millis() - activeBrewingStart) / 1000);

    if (brewSecs <= t1t)
    {
      myNex.writeNum("setbar.val", t1pWave);
      setPressure(t1p);
    }
    else if (brewSecs <= (t2t + t1t))
    {
      myNex.writeNum("setbar.val", t2pWave);
      setPressure(t2p);
    }
    else if (brewSecs <= (t3t + t2t + t1t))
    {
      myNex.writeNum("setbar.val", t3pWave);
      setPressure(t3p);
    }
    else if (brewSecs <= (t4t + t3t + t2t + t1t))
    {
      myNex.writeNum("setbar.val", t4pWave);
      setPressure(t4p);

      if ((t4t + t3t + t2t + t1t) - brewSecs <= 3)
      {
        myNex.writeNum("n0.pco", 64864);
        myNex.writeNum("n1.pco", 64864);
      }
    }
    else if (brewSecs > (t4t + t3t + t2t + t1t))
    {
      // AT this Point Brewing is done
      myNex.writeNum("n0.pco", 1535);
      myNex.writeNum("n1.pco", 1535);
      myNex.writeNum("setbar.val", 0);
      pump.setBrightness(0);
    }
  }
  // Remote Profil
  if (brewActive && remoteProfilingEnabled)
  {
    // TODO: This will be implement later
  }
}

void brewDetect()
{
  if (brewState())
  {

    if (cleaningModeActive && !cleaningRunActive)
    {
      myNex.writeNum("cleanTimer", 1);
      cleaningRunActive = true;
      cleaningShots++;
      if (cleaningShots < 6)
      {
        myNex.writeNum("n0.val", cleaningShots);
        if (cleaningShots == 5)
        {
          myNex.writeNum("n0.pco", 2047);
        }
      }
      else
      {
        myNex.writeNum("n0.pco", 65535);
        cleaningShotsWater++;
        myNex.writeNum("n0.val", cleaningShotsWater);
        if (cleaningShotsWater == 5)
        {
          myNex.writeNum("n0.pco", 2047);
        }
      }
    }

    if (!brewActive && !cleaningModeActive)
    {
      myNex.writeStr("page brew");
      delay(10);
      brewTimer(true); // nextion timer start
      myNex.writeNum("pBrew.pic", 25);
      brewActive = true;
    }
  }
  else
  {
    brewActive = false;
    brewTimer(false);
    myNex.writeNum("pBrew.pic", 8);
    pump.setBrightness(255);

    // Reset cleaningRunActive
    if (cleaningRunActive)
    {
      myNex.writeNum("cleanTimer", 0);
    }
    cleaningRunActive = false;
  }
}

// Function to get the state of the brew switch button
bool brewState()
{
  // Brewswitch Pin is 1 in Off Postion and when on is 0
  // Only of the Machine is On and the Leaver is up we set brew to active
  // Power Led is connected to gnd 0 -> ON 1-> OFF
  if (digitalRead(brewSwitchPin) == LOW)
  {
    // Give the Data back to Marax MCU to let it know we are making coffee
    //  The Mcu will heat to keep temp and for better temps
    // Write to NC Relay
    digitalWrite(brewSwitchRelayPin, false);
    return true;
  }
  else
  {
    digitalWrite(brewSwitchRelayPin, true);
    return false;
  }
}

// C1.19,116,124,095,0560,0
void getMaschineInput()
{

  while (Serial1.available())
  {
    rc = Serial1.read();
    if (rc != endMarker)
    {
      receivedCharsFromMarax[ndx] = rc;
      ndx++;
      if (ndx >= numCharsSerialMarax)
      {
        ndx = numCharsSerialMarax - 1;
      }
    }
    else
    {
      receivedCharsFromMarax[ndx] = '\0';
      ndx = 0;

      // Make sure we have valid data
      if (!receivedCharsFromMarax[25])
      {
        // Marax Brew Temp
        if (receivedCharsFromMarax[14] && receivedCharsFromMarax[15] && receivedCharsFromMarax[16])
        {
          String temp = String(receivedCharsFromMarax[14]) + String(receivedCharsFromMarax[15]) + String(receivedCharsFromMarax[16]);
          brewTemp = temp.toInt();
        }
        // Marax Steam Temp
        if (receivedCharsFromMarax[6] && receivedCharsFromMarax[7] && receivedCharsFromMarax[8])
        {
          String temp = String(receivedCharsFromMarax[6]) + String(receivedCharsFromMarax[7]) + String(receivedCharsFromMarax[8]);
          steamTemp = temp.toInt();
        }
        // Marax Target Temp
        if (receivedCharsFromMarax[10] && receivedCharsFromMarax[11] && receivedCharsFromMarax[12])
        {
          String temp = String(receivedCharsFromMarax[10]) + String(receivedCharsFromMarax[11]) + String(receivedCharsFromMarax[12]);
          steamTargetTemp = temp.toInt();
        }
        // Marax BoostTimer
        if (receivedCharsFromMarax[18] && receivedCharsFromMarax[19] && receivedCharsFromMarax[20] && receivedCharsFromMarax[21])
        {
          String temp = String(receivedCharsFromMarax[18]) + String(receivedCharsFromMarax[19]) + String(receivedCharsFromMarax[20]) + String(receivedCharsFromMarax[21]);
          fastHeatingCountDown = temp.toInt();
        }
        // Marax Heat Mode
        if (receivedCharsFromMarax[23])
        {
          String temp = String(receivedCharsFromMarax[23]);
          heatingElementOn = temp.toInt();
        }
      }
    }
  }
  if (receivedCharsFromMarax[0] != NULL)
  {
    POWER_ON = true;
    noSerialCount = 0;
  }
  else
  {
    noSerialCount++;
    if (noSerialCount == 3000)
    {
      POWER_ON = false;
      brewTemp = 0;
      steamTemp = 0;
      steamTargetTemp = 0;
      fastHeatingCountDown = 0;
      heatingElementOn = 0;

      //Reset all MQTT Values to 0
      mqttClient.publish(brewtemp_topic, toCharArray(String(brewTemp)), true);
      mqttClient.publish(steamtemp_topic, toCharArray(String(steamTemp)), true);
      mqttClient.publish(steamtargettemp_topic, toCharArray(String(steamTargetTemp)), true);
      mqttClient.publish(fastheat_topic, toCharArray(String(fastHeatingCountDown)), true);
      mqttClient.publish(heatingElement_topic, toCharArray(String(heatingElementOn)), true);
      mqttClient.publish(shots_topic, toCharArray(String(shotCount)), true);
      mqttClient.publish(power_topic, toCharArray(String(POWER_ON)), true);
    }
  }

  // Get Serial Update
  if (millis() - serialMaraxUpdateMillis > 5000)
  {
    serialMaraxUpdateMillis = millis();
    memset(receivedCharsFromMarax, 0, numCharsSerialMarax);
    Serial1.write(0x11);
  }
}

void brewTimer(bool start)
{ // small function for easier brew timer start/stop
  if (!brewTimerActive && start)
  {
    myNex.writeNum("activeBrewTime", 0);
    myNex.writeNum("timerState", 1);
    activeBrewingStart = millis();
    brewTimerActive = true;
  }
  else if (!start)
  {
    if (brewTimerActive)
    {
      myNex.writeNum("timerState", 0);
      int brewSecs = (int)((millis() - activeBrewingStart) / 1000);
      if (brewSecs > 20)
      {
        shotCount++;
      }
    }
    brewTimerActive = false;
  }
}
