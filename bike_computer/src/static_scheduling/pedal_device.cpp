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
 * @brief PedalDevice implementation (static scheduling)
 *
 * @date 2025-07-01
 * @version 1.0.0
 ***************************************************************************/

#include "pedal_device.hpp"

// from common
#include "common/task_manager.hpp"

// zpp_lib
#include "zpp_include/time.hpp"

namespace bike_computer::static_scheduling {

PedalDevice::PedalDevice()
    : _button2(zpp_lib::InterruptIn::PinName::BUTTON2), _button3(zpp_lib::InterruptIn::PinName::BUTTON3),
      _button4(zpp_lib::InterruptIn::PinName::BUTTON4) {}

std::chrono::milliseconds PedalDevice::get_current_rotation_time() {
  std::chrono::microseconds initial_time = zpp_lib::Time::get_uptime();
  std::chrono::microseconds elapsed_time = std::chrono::microseconds::zero();

  // we bound the change to one decrement/increment per call
  // we decrement/increment rotation speed when button3/button4 is pressed
  // while button2 is not pressed
  bool has_changed = false;
  while (elapsed_time < TaskManager::get_task_computation_time(TaskManager::TaskType::SpeedTaskType)) {
    if (!has_changed) {
      if (_button2.read() != zpp_lib::kPolarityPressed) {
        if (_button3.read() == zpp_lib::kPolarityPressed) {
          decrease_rotation_speed();
          has_changed = true;
        }

        if (_button4.read() == zpp_lib::kPolarityPressed) {
          increase_rotation_speed();
          has_changed = true;
        }
      }
    }
    elapsed_time = zpp_lib::Time::get_uptime() - initial_time;
  }
  return _pedal_rotation_time;
}

void PedalDevice::increase_rotation_speed() {
  if (_pedal_rotation_time > bike_computer::kMinPedalRotationTime) {
    _pedal_rotation_time -= bike_computer::kDeltaPedalRotationTime;
  }
}

void PedalDevice::decrease_rotation_speed() {
  if (_pedal_rotation_time < bike_computer::kMaxPedalRotationTime) {
    _pedal_rotation_time += bike_computer::kDeltaPedalRotationTime;
  }
}

}  // namespace bike_computer::static_scheduling
