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

#include <utils/log.h>

#include <esp_err.h>

#include <source_location>

#define DSX_RESULT_ERROR(error, message, ...) \
    dsx::resultError(std::source_location::current(), error, k_logTag, message, ##__VA_ARGS__)

#define DSX_RESULT_SUCCESS() \
    dsx::resultSuccess()

#define DSX_CHECK_RESULT(result) \
    dsx::checkResult((result))

namespace dsx {

struct Result {
    operator esp_err_t() const {
        return error;
    }

    explicit operator bool() const {
        return error == ESP_OK;
    }

    bool operator!=(esp_err_t error_) const {
        return error != error_;
    }

    bool operator==(esp_err_t error_) const {
        return error == error_;
    }

    bool operator!=(Result other) const {
        return error != other.error;
    }

    bool operator==(Result other) const {
        return error == other.error;
    }

    esp_err_t error;
    std::string message;
    std::string tag;
};

template <typename... Args>
inline Result resultError(
    const std::source_location &location,
    esp_err_t error,
    std::string_view tag,
    std::string_view message,
    Args &&...args) {
    constexpr std::string_view k_errorMessageFormat = "{}: {}\n\t{} ({}:{}) ";
    const auto errorMessage = std::format(
        k_errorMessageFormat,
        std::vformat(message, std::make_format_args(args...)),
        esp_err_to_name(error),
        location.function_name(),
        location.file_name(),
        location.line());

    return Result{
        .error = error,
        .message = errorMessage,
        .tag = tag.data(),
    };
}

inline Result resultSuccess() {
    return Result{
        .error = ESP_OK,
        .message = "",
        .tag = "",
    };
}

inline void checkResult(const Result &result) {
    if (!result) {
        log(LogSeverity::k_error, result.tag, result.message);
        ESP_ERROR_CHECK(result);
    }
}

inline void checkResult(esp_err_t error) {
    ESP_ERROR_CHECK(error);
}

} // namespace dsx
