#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

float Temperature = 0;
int32_t Pressure = 0;
float Altitude = 0;
int counter = 0;

Adafruit_BMP085 bmp;

void setup()
{
  Serial.begin(9600);
  while (!Serial){}

  Serial.println("LoRa Sender");

  if (!LoRa.begin(433E6))
  {
    Serial.println("Starting LoRa failed!");
    while (1){}
  }

  if (!bmp.begin()) 
  {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
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
  LoRa.write(Temperature);
  LoRa.write(Pressure);
  LoRa.write(Altitude);
  LoRa.write(counter);
  LoRa.endPacket();
  counter++;
  delay(5000);
}
