#include <SPI.h>
#include <LoRa.h>

union float2uint8_t {
  float fVal;
  unsigned char buf[4];
};

float2uint8_t my_float2uint8_t;
float Temperature = 0;
int32_t Pressure = 0;
float Altitude = 0;
int counter = 0;
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
}

void loop()
{
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
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
    counter = LoRa.read();

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

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
    Serial.print("counter = ");
    Serial.println(counter);
  }
}
