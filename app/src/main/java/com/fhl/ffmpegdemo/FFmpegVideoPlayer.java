package com.fhl.ffmpegdemo;

import android.view.Surface;

/**
 * @ProjectName: FFmpeg4Android
 * @Package: com.fhl.ffmpegdemo
 * @ClassName: FFmpegVideoPlayer
 * @Description: java类作用描述
 * @Author: fenghl
 * @CreateDate: 2020/7/3 18:03
 * @UpdateUser: 更新者：
 * @UpdateDate: 2020/7/3 18:03
 * @UpdateRemark: 更新说明：
 * @Version: 1.0
 */
public class FFmpegVideoPlayer {
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("avcodec-57");

        System.loadLibrary("avdevice-57");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avformat-57");
        System.loadLibrary("avutil-55");
        System.loadLibrary("postproc-54");
        System.loadLibrary("swresample-2");
        System.loadLibrary("swscale-4");
        System.loadLibrary("yuv");
        System.loadLibrary("native-lib");
    }
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public static native void render(String path, Surface surface);
}
