/* Схема подключения
---------------------
LORA    - NANO
---------------------
MISO          - D12
SCK           - D13
RST           - D9
GND           - GND
DIO0          - D2
MOSI          - D11
NSS           - D10
V3.3          - 3V
---------------------
BMP180        - NANO
---------------------
GND           - GND
VIN           - 5V/3V
SCL           - A5 <-|
|                    | - I2C
SDA           - A4 <-|
*/
#include <Adafruit_BMP085.h>
#include <LoRa.h>
#include <SPI.h>
#include <Wire.h>
#include <GyverPower.h>
/*
int8_t    :1
uint8_t   :1
int16_t   :2
uint16_t  :2
int32_t   :4
uint32_t  :4
int64_t   :8
uint64_t  :8
*/
union float2uint8_t {
  float fVal;
  unsigned char buf[4];
};

float2uint8_t my_float2uint8_t;
float Temperature = 0;
int32_t Pressure = 0;
float Altitude = 0;
int counter = 0;
int adc_voltage = 0; // отсчёты с аналогового входа
float c_voltage = 4.14; // коэффициент для пересчёта в напряжение
float voltage = 0; // напряжение в вольтах
int min_voltage = 1023;
int max_voltage = 0;

uint32_t last_time_send;
uint32_t SEND_PERIOD = 5000;

Adafruit_BMP085 bmp;

int PIN_VOLT = A2;

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
  pinMode(PIN_VOLT, INPUT);

  autoCalibrate(); // автоматическая калибровка ~ 2 секунды , средняя но достаточная точность

  hardwareDisable(PWR_ALL);
}

void serial_print_values()
{
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

  Serial.print("ADC VOLTAGE = ");
  Serial.print(adc_voltage);  
  Serial.println();
  
  Serial.print("VOLTAGE = ");
  Serial.print(voltage);
  Serial.println(" V");  
}

void lora_send()
{
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
  //  LoRa.write(voltage);
  my_float2uint8_t.fVal = voltage;
  LoRa.write(my_float2uint8_t.buf[0]);
  LoRa.write(my_float2uint8_t.buf[1]);
  LoRa.write(my_float2uint8_t.buf[2]);
  LoRa.write(my_float2uint8_t.buf[3]);
  //  LoRa.write(counter);
  my_float2uint8_t.fVal = counter;
  LoRa.write(my_float2uint8_t.buf[0]);
  LoRa.write(my_float2uint8_t.buf[1]);
  LoRa.write(my_float2uint8_t.buf[2]);
  LoRa.write(my_float2uint8_t.buf[3]);

  LoRa.endPacket();
}

void loop()
{
/*  if (millis() - last_time_send >= SEND_PERIOD)
  {
    last_time_send = millis();
*/
    Temperature = bmp.readTemperature();
    Pressure = bmp.readPressure();
    Altitude = bmp.readAltitude();
    adc_voltage = analogRead(PIN_VOLT);
    voltage = c_voltage * adc_voltage;

    serial_print_values();
    lora_send();

    counter++;
  //}
  sleepDelay(5000);
}
