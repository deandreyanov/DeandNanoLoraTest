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
OLED SSD1306  - NANO
---------------------
GND           - GND
VCC           - 5V/3V
SCL           - A5 <-|
|                    | - I2C
SDA           - A4 <-|
*/

/*
Структура пакета:
  Temperature
  Pressure
  Altitude
  voltage
  counter
  time
*/

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

const   long   MILLISEC_PER_SEC    =   1000;
const   long   SEC_PER_MIN         =   60;
const   long   MIN_PER_HOUR        =   60;
const   long   HOUR_PER_DAY        =   24;
 
const   long   MILLISEC_PER_MIN    = MILLISEC_PER_SEC * SEC_PER_MIN; 
const   long   MILLISEC_PER_HOUR   = MILLISEC_PER_MIN * MIN_PER_HOUR; 
const   long   MILLISEC_PER_DAY    = MILLISEC_PER_HOUR * HOUR_PER_DAY;

union float2uint8_t {
  float fVal;
  unsigned char buf[4];
};

float2uint8_t my_float2uint8_t;
float get_temperature = 0;
int32_t get_pressure = 0;
float get_altitude = 0;
long get_counter = 0; // кол-во отправленных посылок
float get_voltage = 0;
long get_time = 0;

int my_adc_voltage = 0;     // отсчёты с аналогового входа
float my_c_voltage = 4.175; // коэффициент для пересчёта в напряжение
float my_voltage = 0;       // напряжение в вольтах
long my_counter = 0;        // кол-во полученных посылок
long my_time = 0;

bool my_get = false; // false - вывод полученных данных, true - вывод собственных данных

int PIN_VOLT = A2;
uint32_t VIEW_PERIOD = 2000;
uint32_t last_time_view = 0;

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
  display.clearDisplay();

  pinMode(PIN_VOLT, INPUT);
}

void serial_print_time(long t_time)
{
  Serial.print("Time = ");
  Serial.print(t_time);
  Serial.println(" ms");
  Serial.print("Time = ");
  byte days = byte(t_time / MILLISEC_PER_DAY);
  t_time %= MILLISEC_PER_DAY;
  byte hours = byte(t_time / MILLISEC_PER_HOUR);
  t_time %= MILLISEC_PER_HOUR;
  byte minutes = byte(t_time / MILLISEC_PER_MIN);
  t_time %= MILLISEC_PER_MIN;
  byte seconds = byte(t_time / MILLISEC_PER_SEC);
  Serial.print(days);
  Serial.print(" ");
  Serial.print(hours);
  Serial.print(":");
  Serial.print(minutes);
  Serial.print(":");
  Serial.print(seconds);
  Serial.println();
}

void serial_print_values()
{
  Serial.print("GET Temperature = ");
  Serial.print(get_temperature);
  Serial.println(" *C");

  Serial.print("GET Pressure    = ");
  Serial.print(get_pressure);
  Serial.println(" Pa");

  Serial.print("GET Altitude    = ");
  Serial.print(get_altitude);
  Serial.println(" meters");

  Serial.print("GET voltage     = ");
  Serial.print(get_voltage);
  Serial.println(" V");

  // print RSSI of packet
  Serial.print("' with RSSI ");
  Serial.println(LoRa.packetRssi());

  Serial.print("GET counter     = ");
  Serial.println(get_counter);

  Serial.print("MY ADC voltage  = ");
  Serial.print(my_adc_voltage);
  Serial.println();

  Serial.print("MY voltage      = ");
  Serial.print(my_voltage);
  Serial.println(" V");

  Serial.print("RX ");
  serial_print_time(my_time);

  Serial.print("TX ");
  serial_print_time(get_time);
}

void display_print_time(long t_time)
{
  byte days = byte(t_time / MILLISEC_PER_DAY);
  t_time %= MILLISEC_PER_DAY;
  byte hours = byte(t_time / MILLISEC_PER_HOUR);
  t_time %= MILLISEC_PER_HOUR;
  byte minutes = byte(t_time / MILLISEC_PER_MIN);
  t_time %= MILLISEC_PER_MIN;
  byte seconds = byte(t_time / MILLISEC_PER_SEC);
  display.print(days);
  display.print(" ");
  display.print(hours);
  display.print(":");
  display.print(minutes);
  display.print(":");
  display.print(seconds);
}

void display_print_get_values()
{
  display.clearDisplay();
  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);             // Start at top-left corner

  display.print(F("TX|"));
  display.print(F("Time="));
  display_print_time(get_time);
  display.println("");

  display.print(F("V="));
  display.print(get_voltage);
  display.println(" mV");

  display.print(F("Snd="));
  display.print(get_counter);

  display.print(F(" Get="));
  display.println(my_counter);

  display.print(F("T="));
  display.print(get_temperature);

  display.print(F("*C"));

  display.display();
}

void display_print_my_values()
{
  display.clearDisplay();
  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);             // Start at top-left corner

  display.print(F("RX|"));
  display.print(F("Time="));
  display_print_time(millis());
  display.println("");
  
  display.print(F("V="));
  display.print(my_voltage);
  display.println(" mV");

  display.print(F("Snd="));
  display.print(get_counter);

  display.print(F(" Get="));
  display.println(my_counter);

  display.display();
}

void get_lora_packet()
{
  // received a packet
  Serial.print("Received packet '");
  //    Temperature = LoRa.read();
  my_float2uint8_t.buf[0] = LoRa.read();
  my_float2uint8_t.buf[1] = LoRa.read();
  my_float2uint8_t.buf[2] = LoRa.read();
  my_float2uint8_t.buf[3] = LoRa.read();
  get_temperature = my_float2uint8_t.fVal;
  //    Pressure = LoRa.read();
  my_float2uint8_t.buf[0] = LoRa.read();
  my_float2uint8_t.buf[1] = LoRa.read();
  my_float2uint8_t.buf[2] = LoRa.read();
  my_float2uint8_t.buf[3] = LoRa.read();
  get_pressure = my_float2uint8_t.fVal;
  //    Altitude = LoRa.read();
  my_float2uint8_t.buf[0] = LoRa.read();
  my_float2uint8_t.buf[1] = LoRa.read();
  my_float2uint8_t.buf[2] = LoRa.read();
  my_float2uint8_t.buf[3] = LoRa.read();
  get_altitude = my_float2uint8_t.fVal;

  my_float2uint8_t.buf[0] = LoRa.read();
  my_float2uint8_t.buf[1] = LoRa.read();
  my_float2uint8_t.buf[2] = LoRa.read();
  my_float2uint8_t.buf[3] = LoRa.read();
  get_voltage = my_float2uint8_t.fVal;

  my_float2uint8_t.buf[0] = LoRa.read();
  my_float2uint8_t.buf[1] = LoRa.read();
  my_float2uint8_t.buf[2] = LoRa.read();
  my_float2uint8_t.buf[3] = LoRa.read();
  get_counter = my_float2uint8_t.fVal;

  my_float2uint8_t.buf[0] = LoRa.read();
  my_float2uint8_t.buf[1] = LoRa.read();
  my_float2uint8_t.buf[2] = LoRa.read();
  my_float2uint8_t.buf[3] = LoRa.read();
  get_time = my_float2uint8_t.fVal;

  my_counter = my_counter + 1;
}

void loop()
{
  my_adc_voltage = analogRead(PIN_VOLT);
  my_voltage = my_c_voltage * my_adc_voltage;

  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(120, 0);
    display.print(F("*"));
    display.display();

    get_lora_packet();
    serial_print_values();
  }

  my_time = millis();
  if (my_time - last_time_view >= VIEW_PERIOD)
  {
    last_time_view = my_time;
    if (my_get)
    {
      display_print_my_values();
    }
    else
    {
      display_print_get_values();
    }

    my_get = not my_get;
  }
}
