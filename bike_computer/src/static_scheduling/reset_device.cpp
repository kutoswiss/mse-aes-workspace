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
 * @file reset_device.cpp
 * @author Christopher Rejas <ianchris.rejas@master.hes-so.ch>
 * @author Simon Mirkovitch <simon.mirkovitch@master.hes-so.ch>
 * @author Thierno Mane Mame <mame.mane@master.hes-so.ch>
 * @author Faycal Atoub <faycal.atoub@master.hes-so.ch>
 *
 * @brief ResetDevice implementation (static scheduling)
 *
 * @date 2026-10-30
 * @version 1.0.0
 ***************************************************************************/

#include "reset_device.hpp"

// from common
#include "common/task_manager.hpp"

// zpp_lib
#include "zpp_include/time.hpp"

namespace bike_computer::static_scheduling {

ResetDevice::ResetDevice()
    : _button1(zpp_lib::InterruptIn::PinName::BUTTON1), _button1_token(_button1.add_callback([this]() { on_fall_button1(); })) {}

bool ResetDevice::check_reset() {
  std::chrono::microseconds initial_time = zpp_lib::Time::get_uptime();
  std::chrono::microseconds elapsed_time = std::chrono::microseconds::zero();

  bool is_pressed = false;
  while (elapsed_time < TaskManager::get_task_computation_time(TaskManager::TaskType::ResetTaskType)) {
    if (!is_pressed) {
      is_pressed = (_button1.read() == zpp_lib::kPolarityPressed);
    }
    elapsed_time = zpp_lib::Time::get_uptime() - initial_time;
  }

  return is_pressed;
}

std::chrono::microseconds ResetDevice::get_press_time() {
  return _press_time;
}

void ResetDevice::on_fall_button1() {
  _press_time = zpp_lib::Time::get_uptime();
}

}  // namespace bike_computer::static_scheduling
