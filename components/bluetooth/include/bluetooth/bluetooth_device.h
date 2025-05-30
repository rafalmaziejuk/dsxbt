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

#include <bluetooth/bluetooth_types.h>
#include <utils/result.h>

namespace dsx {

class BluetoothDevice final {
  public:
    explicit BluetoothDevice(const BluetoothDeviceConfig &config);
    ~BluetoothDevice();

    std::string getName() const;
    std::string getShortLocalName() const;
    std::string getCompleteLocalName() const;
    std::string getAddressStr() const;
    esp_bd_addr_t &getAddress();
    uint32_t getClassOfDevice() const;
    uint32_t getMajorDeviceClass() const;
    uint32_t getMinorDeviceClass() const;
    uint32_t getServiceClass() const;
    int32_t getRssi() const;
    std::vector<std::string> getRemoteServicesStr() const;
    std::vector<esp_bt_uuid_t> &getRemoteServices() const;
    void setRemoteServicesRaw(const std::vector<esp_bt_uuid_t> &services);

  private:
    struct Impl;
    Impl *m_impl{nullptr};
};

} // namespace dsx
