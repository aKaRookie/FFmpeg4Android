package com.fhl.ffmpegdemo;

import android.Manifest;
import android.os.Environment;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.View;
import android.widget.TextView;

import com.fhl.ffmpegdemo.permission.IPermission;
import com.fhl.ffmpegdemo.permission.PermissionHelper;

import java.io.File;

public class MainActivity extends AppCompatActivity implements View.OnClickListener, IPermission {


    private static final String TAG = MainActivity.class.getSimpleName();
    VideoView        videoView;
    TextView         tv;
    PermissionHelper mPermissionHelper;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        init();
        tv = (TextView) findViewById(R.id.sample_text);
        tv.setOnClickListener(this);
        videoView = findViewById(R.id.videoView);
    }

    private void init() {
        mPermissionHelper = new PermissionHelper(this, this);
        mPermissionHelper.requestPermissions();
    }

    private void requestPermission() {
        if (ActivityCompat.shouldShowRequestPermissionRationale(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)) {

        } else {

        }
    }


    private void render() {
        String path = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "Movies" + File.separator + "test.mp4";
        File file = new File(path);
        if (!file.exists()) {
            Log.e(TAG, path + " 不存在!");
        } else {
            Log.d(TAG, path + " 存在!");
        }
        FFmpegVideoPlayer.render(path, videoView.getHolder().getSurface());
    }

    boolean isPlay = false;

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.sample_text:
                tv.setVisibility(View.GONE);
                tv.post(new Runnable() {
                    @Override
                    public void run() {
                        render();
                        tv.setVisibility(View.VISIBLE);
                    }
                });

                break;
            default:
        }

    }

    @Override
    protected void onStop() {
        super.onStop();

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        videoView = null;
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (mPermissionHelper.requestPermissionsResult(requestCode, permissions, grantResults)) {
            //权限请求结果，并已经处理了该毁掉
            return;
        }
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }

    @Override
    public int getPermissionRequestCode() {
        return 10000;
    }

    @Override
    public String[] getPermissions() {
        return new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE};
    }

    @Override
    public void requestPermissionsSuccess() {
        Log.d(TAG, "权限请求成功！");
    }

    @Override
    public void requestPermissionsFail() {
        Log.w(TAG, "权限请求失败！");
    }
}
