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
 * @file gear_device.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief GearDevice header file (static scheduling with event)
 *
 * @date 2025-07-01
 * @version 1.0.0
 ***************************************************************************/

#pragma once

// std
#include <atomic>

// local
#include "common/constants.hpp"

// zpp_lib
#include "zpp_include/interrupt_in.hpp"
#include "zpp_include/non_copyable.hpp"
#include "zpp_include/registration_token.hpp"

namespace bike_computer::static_scheduling_with_event {

class GearDevice : private zpp_lib::NonCopyable {
public:
  GearDevice();

  // method called for updating the bike system
  [[nodiscard]] uint8_t get_current_gear();
  [[nodiscard]] uint8_t get_current_gear_size() const;

private:
  // called when button3 or button4 is pressed
  void on_fall_button3();
  void on_fall_button4();

  // data members
  std::atomic<uint8_t> _current_gear{bike_computer::kMinGear};

  // buttons
  zpp_lib::InterruptIn _button2;
  zpp_lib::InterruptIn _button3;
  zpp_lib::InterruptIn _button4;
  zpp_lib::RegistrationToken _button3_token;
  zpp_lib::RegistrationToken _button4_token;
};

}  // namespace bike_computer::static_scheduling_with_event
