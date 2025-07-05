// Copyright 2025 Rafal Maziejuk
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

#include "ds4.h"

#include <gpio/gpio_manager.h>

DSX_LOG_TAG(Main);

extern "C" void app_main() {
    dsx::GpioManager gpioManager{};
    DSX_RESULT_CHECK(gpioManager.installInterruptService());

    gpio_config_t gpioConfig = {
        .pin_bit_mask = (1ULL << GPIO_NUM_16),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    DSX_RESULT_CHECK(gpioManager.configureGpio(gpioConfig));

    dsx::BluetoothManager bluetoothManager{};
    dsx::DualShock4 controller{bluetoothManager};

    DSX_RESULT_CHECK(gpioManager.addInterruptHandler({GPIO_NUM_16}, [&](const gpio_num_t &gpioPin) {
        DSX_RESULT_CHECK(controller.discover());
    }));

    while (true) {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}
