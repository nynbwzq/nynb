#include <stdio.h>
#include <stdarg.h>

// 伪造 liblog.so 里的日志函数
int __android_log_print(int prio, const char* tag, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
    return 1;
}

// 伪造 libandroid.so 里的资源函数
void* AAssetManager_fromJava(void* env, void* assetManager) { return 0; }
void* AAssetManager_open(void* mgr, const char* filename, int mode) { return 0; }
void AAsset_close(void* asset) {}
long AAsset_getLength(void* asset) { return 0; }
int AAsset_read(void* asset, void* buf, unsigned long count) { return 0; }
