// Copyright 2025 Haute école d'ingénierie et d'architecture de Fribourg
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/****************************************************************************
 * @file sensor_device.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief SensorDevice implementation
 *
 * @date 2025-07-01
 * @version 1.0.0
 ***************************************************************************/

#include "sensor_device.hpp"

// zephyr
#if !CONFIG_EMUL
#include <zephyr/drivers/sensor.h>
#endif  // !CONFIG_EMUL

namespace bike_computer {

zpp_lib::ZephyrResult SensorDevice::initialize() {
  zpp_lib::ZephyrResult res;
#if !CONFIG_EMUL
  // NOLINTNEXTLINE(cppcoreguidelines-init-variables)
  _sensor_device = DEVICE_DT_GET(DT_INST(0, bosch_bme280));
  if (!device_is_ready(_sensor_device)) {
    res.assign_error(zpp_lib::ZephyrErrorCode::Nodev);
  }
#endif  // !CONFIG_EMUL
  return res;
}

zpp_lib::ZephyrResult SensorDevice::read_temperature(float& temperature) {
  zpp_lib::ZephyrResult res;
#if !CONFIG_EMUL
  struct sensor_value temperature_sv = {};
  if (sensor_sample_fetch(_sensor_device) != 0 || sensor_channel_get(_sensor_device, SENSOR_CHAN_AMBIENT_TEMP, &temperature_sv) != 0) {
    res.assign_error(zpp_lib::ZephyrErrorCode::Io);
    return res;
  }
  temperature = sensor_value_to_float(&temperature_sv);
#else
  // Emulated sensor values
  static constexpr float kEmulatedTemperature = 25.0F;
  temperature                                 = kEmulatedTemperature;
#endif  // !CONFIG_EMUL
  return res;
}

zpp_lib::ZephyrResult SensorDevice::read_humidity(float& humidity) {
  zpp_lib::ZephyrResult res;
#if !CONFIG_EMUL
  struct sensor_value humidity_sv = {};
  if (sensor_sample_fetch(_sensor_device) != 0 || sensor_channel_get(_sensor_device, SENSOR_CHAN_HUMIDITY, &humidity_sv) != 0) {
    res.assign_error(zpp_lib::ZephyrErrorCode::Io);
    return res;
  }
  humidity = sensor_value_to_float(&humidity_sv);
#else
  // Emulated sensor values
  static constexpr float kEmulatedHumidity = 50.0F;
  humidity                                 = kEmulatedHumidity;
#endif  // !CONFIG_EMUL
  return res;
}

}  // namespace bike_computer
