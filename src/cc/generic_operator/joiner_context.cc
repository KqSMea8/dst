/****************************************************************
*
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*
*****************************************************************/
/**
 * @file joiner_context.cc
 * @author lanbijia
 * @date 2012/11/5
 * @brief 定义Joiner类通用部分
 */

#include "generic_operator/joiner_context.h"

namespace dstream {
namespace generic_operator {

const char* JoinerContext::kJoinConfigRoot      = "JoinConfig";
const char* JoinerContext::kJoinConfigTag       = "JoinTags";
const char* JoinerContext::kJoinStorageTag      = "Storage";
const char* JoinerContext::kWindowStep          = "WindowStep";
const char* JoinerContext::kWindowSize          = "WindowSize";
const char* JoinerContext::kJoinWithStore       = "JoinWithStore";
const char* JoinerContext::kRestoreFromStore    = "RestoreFromStore";
const char* JoinerContext::kIgnoreStoreError    = "IgnoreStoreError";
const char* JoinerContext::kStoreBatchInsert    = "BatchInsert";
const char* JoinerContext::kStoreBatchCount     = "BatchCount";
const char* JoinerContext::kStoreBatchIntervalMs = "BatchIntervalMs";

const int32_t JoinerContext::kDefaultWindowStep = 10; // 单位：秒
const int32_t JoinerContext::kDefaultWindowSize = 10; // 单位：秒
const bool JoinerContext::kDefaultJoinWithStore = false;
const bool JoinerContext::kDefaultRestoreFromStore = false;
const bool JoinerContext::kDefaultIgnoreStoreError = true;
const bool JoinerContext::kDefaultStoreBatchInsert = true;
const int32_t JoinerContext::kDefaultStoreBatchCount = 100;
const int32_t JoinerContext::kDefaultStoreBatchIntervalMs = 100;


} // namespace generic_operator
} // namespace dstream


