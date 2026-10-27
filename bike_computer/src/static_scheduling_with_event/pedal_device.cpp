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
 * @file pedal_device.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief PedalDevice implementation (static scheduling with event)
 *
 * @date 2025-07-01
 * @version 1.0.0
 ***************************************************************************/

#include "pedal_device.hpp"

namespace bike_computer::static_scheduling_with_event {

PedalDevice::PedalDevice()
    : _button2(zpp_lib::InterruptIn::PinName::BUTTON2),
      _button3(zpp_lib::InterruptIn::PinName::BUTTON3),
      _button4(zpp_lib::InterruptIn::PinName::BUTTON4),
      _button3_token(_button3.add_callback([this]() { on_fall_button3(); })),
      _button4_token(_button4.add_callback([this]() { on_fall_button4(); })) {}

std::chrono::milliseconds PedalDevice::get_current_rotation_time() {
  return std::chrono::milliseconds(_pedal_rotation_time_ms.load(std::memory_order_acquire));
}

void PedalDevice::on_fall_button3() {
  if (_button2.read() != zpp_lib::kPolarityPressed) {
    decrease_rotation_speed();
  }
}

void PedalDevice::on_fall_button4() {
  if (_button2.read() != zpp_lib::kPolarityPressed) {
    increase_rotation_speed();
  }
}

void PedalDevice::increase_rotation_speed() {
  uint32_t time_ms = _pedal_rotation_time_ms.load(std::memory_order_relaxed);
  if (time_ms > static_cast<uint32_t>(bike_computer::kMinPedalRotationTime.count())) {
    _pedal_rotation_time_ms.store(time_ms - static_cast<uint32_t>(bike_computer::kDeltaPedalRotationTime.count()),
                                  std::memory_order_release);
  }
}

void PedalDevice::decrease_rotation_speed() {
  uint32_t time_ms = _pedal_rotation_time_ms.load(std::memory_order_relaxed);
  if (time_ms < static_cast<uint32_t>(bike_computer::kMaxPedalRotationTime.count())) {
    _pedal_rotation_time_ms.store(time_ms + static_cast<uint32_t>(bike_computer::kDeltaPedalRotationTime.count()),
                                  std::memory_order_release);
  }
}

}  // namespace bike_computer::static_scheduling_with_event