#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

union float2uint8_t {
  float fVal;
  unsigned char buf[4];
};

float2uint8_t my_float2uint8_t;
float Temperature = 0;
int32_t Pressure = 0;
float Altitude = 0;
int counter = 0;
float voltage = 0;
int prev_counter = 0;

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;

  Serial.println("LoRa Receiver");

  if (!LoRa.begin(433E6))
  {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    //for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
}

void loop()
{
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    display.clearDisplay();
    // received a packet
    Serial.print("Received packet '");
    //    Temperature = LoRa.read();
    my_float2uint8_t.buf[0] = LoRa.read();
    my_float2uint8_t.buf[1] = LoRa.read();
    my_float2uint8_t.buf[2] = LoRa.read();
    my_float2uint8_t.buf[3] = LoRa.read();
    Temperature = my_float2uint8_t.fVal;
    //    Pressure = LoRa.read();
    my_float2uint8_t.buf[0] = LoRa.read();
    my_float2uint8_t.buf[1] = LoRa.read();
    my_float2uint8_t.buf[2] = LoRa.read();
    my_float2uint8_t.buf[3] = LoRa.read();
    Pressure = my_float2uint8_t.fVal;

    //    Altitude = LoRa.read();
    my_float2uint8_t.buf[0] = LoRa.read();
    my_float2uint8_t.buf[1] = LoRa.read();
    my_float2uint8_t.buf[2] = LoRa.read();
    my_float2uint8_t.buf[3] = LoRa.read();
    Altitude = my_float2uint8_t.fVal;

    my_float2uint8_t.buf[0] = LoRa.read();
    my_float2uint8_t.buf[1] = LoRa.read();
    my_float2uint8_t.buf[2] = LoRa.read();
    my_float2uint8_t.buf[3] = LoRa.read();
    voltage = my_float2uint8_t.fVal;

    my_float2uint8_t.buf[0] = LoRa.read();
    my_float2uint8_t.buf[1] = LoRa.read();
    my_float2uint8_t.buf[2] = LoRa.read();
    my_float2uint8_t.buf[3] = LoRa.read();
    counter = my_float2uint8_t.fVal;

    Serial.print("Temperature = ");
    Serial.print(Temperature);
    Serial.println(" *C");

    Serial.print("Pressure = ");
    Serial.print(Pressure);
    Serial.println(" Pa");

    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print("Altitude = ");
    Serial.print(Altitude);
    Serial.println(" meters");

    Serial.print("VOLTAGE = ");
    Serial.print(voltage);
    Serial.println(" V");

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
    Serial.print("counter = ");
    Serial.println(counter);

    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);             // Start at top-left corner

    display.print(F("T="));
    display.print(Temperature);
    display.print("*C|");

    display.print(F("p="));
    display.print(Pressure);
    display.print(" Pa");

    display.print(F("H="));
    display.print(Altitude);
    display.println(" m");

    display.print(F("V="));
    display.print(voltage);
    display.println(" mV");

    display.print(F("Cnt="));
    display.print(counter);

    display.print(F("|Time="));
    display.println(millis());

    display.display();
  }
}
