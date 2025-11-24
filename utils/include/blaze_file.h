#pragma once
#include <stdint.h>

void blaze_file_push(const char* filePath, void* data, uint32_t size);
void* blaze_file_fetch(const char* filePath, uint32_t* aSize);
void* blaze_file_valid_check_fetch(const char* filePath, const char* srcPath, uint32_t* aSize);