#include "nrf24_device.h"
#include "main.h"
nRF24Device::nRF24Device(uint8_t spi_bus, int8_t sck, int8_t miso, int8_t mosi, int8_t ss, uint32_t irq, uint32_t rst)
{
    __radio_spi = new SPIClass(spi_bus);
    __radio_spi->begin(sck, miso, mosi, ss);
    __radio = new nRF24{new Module{static_cast<uint32_t>(ss), irq, rst, RADIOLIB_NC, *__radio_spi, __spi_setting}};
}

nRF24Device::~nRF24Device()
{
    if (__radio)
    {
        delete __radio;
    }
    if (__radio_spi)
    {
        delete __radio_spi;
    }
}

bool nRF24Device::init(int16_t freq, int16_t dr, int8_t pwr, uint8_t addrWidth)
{
    Serial.println("nRF24 device init");
    auto status = __radio->begin(freq, dr, pwr, addrWidth);
    if (status == RADIOLIB_ERR_NONE)
    {
        __radio->setBitRate(dr);
        __radio->setCrcFiltering(false);
        __radio->setAutoAck(false);
        Serial.println("nRF24 device init success");
        return true;
    }
    Serial.print("nRF24 device init failed. error code: ");
    Serial.println(status);
    return false;
}

bool nRF24Device::set_transmit_addr(uint8_t *addr)
{
    auto status{__radio->setTransmitPipe(addr)};
    Serial.print("set transmit addr: ");
    for (int i = 0; i < 5; i++)
    {
        Serial.print(addr[i], HEX);
        Serial.print(" ");
    }
    Serial.print("status: ");
    Serial.println(status);
    return RADIOLIB_ERR_NONE == status;
}

bool nRF24Device::set_receive_addr(uint8_t pipe_num, uint8_t *addr)
{
    auto status{__radio->setReceivePipe(pipe_num, addr)};
    Serial.print("set receive addr and pipe: ");
    for (int i = 0; i < 5; i++)
    {
        Serial.print(addr[i], HEX);
        Serial.print(" ");
    }
    Serial.print("status: ");
    Serial.println(status);
    return RADIOLIB_ERR_NONE == status;
}

bool nRF24Device::send(uint8_t *message, size_t size)
{
    auto status{__radio->transmit(message, size, 0)};
    if (status == RADIOLIB_ERR_ACK_NOT_RECEIVED)
    {
        // static_cast<nRF24 *>(__radio)->clearIRQ();
        ESP.restart(); // 重启ESP32
    }
    return RADIOLIB_ERR_NONE == status;
}

bool nRF24Device::recv(uint8_t *buffer, size_t &size)
{
    size = __radio->getPacketLength(); // 最大数据包长度
    int status = __radio->receive(buffer, size);
    return RADIOLIB_ERR_NONE == status;
}

int32_t nRF24Device::set_frequency(uint32_t frequency)
{
    return __radio->setFrequency(frequency / 1000000);
}

uint8_t nRF24Device::set_power(uint8_t power)
{
    return 0;
}

uint32_t nRF24Device::set_data_rate(uint32_t rate)
{
    // return __radio->setDataRate(rate);
    return 0;
}

uint8_t nRF24Device::set_addr_width(uint8_t addr_width)
{
    return 0;
}

bool nRF24Device::shutdown()
{
    return RADIOLIB_ERR_NONE == __radio->sleep();
}

bool nRF24Device::reboot()
{
    return true;
}
