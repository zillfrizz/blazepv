#include <bz_file.h>
#include <stdlib.h>
#include <io.h>
#include <stdio.h>
#include <sys/stat.h>

/* 
   UTILS
*/
void file_push(const char* filePath, void* data, uint32_t size) {
    FILE* f = fopen(filePath, "wb");
    fwrite(data, 1, size, f);
    fclose(f);
    free(data);
}

void* file_fetch(const char* filePath, uint32_t* aSize) {
    FILE* f = fopen(filePath, "rb");
    fseek(f, 0, SEEK_END);
    *aSize = ftell(f);
    rewind(f);
    void* buffer = malloc(*aSize);
    *aSize = fread(buffer, 1, *aSize, f);
    fclose(f);
    return buffer;
}

void* file_valid_check_fetch(const char* filePath, const char* srcPath, uint32_t* aSize) {
    struct stat fileStats, sourceStats;
    if (stat(filePath, &fileStats) || stat(srcPath, &sourceStats)) return 0;
    if (fileStats.st_mtime < sourceStats.st_mtime) return 0;
    return file_fetch(filePath, aSize);
}
