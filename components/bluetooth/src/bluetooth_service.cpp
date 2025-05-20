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

#include <bluetooth/bluetooth_service.h>

#include <utils/log.h>

#include <esp_bt_main.h>

DSX_LOG_TAG(BluetoothService);

namespace dsx {

struct BluetoothService::Impl {
    BluetoothServiceConfig config{};
};

BluetoothService::BluetoothService()
    : m_impl{new Impl{}} {}

BluetoothService::~BluetoothService() {
    delete m_impl;
    m_impl = nullptr;
}

Result BluetoothService::initialize(const BluetoothServiceConfig &config) {
    m_impl->config = config;

    auto result = initializeBluetoothStack();
    if (result != ESP_OK) {
        return result;
    }

    return DSX_RESULT_SUCCESS();
}

Result BluetoothService::initializeBluetoothStack() {
    const auto &config = m_impl->config;

    esp_bt_controller_config_t bluetoothConfig = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_err_t error = esp_bt_controller_init(&bluetoothConfig);
    if (error != ESP_OK) {
        return DSX_RESULT_ERROR(error, "initialize bluetooth controller failed");
    }

    error = esp_bt_controller_enable(config.mode);
    if (error != ESP_OK) {
        return DSX_RESULT_ERROR(error, "enable classic bluetooth mode controller failed");
    }

    esp_bluedroid_config_t bluedroidConfig = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
    error = esp_bluedroid_init_with_cfg(&bluedroidConfig);
    if (error != ESP_OK) {
        return DSX_RESULT_ERROR(error, "initialize bluedroid failed");
    }

    error = esp_bluedroid_enable();
    if (error != ESP_OK) {
        return DSX_RESULT_ERROR(error, "enable bluedroid failed");
    }

    DSX_LOGI("bluetooth stack initialized");

    return DSX_RESULT_SUCCESS();
}

} // namespace dsx
