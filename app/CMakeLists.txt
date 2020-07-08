cmake_minimum_required(VERSION 3.4.1)
add_library(
        native-lib
        SHARED
        src/main/cpp/native-lib.cpp)
find_library(
        log-lib
        log)

#找到Android的Window绘制相关的库（这个库已经在Android平台中了）
find_library(
        android-lib
        android
)
#声明导入文件更目录变量ARM_DIR ,这使用了相对与系统的更目录，因为使用相对路径似乎不起作用
#Users/wangyijin/Desktop/demo/FFmpeg4Android/app/
set(ARM_DIR E:\\GitDownloads\\FFmpeg4Android\\app\\src\\main\\jniLibs)


#导入头文件
include_directories(src/main/jniLibs/include)

#添加动态库
add_library(avcodec-57
        SHARED
        IMPORTED)
#设置动态库路径
set_target_properties(avcodec-57
        PROPERTIES IMPORTED_LOCATION
        ${ARM_DIR}/armeabi/libavcodec-57.so
        )

add_library(avdevice-57
        SHARED
        IMPORTED)
set_target_properties(avdevice-57
        PROPERTIES IMPORTED_LOCATION
        ${ARM_DIR}/armeabi/libavdevice-57.so)
add_library(avformat-57
        SHARED
        IMPORTED)
set_target_properties(avformat-57
        PROPERTIES IMPORTED_LOCATION
        ${ARM_DIR}/armeabi/libavformat-57.so)
add_library(avutil-55
        SHARED
        IMPORTED)
set_target_properties(avutil-55
        PROPERTIES IMPORTED_LOCATION
        ${ARM_DIR}/armeabi/libavutil-55.so)
add_library(postproc-54
        SHARED
        IMPORTED)
set_target_properties(postproc-54
        PROPERTIES IMPORTED_LOCATION
        ${ARM_DIR}/armeabi/libpostproc-54.so)
add_library(swresample-2
        SHARED
        IMPORTED)
set_target_properties(swresample-2
        PROPERTIES IMPORTED_LOCATION
        ${ARM_DIR}/armeabi/libswresample-2.so)
add_library(swscale-4
        SHARED
        IMPORTED)
set_target_properties(swscale-4
        PROPERTIES IMPORTED_LOCATION
        ${ARM_DIR}/armeabi/libswscale-4.so)
add_library(avfilter-6
        SHARED
        IMPORTED)
set_target_properties(avfilter-6
        PROPERTIES IMPORTED_LOCATION
        ${ARM_DIR}/armeabi/libavfilter-6.so)


add_library(yuv
        SHARED
        IMPORTED)
set_target_properties(yuv
        PROPERTIES IMPORTED_LOCATION
        ${ARM_DIR}/armeabi/libyuv.so )


target_link_libraries(
        native-lib
        avcodec-57
        avdevice-57
        avformat-57
        avfilter-6
        avutil-55
        postproc-54
        swresample-2
        swscale-4
        yuv
        ${android-lib}
        ${log-lib})