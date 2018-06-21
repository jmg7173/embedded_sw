#include <jni.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "android/log.h"

#define DRIVER_FILE "/dev/dev_driver"

#define LOG_TAG "NATIVE"
#define LOGV(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

JNIEXPORT jint JNICALL Java_com_example_puzzle_MainActivity2_openDriver(JNIEnv *env, jobject this){
    int fd = open(DRIVER_FILE, O_RDWR);
    if(fd < 0){
        LOGV("fd: %d /dev/dev_driver open error", fd);
        exit(-1);
    }
    return fd;
}

JNIEXPORT void JNICALL Java_com_example_puzzle_MainActivity2_writeDriver(JNIEnv *env, jobject this, jint fd, jint val){
    int ret = write(fd, &val, 2);
}

JNIEXPORT jint JNICALL Java_com_example_puzzle_MainActivity2_readDriver(JNIEnv *env, jobject this, jint fd){
    char buf[2] = {0};
    int ret = read(fd, buf, 2);
    return ret;
}

JNIEXPORT void JNICALL Java_com_example_puzzle_MainActivity2_closeDriver(JNIEnv *env, jobject this, jint fd){
    int ret = close(fd);
    LOGV("close file: %d", ret);
}

