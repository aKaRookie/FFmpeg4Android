//
// Created by Administrator on 2020/7/2.
//

#ifndef FFMPEG4ANDROID_NATIVE_LIB_H
#define FFMPEG4ANDROID_NATIVE_LIB_H

#include "../../../../../../Android/Sdk/ndk-bundle/sysroot/usr/include/jni.h"
extern "C"
JNIEXPORT void

JNICALL
Java_com_fhl_ffmpegdemo_FFmpegVideoPlayer_render(JNIEnv *,
                                                  jclass /* this */, jstring, jobject);

#endif //FFMPEG4ANDROID_NATIVE_LIB_H
