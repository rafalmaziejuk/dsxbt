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
#include "bluetooth_utils.h"

#include <bluetooth/events/bluetooth_event.h>
#include <bluetooth/bluetooth_types.h>
#include <utils/log.h>

#include <algorithm>
#include <unordered_set>

DSX_LOG_TAG(BluetoothGap);

namespace dsx {

namespace {

BluetoothEventCallback s_eventCallback;

void callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);
void handleDeviceDiscoveryEvent(esp_bt_gap_cb_param_t *param);
void handleDiscoveryStateChangedEvent(esp_bt_gap_cb_param_t *param);

} // namespace

Result BluetoothGap::initialize(const BluetoothGapConfig &config, BluetoothEventCallback eventCallback) {
    assert(!s_eventCallback);

    esp_err_t error{ESP_FAIL};
    if (config.deviceName) {
        const auto &deviceName = *config.deviceName;
        error = esp_bt_gap_set_device_name(deviceName.c_str());
        if (error != ESP_OK) {
            return DSX_RESULT_ERROR(error, "set device name failed");
        }
    }

    error = esp_bt_gap_set_scan_mode(config.connectionMode, config.discoveryMode);
    if (error != ESP_OK) {
        return DSX_RESULT_ERROR(error, "set scan mode failed");
    }

    error = esp_bt_gap_register_callback(callback);
    if (error != ESP_OK) {
        return DSX_RESULT_ERROR(error, "register callback failed");
    }

    s_eventCallback = eventCallback;

    DSX_LOGI("bluetooth gap initialized");

    return DSX_RESULT_SUCCESS();
}

Result BluetoothGap::startDiscovery(esp_bt_inq_mode_t mode, uint8_t duration, uint8_t responsesCount) {
    esp_err_t error = esp_bt_gap_start_discovery(mode, duration, responsesCount);
    if (error != ESP_OK) {
        return DSX_RESULT_ERROR(error, "start discovery failed");
    }

    return DSX_RESULT_SUCCESS();
}

Result BluetoothGap::stopDiscovery() {
    esp_err_t error = esp_bt_gap_cancel_discovery();
    if (error != ESP_OK) {
        return DSX_RESULT_ERROR(error, "stop discovery failed");
    }

    return DSX_RESULT_SUCCESS();
}

namespace {

void callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {
    switch (event) {
    case ESP_BT_GAP_DISC_RES_EVT:
        handleDeviceDiscoveryEvent(param);
        break;

    case ESP_BT_GAP_DISC_STATE_CHANGED_EVT:
        handleDiscoveryStateChangedEvent(param);
        break;

    default:
        DSX_LOGW("bluetooth gap unhandled event: {}", static_cast<uint32_t>(event));
        break;
    }
}

void handleDeviceDiscoveryEvent(esp_bt_gap_cb_param_t *param) {
    static std::unordered_set<std::string> s_discoveredDeviceAddresses;

    auto bluetoothDeviceAddress = parseBluetoothDeviceAddress(param->disc_res.bda);
    if (s_discoveredDeviceAddresses.contains(bluetoothDeviceAddress)) {
        return;
    }
    s_discoveredDeviceAddresses.insert(bluetoothDeviceAddress);

    BluetoothDeviceDiscoveredEvent deviceDiscoveredEvent{};
    for (uint32_t i = 0u; i < param->disc_res.num_prop; i++) {
        const auto &property = *(param->disc_res.prop + i);
        switch (property.type) {
        case ESP_BT_GAP_DEV_PROP_BDNAME:
            if (property.val) {
                deviceDiscoveredEvent.name = parseBluetoothDeviceName(
                    static_cast<uint8_t *>(property.val),
                    static_cast<uint8_t>(property.len));
            }
            break;

        case ESP_BT_GAP_DEV_PROP_COD:
            deviceDiscoveredEvent.cod = *static_cast<uint32_t *>(property.val);
            break;

        case ESP_BT_GAP_DEV_PROP_RSSI:
            deviceDiscoveredEvent.rssi = *static_cast<int8_t *>(property.val);
            break;

        case ESP_BT_GAP_DEV_PROP_EIR:
            if (property.val) {
                deviceDiscoveredEvent.eir = parseBluetoothDeviceEirData(
                    static_cast<uint8_t *>(property.val));
            }
            break;

        default:
            break;
        }
    }

    if (!esp_bt_gap_is_valid_cod(deviceDiscoveredEvent.cod)) {
        DSX_LOGW("invalid COD: 0x{:08X}", deviceDiscoveredEvent.cod);
        return;
    }

    std::copy(param->disc_res.bda,
              param->disc_res.bda + ESP_BD_ADDR_LEN,
              deviceDiscoveredEvent.address);
    deviceDiscoveredEvent.addressStr = bluetoothDeviceAddress;

    BluetoothEvent event{deviceDiscoveredEvent};
    s_eventCallback(event);
}

void handleDiscoveryStateChangedEvent(esp_bt_gap_cb_param_t *param) {
    BluetoothEvent event{BluetoothDiscoveryStateChangedEvent{
        .state = param->disc_st_chg.state,
    }};
    s_eventCallback(event);
}

} // namespace

} // namespace dsx
