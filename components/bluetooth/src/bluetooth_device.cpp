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

struct BluetoothDevice::Impl {
    BluetoothDeviceConfig config{};
};

BluetoothDevice::BluetoothDevice(const BluetoothDeviceConfig &config)
    : m_impl{new Impl{}} {
    m_impl->config = config;
}

BluetoothDevice::~BluetoothDevice() {
    delete m_impl;
    m_impl = nullptr;
}

std::string BluetoothDevice::getName() const {
    return m_impl->config.name;
}

std::string BluetoothDevice::getShortLocalName() const {
    return m_impl->config.eir.shortLocalName;
}

std::string BluetoothDevice::getCompleteLocalName() const {
    return m_impl->config.eir.completeLocalName;
}

std::string BluetoothDevice::getAddressStr() const {
    return parseBluetoothDeviceAddress(m_impl->config.address);
}

esp_bd_addr_t &BluetoothDevice::getAddress() {
    return m_impl->config.address;
}

uint32_t BluetoothDevice::getClassOfDevice() const {
    return m_impl->config.cod;
}

uint32_t BluetoothDevice::getMajorDeviceClass() const {
    return esp_bt_gap_get_cod_major_dev(m_impl->config.cod);
}

uint32_t BluetoothDevice::getMinorDeviceClass() const {
    return esp_bt_gap_get_cod_minor_dev(m_impl->config.cod);
}

uint32_t BluetoothDevice::getServiceClass() const {
    return esp_bt_gap_get_cod_srvc(m_impl->config.cod);
}

int32_t BluetoothDevice::getRssi() const {
    return m_impl->config.rssi;
}

std::vector<std::string> BluetoothDevice::getRemoteServicesStr() const {
    std::vector<std::string> remoteServices(m_impl->config.remoteServices.size());
    for (uint32_t i = 0u; i < m_impl->config.remoteServices.size(); i++) {
        auto uuid = m_impl->config.remoteServices[i];
        remoteServices[i] = parseBluetoothDeviceRemoteService(&uuid);
    }

    return remoteServices;
}

std::vector<esp_bt_uuid_t> &BluetoothDevice::getRemoteServices() const {
    return m_impl->config.remoteServices;
}

void BluetoothDevice::setRemoteServicesRaw(const std::vector<esp_bt_uuid_t> &services) {
    m_impl->config.remoteServices = services;
}

} // namespace dsx
