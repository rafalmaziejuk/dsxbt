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

#include "bluetooth_gap.h"
#include "bluetooth_hid_host.h"

#include <bluetooth/bluetooth_service.h>
#include <utils/log.h>

#include <esp_bt_main.h>

DSX_LOG_TAG(BluetoothService);

namespace dsx {

BluetoothService::~BluetoothService() {
    if (m_isHidHostInitialized) {
        DSX_RESULT_CHECK(BluetoothHidHost::deinitialize());
    }
}

Result BluetoothService::initialize(const BluetoothServiceConfig &config) {
    if (!m_isInitialized) {
        auto result = initializeBluetoothStack(config);
        if (result != ESP_OK) {
            return result;
        }

        result = BluetoothGap::initialize(config.gapConfig, config.eventCallback);
        if (result != ESP_OK) {
            return result;
        }

        m_isInitialized = true;

        DSX_LOGI("bluetooth service initialized");
    }

    return DSX_RESULT_SUCCESS();
}

Result BluetoothService::initializeHidHost(const BluetoothHidHostConfig &config) {
    if (!m_isInitialized) {
        return DSX_RESULT_ERROR(ESP_FAIL, "bluetooth service is not initialized");
    }

    if (!m_isHidHostInitialized) {
        auto result = BluetoothHidHost::initialize(config.eventCallback);
        if (result != ESP_OK) {
            return result;
        }

        m_isHidHostInitialized = true;
    }

    return DSX_RESULT_SUCCESS();
}

Result BluetoothService::startDiscovery(esp_bt_inq_mode_t mode, uint8_t duration, uint8_t responsesCount) {
    if (!m_isInitialized) {
        return DSX_RESULT_ERROR(ESP_FAIL, "bluetooth service is not initialized");
    }

    return BluetoothGap::startDiscovery(mode, duration, responsesCount);
}

Result BluetoothService::stopDiscovery() {
    if (!m_isInitialized) {
        return DSX_RESULT_ERROR(ESP_FAIL, "bluetooth service is not initialized");
    }

    return BluetoothGap::stopDiscovery();
}

Result BluetoothService::startRemoteServicesDiscovery(const BluetoothDevice &device) {
    if (!m_isInitialized) {
        return DSX_RESULT_ERROR(ESP_FAIL, "bluetooth service is not initialized");
    }

    return BluetoothGap::startRemoteServicesDiscovery(device);
}

Result BluetoothService::startRemoteServiceRecordDiscovery(const BluetoothDevice &device, esp_bt_uuid_t uuid) {
    if (!m_isInitialized) {
        return DSX_RESULT_ERROR(ESP_FAIL, "bluetooth service is not initialized");
    }

    return BluetoothGap::startRemoteServiceRecordDiscovery(device, uuid);
}

Result BluetoothService::openHidDeviceConnection(const BluetoothDevice &device, esp_hid_transport_t transport, esp_ble_addr_type_t bleAddressType) {
    if (!m_isInitialized) {
        return DSX_RESULT_ERROR(ESP_FAIL, "bluetooth service is not initialized");
    }
    if (!m_isHidHostInitialized) {
        return DSX_RESULT_ERROR(ESP_FAIL, "bluetooth hid host not initialized");
    }

    return BluetoothHidHost::openHidDeviceConnection(device, transport, bleAddressType);
}

Result BluetoothService::initializeBluetoothStack(const BluetoothServiceConfig &config) {
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
