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
 * @file speedometer_device.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Speedometer implementation
 *
 * @date 2025-07-01
 * @version 1.0.0
 ***************************************************************************/

#include "speedometer.hpp"

// zephyr

// std
#include <chrono>

// zpp_lib
#include "zpp_include/time.hpp"
#include "zpp_include/zpp_assert.hpp"
#include "zpp_include/zpp_log.hpp"

ZPP_LOG_MODULE_DECLARE(bike_computer, CONFIG_APP_LOG_LEVEL);

namespace bike_computer {

static constexpr float kMetersPerKilometer  = 1000.0F;
static constexpr float kMillisecondsPerHour = 3600.0F * 1000.0F;

Speedometer::Speedometer() : _last_time(zpp_lib::Time::get_uptime()) {}

void Speedometer::set_current_pedal_rotation_time(const std::chrono::milliseconds& current_rotation_time) {
  if (_pedal_rotation_time != current_rotation_time) {
    // compute distance before changing the rotation time
    compute_traveled_distance();

    // change pedal rotation time
    _pedal_rotation_time = current_rotation_time;

    // compute speed with the new pedal rotation time
    compute_speed();
  }
}

void Speedometer::set_gear_size(uint8_t gear_size) {
  if (_gear_size != gear_size) {
    // compute distance before changing the gear size
    compute_traveled_distance();

    // change gear size
    _gear_size = gear_size;

    // compute speed with the new gear size
    compute_speed();
  }
}

float Speedometer::get_current_speed() const {
  return _current_speed;
}

float Speedometer::get_traveled_distance() {
  // make sure to update the distance traveled
  return compute_traveled_distance();
}

void Speedometer::reset() {
#if CONFIG_TEST
  if (_cb != nullptr) {
    _cb();
  }
#endif  // CONFIG_TEST
  auto res = _total_distance_mutex.lock();
  if (!res) {
    ZPP_LOG_ERR("Cannot lock mutex: %d", static_cast<int>(res.error()));
    return;
  }
  _total_distance = 0.0F;
  _last_time      = zpp_lib::Time::get_uptime();
  res             = _total_distance_mutex.unlock();
  if (!res) {
    ZPP_LOG_ERR("Cannot unlock mutex: %d", static_cast<int>(res.error()));
  }
}

void Speedometer::compute_speed() {
  // For computing the speed given a rear gear (braquet), one must divide the size of
  // the tray (plateau) by the size of the rear gear (pignon arrière), and then multiply
  // the result by the circumference of the wheel. Example: tray = 50, rear gear = 15.
  // Distance run with one pedal turn (wheel circumference = 2.10 m) = 50/15 * 2.1 m
  // = 6.99m If you ride at 80 pedal turns / min, you run a distance of 6.99 * 80 / min
  // ~= 560 m / min = 33.6 km/h
  const float tray_gear_ratio          = static_cast<float>(kTraySize) / static_cast<float>(_gear_size);
  const float distance_per_pedal_turn  = tray_gear_ratio * kWheelCircumference;
  const float pedal_rotations_per_hour = kMillisecondsPerHour / static_cast<float>(_pedal_rotation_time.count());
  _current_speed                       = (distance_per_pedal_turn * pedal_rotations_per_hour) / kMetersPerKilometer;
}

float Speedometer::compute_traveled_distance() {
  // For computing the speed given a rear gear (braquet), one must divide the size of
  // the tray (plateau) by the size of the rear gear (pignon arrière), and then multiply
  // the result by the circumference of the wheel. Example: tray = 50, rear gear = 15.
  // Distance run with one pedal turn (wheel circumference = 2.10 m) = 50/15 * 2.1 m
  // = 6.99m If you ride at 80 pedal turns / min, you run a distance of 6.99 * 80 / min
  // ~= 560 m / min = 33.6 km/h. We then multiply the speed by the time for getting the
  // distance traveled.
  auto res = _total_distance_mutex.lock();
  if (!res) {
    ZPP_LOG_ERR("Cannot lock mutex: %d", static_cast<int>(res.error()));
    return _total_distance;
  }

  const std::chrono::microseconds current_time  = zpp_lib::Time::get_uptime();
  const std::chrono::microseconds elapsed_time  = current_time - _last_time;
  const std::chrono::microseconds rotation_time = _pedal_rotation_time;

  const float pedal_rotations         = static_cast<float>(elapsed_time.count()) / static_cast<float>(rotation_time.count());
  const float tray_gear_ratio         = static_cast<float>(kTraySize) / static_cast<float>(_gear_size);
  const float distance_per_pedal_turn = tray_gear_ratio * kWheelCircumference;

  _total_distance += (distance_per_pedal_turn * pedal_rotations) / kMetersPerKilometer;
  _last_time = current_time;

  const float total_distance = _total_distance;
  res                        = _total_distance_mutex.unlock();
  if (!res) {
    ZPP_LOG_ERR("Cannot unlock mutex: %d", static_cast<int>(res.error()));
  }
  return total_distance;
}

}  // namespace bike_computer
