//==============================================================================
//
//  OvenMediaEngine
//
//  Created by Hyunjun Jang
//  Copyright (c) 2018 AirenSoft. All rights reserved.
//
//==============================================================================
#pragma once

#define logd(...) do {} while(false)
#define logi(...) do {} while(false)
#define logw(...) do {} while(false)
#define loge(...) do {} while(false)
#define logc(...) do {} while(false)
#define logp(...) do {} while(false)

#define logtd(format, ...)                            logd(OV_LOG_TAG, format, ## __VA_ARGS__) // NOLINT
#define logtp(format, ...)                            logp(OV_LOG_TAG ".Packet", format, ## __VA_ARGS__) // NOLINT
#define logts(format, ...)                            logi(OV_LOG_TAG ".Stat", format, ## __VA_ARGS__) // NOLINT

#define logti(format, ...)                            logi(OV_LOG_TAG, format, ## __VA_ARGS__) // NOLINT
#define logtw(format, ...)                            logw(OV_LOG_TAG, format, ## __VA_ARGS__) // NOLINT
#define logte(format, ...)                            loge(OV_LOG_TAG, format, ## __VA_ARGS__) // NOLINT
#define logtc(format, ...)                            logc(OV_LOG_TAG, format, ## __VA_ARGS__) // NOLINT
