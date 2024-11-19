#include <Arduino.h>
#include "nrf24_device.h"
#include "main.h"

nRF24Device nrf24_device{FSPI, NRF24_SCK, NRF24_MISO, NRF24_MOSI, NRF24_CSN, NRF24_IRQ, NRF24_CE};
uint8_t send_buffer[128];
size_t len;
bool send_flag = false;

void IRAM_ATTR onReceive()
{
  len = Serial.available();
  if (len > 0)
  {
    size_t bytesRead = Serial.read(send_buffer, len);
    // Serial1.print("Received data: "); 
    // for (size_t i = 0; i < bytesRead; i++)
    // {
    //   Serial1.print(send_buffer[i], HEX);
    //   Serial1.print(" ");
    // }
    send_flag = true;
  }
}
void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 18, 17);
  Serial1.println("Hello World");
  pinMode(37, OUTPUT);
  digitalWrite(37, HIGH);
  byte addr_rvf[] = {0x02, 0x24, 0x46, 0x68, 0x90};
  byte addr_pcie[] = {0x01, 0x23, 0x45, 0x67, 0x89};
  if (!nrf24_device.init(2450, 1000, 0, 5))
  {
    Serial.println("RadioComm init failed on nRF24 init.");
    return;
  }
  nrf24_device.set_transmit_addr(addr_pcie);
  nrf24_device.set_receive_addr(0, addr_rvf);
  Serial.setRxTimeout(10);
  Serial.onReceive(onReceive);
}

void loop()
{
  if (send_flag)
  {
    digitalWrite(37, LOW);
    send_flag = false;
    nrf24_device.send(send_buffer, len);
    digitalWrite(37, HIGH);
  }
  delay(5);
}
