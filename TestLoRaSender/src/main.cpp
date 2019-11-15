#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

union float2uint8_t {
  float fVal;
  unsigned char buf[4];
};

float2uint8_t my_float2uint8_t;
float Temperature = 0;
int32_t Pressure = 0;
float Altitude = 0;
int counter = 0;

Adafruit_BMP085 bmp;

void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
  }

  Serial.println("LoRa Sender");

  if (!LoRa.begin(433E6))
  {
    Serial.println("Starting LoRa failed!");
    while (1)
    {
    }
  }

  if (!bmp.begin())
  {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1)
    {
    }
  }
}

void loop()
{
  Temperature = bmp.readTemperature();
  Pressure = bmp.readPressure();
  Altitude = bmp.readAltitude();

  Serial.print("Sending packet: ");
  Serial.println(counter);

  Serial.print("Temperature = ");
  Serial.print(Temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(Pressure);
  Serial.println(" Pa");

  Serial.print("Altitude = ");
  Serial.print(Altitude);
  Serial.println(" meters");
  Serial.println();

  LoRa.beginPacket();  
//  LoRa.write(Temperature);
  my_float2uint8_t.fVal = Temperature;
  LoRa.write(my_float2uint8_t.buf[0]);
  LoRa.write(my_float2uint8_t.buf[1]);
  LoRa.write(my_float2uint8_t.buf[2]);
  LoRa.write(my_float2uint8_t.buf[3]);
//  LoRa.write(Pressure);
  my_float2uint8_t.fVal = Pressure;
  LoRa.write(my_float2uint8_t.buf[0]);
  LoRa.write(my_float2uint8_t.buf[1]);
  LoRa.write(my_float2uint8_t.buf[2]);
  LoRa.write(my_float2uint8_t.buf[3]);
//  LoRa.write(Altitude);
  my_float2uint8_t.fVal = Altitude;
  LoRa.write(my_float2uint8_t.buf[0]);
  LoRa.write(my_float2uint8_t.buf[1]);
  LoRa.write(my_float2uint8_t.buf[2]);
  LoRa.write(my_float2uint8_t.buf[3]);

  LoRa.write(counter);
  LoRa.endPacket();
  counter++;
  delay(5000);
}
