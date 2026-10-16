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
 * @author Christopher Rejas <crejas@pm.me>
 *
 * @brief SensorDevice source file
 *
 * @date 2026-10-13
 * @version 1.0.0
 ***************************************************************************/

// zephyr
#if !CONFIG_EMUL
#include <zephyr/drivers/sensor.h>
#endif  // !CONFIG_EMUL

// sensor_device
#include "sensor_device.hpp"

namespace bike_computer {

zpp_lib::ZephyrResult SensorDevice::initialize() {
  _sensor_device = DEVICE_DT_GET(DT_INST(0, bosch_bme280));

  if (!device_is_ready(_sensor_device)) {
    return zpp_lib::ZephyrResult(zpp_lib::ZephyrErrorCode::Busy);
  }

  return {};
}

zpp_lib::ZephyrResult SensorDevice::read_temperature(float& temperature) {
  static struct sensor_value s_temperature_sv = {};
  sensor_sample_fetch(_sensor_device);
  sensor_channel_get(_sensor_device, SENSOR_CHAN_AMBIENT_TEMP, &s_temperature_sv);
  temperature = sensor_value_to_float(&s_temperature_sv);
  return {};
}

zpp_lib::ZephyrResult SensorDevice::read_humidity(float& humidity) {
  static struct sensor_value s_humidity_sv = {};
  sensor_sample_fetch(_sensor_device);
  sensor_channel_get(_sensor_device, SENSOR_CHAN_HUMIDITY, &s_humidity_sv);
  humidity = sensor_value_to_float(&s_humidity_sv);
  return {};
}

}  // namespace bike_computer
