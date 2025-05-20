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

#include "application/application.h"

#include <utils/result.h>

#include <nvs.h>
#include <nvs_flash.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace dsx {

Application::Application() {
    esp_err_t error = nvs_flash_init();
    if (error == ESP_ERR_NVS_NO_FREE_PAGES ||
        error == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        DSX_RESULT_CHECK(nvs_flash_erase());
        DSX_RESULT_CHECK(nvs_flash_init());
    }

    BluetoothServiceConfig bluetoothServiceConfig{
        .mode = ESP_BT_MODE_CLASSIC_BT,
    };
    DSX_RESULT_CHECK(m_bluetoothService.initialize(bluetoothServiceConfig));
}

Application::~Application() {}

void Application::run() {
    while (true) {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}

} // namespace dsx
