package com.fhl.ffmpegdemo;

import android.content.Context;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * @ProjectName: FFmpeg4Android
 * @Package: com.fhl.ffmpegdemo
 * @ClassName: VideoView
 * @Description: java类作用描述
 * @Author: fenghl
 * @CreateDate: 2020/7/2 22:34
 * @UpdateUser: 更新者：
 * @UpdateDate: 2020/7/2 22:34
 * @UpdateRemark: 更新说明：
 * @Version: 1.0
 */
public class VideoView extends SurfaceView {
    public VideoView(Context context) {
        this(context,null);
    }

    public VideoView(Context context, AttributeSet attrs) {
        this(context, attrs,0);
    }

    public VideoView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init() {
        SurfaceHolder holder = getHolder();
        holder.setFormat(PixelFormat.RGBA_8888);
    }
}
