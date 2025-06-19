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

#pragma once

#include <esp_bt.h>
#include <esp_bt_defs.h>
#include <esp_gap_bt_api.h>
#include <esp_hidh.h>

#include <array>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace dsx {

class BluetoothEvent;
using BluetoothEventCallback = std::function<void(BluetoothEvent &event)>;
using BluetoothDeviceAddress = std::array<uint8_t, ESP_BD_ADDR_LEN>;

struct BluetoothGapConfig {
    std::optional<std::string> deviceName;
    esp_bt_connection_mode_t connectionMode;
    esp_bt_discovery_mode_t discoveryMode;
};

struct BluetoothManagerConfig {
    esp_bt_mode_t mode;
    BluetoothGapConfig gapConfig;

    BluetoothEventCallback eventCallback;
};

struct BluetoothHidHostConfig {
    BluetoothEventCallback eventCallback;
};

struct BluetoothDeviceEirData {
    std::string shortLocalName;
    std::string completeLocalName;
};

struct BluetoothDeviceDiscoveryData {
    std::string name;
    std::string addressStr;
    BluetoothDeviceAddress address;
    uint32_t cod;
    uint32_t majorDeviceClass;
    uint32_t minorDeviceClass;
    uint32_t serviceClass;
    int32_t rssi;
    BluetoothDeviceEirData eir;
};

} // namespace dsx
