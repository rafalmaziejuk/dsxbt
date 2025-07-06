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

void callback(esp_hidh_cb_event_t event, esp_hidh_cb_param_t *param);
void handleHidHostDeviceOpenEvent(esp_hidh_cb_param_t *param);
void handleHidHostDeviceCloseEvent(esp_hidh_cb_param_t *param);

} // namespace

Result BluetoothHidHost::initialize(BluetoothEventCallback eventCallback) {
    assert(!s_eventCallback && "bluetooth hid host already initialized");

    esp_err_t error = esp_bt_hid_host_init();
    if (error != ESP_OK) {
        return DSX_RESULT_ERROR(error, "unable to initialize bluetooth hid host");
    }

    error = esp_bt_hid_host_register_callback(callback);
    if (error != ESP_OK) {
        return DSX_RESULT_ERROR(error, "unable to register bluetooth hid host event callback");
    }

    s_eventCallback = eventCallback;

    DSX_LOGD("bluetooth hid host initialized");

    return DSX_RESULT_SUCCESS();
}

Result BluetoothHidHost::deinitialize() {
    esp_err_t error = esp_bt_hid_host_deinit();
    if (error != ESP_OK) {
        return DSX_RESULT_ERROR(error, "unable to deinit bluetooth hid host");
    }

    DSX_LOGD("bluetooth hid host deinitialized");

    return DSX_RESULT_SUCCESS();
}

Result BluetoothHidHost::connectDevice(BluetoothDeviceAddress &address) {
    DSX_LOGD("esp_bt_hid_host_connect called");

    esp_err_t error = esp_bt_hid_host_connect(address.data());
    if (error != ESP_OK) {
        return DSX_RESULT_ERROR(error, "unable to connect to hid host device");
    }

    return DSX_RESULT_SUCCESS();
}

Result BluetoothHidHost::disconnectDevice(BluetoothDeviceAddress &address) {
    DSX_LOGD("esp_bt_hid_host_connect called");

    esp_err_t error = esp_bt_hid_host_disconnect(address.data());
    if (error != ESP_OK) {
        return DSX_RESULT_ERROR(error, "unable to disconnect from hid host device");
    }

    return DSX_RESULT_SUCCESS();
}

Result BluetoothHidHost::getDeviceReport(BluetoothDeviceAddress &address, esp_hidh_report_type_t reportType, uint8_t reportId, size_t size) {
    DSX_LOGD("esp_bt_hid_host_get_report called");

    esp_err_t error = esp_bt_hid_host_get_report(address.data(), reportType, reportId, size);
    if (error != ESP_OK) {
        return DSX_RESULT_ERROR(error, "unable to get hid host device report");
    }

    return DSX_RESULT_SUCCESS();
}

namespace {

void callback(esp_hidh_cb_event_t event, esp_hidh_cb_param_t *param) {
    switch (event) {
    case ESP_HIDH_OPEN_EVT:
        handleHidHostDeviceOpenEvent(param);
        break;

    case ESP_HIDH_CLOSE_EVT:
        handleHidHostDeviceCloseEvent(param);
        break;

    default:
        DSX_LOGD("bluetooth hid host unhandled event: {}", static_cast<uint32_t>(event));
        break;
    }
}

void handleHidHostDeviceOpenEvent(esp_hidh_cb_param_t *param) {
    BluetoothDeviceAddress address{};
    std::copy(param->open.bd_addr,
              param->open.bd_addr + ESP_BD_ADDR_LEN,
              address.begin());

    BluetoothEvent event{BluetoothHidDeviceOpenEvent{
        .status = param->open.status,
        .connectionState = param->open.conn_status,
        .address = address,
    }};
    s_eventCallback(event);
}

void handleHidHostDeviceCloseEvent(esp_hidh_cb_param_t *param) {
    BluetoothEvent event{BluetoothHidDeviceCloseEvent{
        .status = param->close.status,
        .connectionState = param->close.conn_status,
    }};
    s_eventCallback(event);
}

} // namespace

} // namespace dsx
