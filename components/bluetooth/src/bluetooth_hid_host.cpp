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

#include "bluetooth_hid_host.h"

#include <bluetooth/bluetooth_device.h>
#include <bluetooth/events/bluetooth_event.h>

DSX_LOG_TAG(BluetoothHidHost);

namespace dsx {

namespace {

BluetoothEventCallback s_eventCallback;

void callback(void *arg, esp_event_base_t eventBase, int32_t id, void *eventData);
void handleHidDeviceOpenEvent(esp_hidh_event_data_t *param);
void handleHidDeviceInputReportEvent(esp_hidh_event_data_t *param);
void handleHidDeviceFeatureReportEvent(esp_hidh_event_data_t *param);
void handleHidDeviceCloseEvent(esp_hidh_event_data_t *param);

} // namespace

Result BluetoothHidHost::initialize(BluetoothEventCallback eventCallback) {
    assert(!s_eventCallback && "bluetooth hid host already initialized");

    constexpr uint16_t k_eventStackSize = 4096u;
    esp_hidh_config_t config = {
        .callback = callback,
        .event_stack_size = k_eventStackSize,
        .callback_arg = nullptr,
    };
    esp_err_t error = esp_hidh_init(&config);
    if (error != ESP_OK) {
        return DSX_RESULT_ERROR(error, "hid host init failed");
    }

    s_eventCallback = eventCallback;

    DSX_LOGD("bluetooth hid host initialized");

    return DSX_RESULT_SUCCESS();
}

Result BluetoothHidHost::deinitialize() {
    esp_err_t error = esp_hidh_deinit();
    if (error != ESP_OK) {
        return DSX_RESULT_ERROR(error, "hid host deinit failed");
    }
    DSX_LOGD("bluetooth hid host deinitialized");

    return DSX_RESULT_SUCCESS();
}

Result BluetoothHidHost::openHidDeviceConnection(BluetoothDeviceAddress &address, esp_hid_transport_t transport, esp_ble_addr_type_t bleAddressType) {
    DSX_LOGD("esp_hidh_dev_open called");

    if (esp_hidh_dev_open(address.data(), transport, bleAddressType) == nullptr) {
        return DSX_RESULT_ERROR(ESP_FAIL, "unable to open hid host device");
    }

    return DSX_RESULT_SUCCESS();
}

Result BluetoothHidHost::getHidDeviceFeatureReport(esp_hidh_dev_t *deviceData, size_t reportMapIndex, size_t reportId, size_t size, uint8_t *buffer, size_t *sizeOut) {
    DSX_LOGD("esp_hidh_dev_feature_get called");

    esp_err_t error = esp_hidh_dev_feature_get(deviceData, reportMapIndex, reportId, size, buffer, sizeOut);
    if (error != ESP_OK) {
        return DSX_RESULT_ERROR(error, "unable to send hid host device ");
    }

    return DSX_RESULT_SUCCESS();
}

Result BluetoothHidHost::getHidDeviceBluetoothAddress(esp_hidh_dev_t *deviceData, BluetoothDeviceAddress &addressOut) {
    DSX_LOGD("esp_hidh_dev_bda_get called");

    const auto *address = esp_hidh_dev_bda_get(deviceData);
    if (address == nullptr) {
        return DSX_RESULT_ERROR(ESP_FAIL, "unable to get hid host device bluetooth address");
    }

    std::copy(address, address + ESP_BD_ADDR_LEN, addressOut.data());

    return DSX_RESULT_SUCCESS();
}

namespace {

void callback(void *arg, esp_event_base_t eventBase, int32_t id, void *eventData) {
    esp_hidh_event_t event = static_cast<esp_hidh_event_t>(id);
    esp_hidh_event_data_t *param = static_cast<esp_hidh_event_data_t *>(eventData);

    switch (event) {
    case ESP_HIDH_OPEN_EVENT:
        handleHidDeviceOpenEvent(param);
        break;

    case ESP_HIDH_INPUT_EVENT:
        handleHidDeviceInputReportEvent(param);
        break;

    case ESP_HIDH_FEATURE_EVENT:
        handleHidDeviceFeatureReportEvent(param);
        break;

    case ESP_HIDH_CLOSE_EVENT:
        handleHidDeviceCloseEvent(param);
        break;

    default:
        DSX_LOGD("bluetooth hid host unhandled event: {}", id);
        break;
    }
}

void handleHidDeviceOpenEvent(esp_hidh_event_data_t *param) {
    BluetoothEvent event{BluetoothHidDeviceOpenEvent{
        .deviceData = param->open.dev,
        .status = param->open.status,
    }};
    s_eventCallback(event);
}

void handleHidDeviceInputReportEvent(esp_hidh_event_data_t *param) {
    // DSX_LOGI("size= {}, data[0]=0x{:02X}", param->input.length, param->input.data[0]);
}

void handleHidDeviceFeatureReportEvent(esp_hidh_event_data_t *param) {
}

void handleHidDeviceCloseEvent(esp_hidh_event_data_t *param) {
    BluetoothEvent event{BluetoothHidDeviceCloseEvent{
        .deviceData = param->close.dev,
        .reason = param->close.reason,
        .status = param->close.status,
    }};
    s_eventCallback(event);
}

} // namespace

} // namespace dsx
