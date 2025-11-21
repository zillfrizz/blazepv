#pragma once
#include <stdint.h>

void file_push(const char* filePath, void* data, uint32_t size);
void* file_fetch(const char* filePath, uint32_t* aSize);
void* file_valid_check_fetch(const char* filePath, const char* srcPath, uint32_t* aSize);