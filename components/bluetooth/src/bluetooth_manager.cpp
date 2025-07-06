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

#include <bluetooth/bluetooth_manager.h>

#include <esp_bt_main.h>

DSX_LOG_TAG(BluetoothManager);

namespace dsx {

BluetoothManager::~BluetoothManager() {
    if (m_isHidHostInitialized) {
        DSX_RESULT_CHECK(BluetoothHidHost::deinitialize());
    }
}

Result BluetoothManager::initialize(const BluetoothManagerConfig &config) {
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

        DSX_LOGD("bluetooth manager initialized");
    }

    return DSX_RESULT_SUCCESS();
}

Result BluetoothManager::initializeHidHost(const BluetoothHidHostConfig &config) {
    assert(m_isInitialized);

    if (!m_isHidHostInitialized) {
        auto result = BluetoothHidHost::initialize(config.eventCallback);
        if (result != ESP_OK) {
            return result;
        }

        m_isHidHostInitialized = true;
    }

    return DSX_RESULT_SUCCESS();
}

Result BluetoothManager::startDiscovery(esp_bt_inq_mode_t mode, uint8_t duration, uint8_t responsesCount) {
    assert(m_isInitialized);

    return BluetoothGap::startDiscovery(mode, duration, responsesCount);
}

Result BluetoothManager::stopDiscovery() {
    assert(m_isInitialized);

    return BluetoothGap::stopDiscovery();
}

Result BluetoothManager::startRemoteServicesDiscovery(BluetoothDeviceAddress &address) {
    assert(m_isInitialized);

    return BluetoothGap::startRemoteServicesDiscovery(address);
}

Result BluetoothManager::startRemoteServiceRecordDiscovery(BluetoothDeviceAddress &address, esp_bt_uuid_t uuid) {
    assert(m_isInitialized);

    return BluetoothGap::startRemoteServiceRecordDiscovery(address, uuid);
}

Result BluetoothManager::connectHidHostDevice(BluetoothDeviceAddress &address) {
    assert(m_isInitialized && m_isHidHostInitialized);

    return BluetoothHidHost::connectDevice(address);
}

Result BluetoothManager::disconnectHidHostDevice(BluetoothDeviceAddress &address) {
    assert(m_isInitialized && m_isHidHostInitialized);

    return BluetoothHidHost::disconnectDevice(address);
}

Result BluetoothManager::getHidHostDeviceReport(BluetoothDeviceAddress &address, esp_hidh_report_type_t reportType, uint8_t reportId, size_t reportSize) {
    assert(m_isInitialized && m_isHidHostInitialized);

    return BluetoothHidHost::getDeviceReport(address, reportType, reportId, reportSize);
}

Result BluetoothManager::initializeBluetoothStack(const BluetoothManagerConfig &config) {
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

    DSX_LOGD("bluetooth stack initialized");

    return DSX_RESULT_SUCCESS();
}

} // namespace dsx
