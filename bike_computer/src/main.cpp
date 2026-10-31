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
 * @file main.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Main function of the bike_computer program
 *
 * @date 2025-07-01
 * @version 1.0.0
 ***************************************************************************/

// zpp_lib
#include "zpp_include/zpp_log.hpp"

// bike_computer
#if CONFIG_BIKE_COMPUTER_STATIC_SCHEDULING
#include "static_scheduling/bike_system.hpp"
#endif  // CONFIG_BIKE_COMPUTER_STATIC_SCHEDULING

ZPP_LOG_MODULE_REGISTER(bike_computer, CONFIG_APP_LOG_LEVEL);

int main() {
#if CONFIG_BIKE_COMPUTER_STATIC_SCHEDULING
  bike_computer::static_scheduling::BikeSystem bike_system;

  auto res = bike_system.start();
  if (!res) {
    ZPP_LOG_ERR("Cannot start bike system: %d", static_cast<int>(res.error()));
    return -1;
  }
#endif  // CONFIG_BIKE_COMPUTER_STATIC_SCHEDULING

  return 0;
}
