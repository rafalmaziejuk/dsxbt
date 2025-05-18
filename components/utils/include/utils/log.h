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

#include <esp_log.h>

#include <format>

#define DSX_LOG_TAG(tag) \
    static constexpr std::string_view k_logTag = #tag

#define DSX_LOG_IMPL(verbosity, message, ...) \
    dsx::log(verbosity, k_logTag, message, ##__VA_ARGS__)

#define DSX_LOGD(message, ...) DSX_LOG_IMPL(dsx::LogSeverity::k_debug, message, ##__VA_ARGS__)
#define DSX_LOGI(message, ...) DSX_LOG_IMPL(dsx::LogSeverity::k_info, message, ##__VA_ARGS__)
#define DSX_LOGW(message, ...) DSX_LOG_IMPL(dsx::LogSeverity::k_warn, message, ##__VA_ARGS__)
#define DSX_LOGE(message, ...) DSX_LOG_IMPL(dsx::LogSeverity::k_error, message, ##__VA_ARGS__)

namespace dsx {

enum class LogSeverity : uint8_t {
    k_debug,
    k_info,
    k_warn,
    k_error
};

template <typename... Args>
inline void log(LogSeverity severity, std::string_view tag, std::string_view format, Args &&...args) {
    const auto message = std::vformat(format, std::make_format_args(args...));

    switch (severity) {
    case LogSeverity::k_debug:
        ESP_LOGD(tag.data(), "%s", message.c_str());
        break;

    case LogSeverity::k_info:
        ESP_LOGI(tag.data(), "%s", message.c_str());
        break;

    case LogSeverity::k_warn:
        ESP_LOGW(tag.data(), "%s", message.c_str());
        break;

    case LogSeverity::k_error:
        ESP_LOGE(tag.data(), "%s", message.c_str());
        break;
    }
}

} // namespace dsx
