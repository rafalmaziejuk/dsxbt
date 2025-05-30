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

#include <bluetooth/events/bluetooth_gap_event.h>

#include <variant>

namespace dsx {

class BluetoothEvent {
  public:
    template <typename EventType>
    BluetoothEvent(const EventType &eventType) {
        m_data = eventType;
    }

    template <typename EventType>
    [[nodiscard]] std::optional<EventType> get() const {
        if (std::holds_alternative<EventType>(m_data)) {
            return *std::get_if<EventType>(&m_data);
        }

        return std::nullopt;
    }

  private:
    using EventDataType = std::variant<BluetoothDeviceDiscoveredEvent,
                                       BluetoothDiscoveryStateChangedEvent,
                                       BluetoothRemoteServicesDiscoveredEvent,
                                       BluetoothRemoteServiceRecordDiscoveredEvent>;

    EventDataType m_data;
};

} // namespace dsx
