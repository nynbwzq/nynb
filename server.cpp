#include <iostream>
#include <dlfcn.h>
#include <cstring>
using namespace std;

// 伪造 JNI 环境
typedef void* JNIEnv; typedef void* jobject; typedef void* jmethodID; typedef int jint; typedef int* jintArray;
void* fake_jni_table[256] = {0};
void* fake_env_ptr = fake_jni_table;
jint Fake_CallIntMethod(JNIEnv* e, jobject o, jmethodID m, ...) { return 1; }
void Fake_CallVoidMethod(JNIEnv* e, jobject o, jmethodID m, ...) {}
jintArray Fake_NewIntArray(JNIEnv* e, jint l) { return new int[l]; }
void Fake_GetIntArrayRegion(JNIEnv* e, jintArray a, jint s, jint l, jint* b) { if(a && b) memcpy(b, a+s, l*sizeof(jint)); }
void setup_fake_jni() { for(int i=0; i<256; i++) fake_jni_table[i] = (void*)Fake_CallVoidMethod; }

typedef jint (*TEFACES_t)(JNIEnv*, jobject, jint, jint, jint, jint, jintArray);
TEFACES_t TEFACES = nullptr;
int chessList[227] = {0};
int step_num = 0;

int main() {
    setup_fake_jni();
    cout << "正在加载 ./libmmai.so ..." << endl;
    void* handle = dlopen("./libmmai.so", RTLD_LAZY | RTLD_LOCAL);
    if (!handle) { cout << "❌ dlopen失败: " << dlerror() << endl; return 1; }
    cout << "✅ 成功加载!" << endl;
    
    auto LoadLib = (void(*)(char*))dlsym(handle, "_Z7LoadLibPc");
    auto rap_init = (void(*)())dlsym(handle, "_Z8rap_initv");
    TEFACES = (TEFACES_t)dlsym(handle, "Java_com_bf92_ai_AI_TEFACES");
    
    if (!TEFACES) { cout << "❌ 找不到 TEFACES" << endl; return 1; }
    if (rap_init) rap_init();
    if (LoadLib) {
        char path_buf[] = "./bg.png";
        LoadLib(path_buf);
    }
    
    // 初始化新局 (指令 0x2)
    TEFACES((JNIEnv*)&fake_env_ptr, nullptr, 0x2, 1, 0, 0, chessList);
    
    // 模拟黑棋下天元 (7,7)，一维索引 112
    int pos = 7 * 15 + 7;
    TEFACES((JNIEnv*)&fake_env_ptr, nullptr, 0x3, 1, pos, step_num, chessList);
    chessList[step_num++] = pos;
    
    // 设置时间 5 秒，让 AI (白棋) 思考 (指令 0x7 和 0x6)
    TEFACES((JNIEnv*)&fake_env_ptr, nullptr, 0x7, 2, 0, 5, chessList);
    jint res = TEFACES((JNIEnv*)&fake_env_ptr, nullptr, 0x6, 2, 0, step_num, chessList);
    
    cout << "🔥 AI 返回落子坐标 (0-224): " << res << endl;
    return 0;
}