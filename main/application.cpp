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

#include "application.h"

#include <bluetooth/events/bluetooth_event.h>
#include <utils/log.h>
#include <utils/result.h>

#include <nvs.h>
#include <nvs_flash.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

DSX_LOG_TAG(Application);

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
        .gapConfig = BluetoothGapConfig{
            .connectionMode = ESP_BT_CONNECTABLE,
            .discoveryMode = ESP_BT_NON_DISCOVERABLE,
        },
        .eventCallback = [this](BluetoothEvent &event) {
            onBluetoothEvent(event);
        },
    };
    DSX_RESULT_CHECK(m_bluetoothService.initialize(bluetoothServiceConfig));
}

Application::~Application() {}

void Application::run() {
    DSX_RESULT_CHECK(m_bluetoothService.startDiscovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY));

    while (true) {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}

void Application::onBluetoothEvent(BluetoothEvent &bluetoothEvent) {
    if (const auto event = bluetoothEvent.get<BluetoothDiscoveryStateChangedEvent>()) {
        if (event->state == ESP_BT_GAP_DISCOVERY_STARTED) {
            DSX_LOGI("bluetooth discovery started");
        } else if (event->state == ESP_BT_GAP_DISCOVERY_STOPPED) {
            DSX_LOGI("bluetooth discovery stopped");
        }
    }

    if (const auto event = bluetoothEvent.get<BluetoothDeviceDiscoveredEvent>()) {
        DSX_LOGI("device discovered");
    }
}

} // namespace dsx
