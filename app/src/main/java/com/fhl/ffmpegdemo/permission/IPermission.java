package com.fhl.ffmpegdemo.permission;

/**
 * @ProjectName: FFmpeg4Android
 * @Package: com.fhl.ffmpegdemo.permission
 * @ClassName: IPermission
 * @Description: 权限请求接口
 * @Author: fenghl
 * @CreateDate: 2020/7/3 17:51
 * @UpdateUser: 更新者：
 * @UpdateDate: 2020/7/3 17:51
 * @UpdateRemark: 更新说明：
 * @Version: 1.0
 */
public interface IPermission {
    /**
     * 可设置请求权限请求码
     */
    int getPermissionRequestCode();

    /**
     * 设置需要请求的权限
     */
    String[] getPermissions();

    /**
     * 请求权限成功回调
     */
    void requestPermissionsSuccess();

    /**
     * 请求权限失败回调
     */
    void requestPermissionsFail();
}
