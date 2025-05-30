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

#include "bluetooth_utils.h"

#include <format>

namespace dsx {

std::string parseBluetoothDeviceAddress(const uint8_t *data) {
    if (data == nullptr) {
        return "";
    }

    constexpr std::string_view k_bluetoothAddressFormat = "{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}";
    return std::format(
        k_bluetoothAddressFormat,
        data[0], data[1], data[2],
        data[3], data[4], data[5]);
}

std::string parseBluetoothDeviceName(const uint8_t *data, uint8_t length) {
    return {reinterpret_cast<const char *>(data), length};
}

BluetoothDeviceEirData parseBluetoothDeviceEirData(uint8_t *property) {
    BluetoothDeviceEirData eirData{};

    uint8_t length = 0u;
    uint8_t *data = esp_bt_gap_resolve_eir_data(property, ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME, &length);
    if (data != nullptr || length > 0u) {
        eirData.shortLocalName = parseBluetoothDeviceName(data, length);
    }

    data = esp_bt_gap_resolve_eir_data(property, ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME, &length);
    if (data != nullptr || length > 0u) {
        eirData.completeLocalName = parseBluetoothDeviceName(data, length);
    }

    return eirData;
}

std::string parseBluetoothDeviceRemoteService(const esp_bt_uuid_t *uuid) {
    std::string result = "";
    if (uuid == nullptr) {
        return result;
    }

    if (uuid->len == 2u) {
        constexpr std::string_view k_format = "{:04X}";
        result = std::format(k_format, uuid->uuid.uuid16);
    } else if (uuid->len == 4u) {
        constexpr std::string_view k_format = "{:08X}";
        result = std::format(k_format, uuid->uuid.uuid32);
    } else if (uuid->len == 16u) {
        constexpr std::string_view k_format = "{:02X}{:02X}{:02X}{:02X}-{:02X}{:02X}-{:02X}{:02X}-{:02X}{:02X}-{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}";
        const auto *p = uuid->uuid.uuid128;
        result = std::format(
            k_format,
            p[15], p[14], p[13], p[12], p[11], p[10], p[9], p[8],
            p[7], p[6], p[5], p[4], p[3], p[2], p[1], p[0]);
    }

    return result;
}

} // namespace dsx
