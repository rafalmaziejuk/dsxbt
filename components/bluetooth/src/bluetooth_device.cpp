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

#include <bluetooth/bluetooth_device.h>

namespace dsx {
BluetoothDevice::BluetoothDevice(const BluetoothDeviceConfig &config)
    : m_config{config} {}

std::string BluetoothDevice::getName() const {
    return m_config.name;
}

std::string BluetoothDevice::getShortLocalName() const {
    return m_config.eir.shortLocalName;
}

std::string BluetoothDevice::getCompleteLocalName() const {
    return m_config.eir.completeLocalName;
}

uint8_t *BluetoothDevice::getAddress() const {
    return const_cast<uint8_t *>(m_config.address);
}

std::string BluetoothDevice::getAddressStr() const {
    return parseBluetoothDeviceAddress(m_config.address);
}

uint32_t BluetoothDevice::getClassOfDevice() const {
    return m_config.cod;
}

uint32_t BluetoothDevice::getMajorDeviceClass() const {
    return esp_bt_gap_get_cod_major_dev(m_config.cod);
}

uint32_t BluetoothDevice::getMinorDeviceClass() const {
    return esp_bt_gap_get_cod_minor_dev(m_config.cod);
}

uint32_t BluetoothDevice::getServiceClass() const {
    return esp_bt_gap_get_cod_srvc(m_config.cod);
}

int32_t BluetoothDevice::getRssi() const {
    return m_config.rssi;
}

} // namespace dsx
