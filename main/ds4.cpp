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

#include <bluetooth/events/bluetooth_event.h>
#include <utils/log.h>

#include <nvs.h>
#include <nvs_flash.h>

DSX_LOG_TAG(DualShock4);

namespace dsx {

DualShock4::DualShock4(BluetoothManager &bluetoothManager)
    : BluetoothDevice{bluetoothManager} {
    esp_err_t error = nvs_flash_init();
    if (error == ESP_ERR_NVS_NO_FREE_PAGES ||
        error == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        DSX_RESULT_CHECK(nvs_flash_erase());
        DSX_RESULT_CHECK(nvs_flash_init());
    }

    auto onBluetoothEventCallback = [this](BluetoothEvent &event) {
        DualShock4::onEvent(event);
    };

    BluetoothManagerConfig bluetoothManagerConfig{
        .mode = ESP_BT_MODE_CLASSIC_BT,
        .gapConfig = BluetoothGapConfig{
            .connectionMode = ESP_BT_CONNECTABLE,
            .discoveryMode = ESP_BT_NON_DISCOVERABLE,
        },
        .eventCallback = onBluetoothEventCallback,
    };
    DSX_RESULT_CHECK(bluetoothManager.initialize(bluetoothManagerConfig));

    BluetoothHidHostConfig bluetoothHidHostConfig{
        .eventCallback = onBluetoothEventCallback,
    };
    DSX_RESULT_CHECK(bluetoothManager.initializeHidHost(bluetoothHidHostConfig));
}

DualShock4::~DualShock4() {
}

void DualShock4::onEvent(BluetoothEvent &bluetoothEvent) {
    if (const auto event = bluetoothEvent.get<BluetoothDeviceDiscoveredEvent>()) {
        onDeviceDiscoveredEvent(*event);
    }

    if (const auto event = bluetoothEvent.get<BluetoothDiscoveryStateChangedEvent>()) {
        if (event->state == ESP_BT_GAP_DISCOVERY_STARTED) {
            DSX_LOGI("bluetooth device discovery started");
        } else if (event->state == ESP_BT_GAP_DISCOVERY_STOPPED) {
            DSX_LOGI("bluetooth device discovery stopped");
        }
    }

    if (const auto event = bluetoothEvent.get<BluetoothRemoteServiceRecordDiscoveredEvent>()) {
        onRemoteServiceRecordDiscoveredEvent(*event);
    }

    if (const auto event = bluetoothEvent.get<BluetoothHidDeviceOpenEvent>()) {
        onHidDeviceOpenedEvent(*event);
    }
}

void DualShock4::onDeviceDiscoveredEvent(const BluetoothDeviceDiscoveredEvent &event) {
    auto found = true;
    found &= (event.data.serviceClass == ESP_BT_COD_SRVC_LMTD_DISCOVER);
    found &= (event.data.majorDeviceClass == ESP_BT_COD_MAJOR_DEV_PERIPHERAL);
    found &= (event.data.minorDeviceClass == ESP_BT_COD_MINOR_PERIPHERAL_GAMEPAD);

    if (found && (m_state == BluetoothDeviceState::k_unknown)) {
        DSX_RESULT_CHECK(m_rBluetoothManager.stopDiscovery());
        DSX_LOGI("bluetooth device discovered: {}", event.data.addressStr);

        m_state = BluetoothDeviceState::k_discovered;
        m_address = event.data.address;

        esp_bt_uuid_t uuid{
            .len = ESP_UUID_LEN_16,
            .uuid = {
                .uuid16 = 0x1124u,
            },
        };
        DSX_RESULT_CHECK(m_rBluetoothManager.startRemoteServiceRecordDiscovery(m_address, uuid));
    }
}

void DualShock4::onRemoteServiceRecordDiscoveredEvent(const BluetoothRemoteServiceRecordDiscoveredEvent &event) {
    if (event.discovered) {
        DSX_LOGI("dualshock 4 controller discovered successfully");

        DSX_LOGI("opening connection with dualshock 4 controller");
        DSX_RESULT_CHECK(m_rBluetoothManager.openHidDeviceConnection(m_address, ESP_HID_TRANSPORT_BT, BLE_ADDR_TYPE_PUBLIC));
    } else {
        DSX_LOGE("unable to discover remote hid service for discovered device");
        m_state = BluetoothDeviceState::k_unknown;
    }
}

void DualShock4::onHidDeviceOpenedEvent(const BluetoothHidDeviceOpenEvent &event) {
    if (event.status == ESP_OK) {
        DSX_LOGI("connection with dualshock 4 controller opened successfully");

        if (m_state != BluetoothDeviceState::k_discovered) {
            DSX_RESULT_CHECK(m_rBluetoothManager.getHidDeviceBluetoothAddress(event.deviceData, m_address));
        }
        m_state = BluetoothDeviceState::k_connected;
    } else {
        DSX_LOGE("unable to open connection with dualshock 4 controller");
        m_state = BluetoothDeviceState::k_unknown;
    }
}

} // namespace dsx
