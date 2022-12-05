#include "hardware/I2C.h"
#include "component/Motor.h"

#include <utils/Logger.h>

#include <cstring>
#include <cmath>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

extern "C" {
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
}

using namespace component;

namespace hardware {

int I2C::open_pwm_pca9685(const std::string &device, int address, const double freq_hz) {
    if(open(device, address) == -1) {
        logE << "Failed to initialize pca9685 chip: " << std::endl;
        return -1;
    }

    set_pwm(0, 0, ALL_LED_ON_L, ALL_LED_ON_H, ALL_LED_OFF_L, ALL_LED_OFF_H);

    write(MODE2, OUTDRV);
    write(MODE1, ALLCALL);
    usleep(5000);
    auto mode1_val = read(MODE1);
    mode1_val &= ~SLEEP;
    write(MODE1, mode1_val);
    usleep(5000);

    set_pwm_freq(freq_hz);

    return 1;
}

int I2C::open(const std::string& device, const uint8_t address) {
    close();

    m_fd = ::open(device.c_str(), O_RDWR);
    if(m_fd < 0) {
        logE << "Failed to open i2c bus: " << strerror(errno) << std::endl;
        return -1;
    }
    if(ioctl(m_fd, I2C_SLAVE, address) < 0) {
        logE << "Failed to set i2c bus address: " << strerror(errno) << std::endl;
        return -1;
    }
    return 1;
}

void I2C::close() {
    if(m_fd >= 0) {
        ::close(m_fd);
        m_fd = -1;
    }
}

void I2C::write(const uint8_t address, const uint8_t value) {
    i2c_smbus_data data;
    data.byte = value;
    if(i2c_smbus_access(m_fd, I2C_SMBUS_WRITE, address, I2C_SMBUS_BYTE_DATA, &data)) {
        logE << "Failed to write on i2c bus address: " << strerror(errno) << std::endl;
    }
}

uint8_t I2C::read(const uint8_t address) {
    i2c_smbus_data data;
    if(i2c_smbus_access(m_fd, I2C_SMBUS_READ, address, I2C_SMBUS_BYTE_DATA, &data)) {
        logE << "Failed to read the value of i2c bus address: " << strerror(errno) << std::endl;
    }
    return data.byte & 0xFF;
}

void I2C::set_pwm_pca9685(const int channel, const uint16_t on, const uint16_t off) {
    const auto offset = 4 * channel;
    set_pwm(on, off,
        LED0_ON_L  + offset,
        LED0_ON_H  + offset,
        LED0_OFF_L + offset,
        LED0_OFF_H + offset
    );
}

void I2C::set_pwm(const uint16_t on, const uint16_t off, const uint8_t on_low, 
    const uint8_t on_high, const uint8_t off_low, const uint8_t off_high) {
    write(on_low, on & 0xFF);
    write(on_high, on >> 8);
    write(off_low, off & 0xFF);
    write(off_high, off >> 8);
}

void I2C::set_pwm_freq(const double freq_hz) {
  auto prescaleval = 2.5e7; // 25MHz
  prescaleval /= 4096.0;    // 12-bit
  prescaleval /= freq_hz;
  prescaleval -= 1.0;

  auto prescale = static_cast<int>(std::round(prescaleval));

  const auto oldmode = read(MODE1);

  auto newmode = (oldmode & 0x7F) | SLEEP;

  write(MODE1, newmode);
  write(PRESCALE, prescale);
  write(MODE1, oldmode);
  usleep(5000);
  write(MODE1, oldmode | RESTART);
}

}  // hardware