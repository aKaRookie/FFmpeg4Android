#include <jni.h>
#include <string>

extern "C"
{
#include "libavformat/avformat.h"
#include <libavutil/imgutils.h>
#include <libyuv.h>
}

#include <android/log.h>

//使用这两个Window相关的头文件需要在CMake脚本中引入android库
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <unistd.h>

#define TAG "ffmpeg:"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
extern "C"
JNIEXPORT void

JNICALL
Java_com_fhl_ffmpegdemo_FFmpegVideoPlayer_render(
        JNIEnv *env,
        jclass /* this */, jstring path, jobject surface) {
    const char *path_str = env->GetStringUTFChars(path, NULL);
    //1.注册所有组件，例如初始化一些全局的变量，初始化网络等等。。。
    av_register_all();
    //avcodec_register_all();
    //封装格式上下文，统领全局的结构体，保存了视频文件封装格式的相关信息
    AVFormatContext *fmt_ctx = avformat_alloc_context();
    //2.打开输入视频文件
    if (avformat_open_input(&fmt_ctx, path_str, NULL, NULL) != 0) {
        LOGE("无法打开输入视频文件!");
        return;
    }
    //3.获取视频文件信息，例如得到视频的宽高
    //第二个参数是一个字典，表示你需要获取什么信息，比如视频的元数据
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        LOGE("无法获取视频文件信息");
        return;
    }
    //获取视频流的索引位置
    //遍历所有类型的流（音频流、视频流、字幕流），找到视频流
    int video_stream_idx = -1;
    for (int i = 0; i < fmt_ctx->nb_streams; ++i) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
            break;
        }
    }
    if (video_stream_idx == -1) {
        LOGE("找到不视频流\n");
        return;
    }
    LOGE("已找到视频流：%d", video_stream_idx);
    //只有知道视频的编码方式，才能够根据编码方式去找到解码器
    //获取视频流中的编解码上下文
    AVCodecContext *codec_ctx = NULL;
    //codec_ctx = avcodec_alloc_context3(NULL);
    codec_ctx = fmt_ctx->streams[video_stream_idx]->codec;
    if (codec_ctx == NULL) {
        LOGE("codec_ctx is NULL\n");
        return;
    }
    // avcodec_parameters_to_context(codec_ctx, fmt_ctx->streams[video_stream_idx]->codecpar);
    const AVCodec *codec = avcodec_find_decoder(codec_ctx->codec_id);
    if (codec == NULL) {
        LOGE("找不到解码器，或者视频已加密\n");
        return;
    }
    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
        LOGE("%s", "解码器无法打开\n");
        return;
    }
    //准备读取
    // AVPacket用于存储一帧一帧的压缩数据(H264)
    // 缓冲区，开辟空间
    AVPacket *packet = (AVPacket *) malloc(sizeof(AVPacket));
    //AVFrame用于存储解码后的像素数据(yuv)
    // 内存分配
    AVFrame *yuv_frame = av_frame_alloc();
    AVFrame *rgb_frame = av_frame_alloc();

    int got_frame = 0, ret;
    int frame_count = 0;
    //窗体
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    //绘制时的缓冲区
    ANativeWindow_Buffer outBuffer;

    //uint8_t *pBuffer = NULL;
    //int rgbSize = av_image_get_buffer_size(AV_PIX_FMT_ARGB, codec_ctx->width, codec_ctx->height, 1);
    //pBuffer = (uint8_t *) av_malloc(rgbSize);
    //6.一帧一帧的读取压缩数据
    while (av_read_frame(fmt_ctx, packet) >= 0) {

        //avcodec_receive_frame(codec_ctx, yuv_frame);
        //avcodec_send_frame(codec_ctx, yuv_frame);
        LOGE("packet stream_index:%d, video_stream_idx:%d", packet->stream_index, video_stream_idx);
        if (packet->stream_index == video_stream_idx) {
            ret = avcodec_decode_video2(codec_ctx, yuv_frame, &got_frame, packet);
            if (ret < 0) {
                LOGE("解码错误");
                return;
            } else {
                LOGE("解码成功");
            }
            //为0说明解码完成，非0正在解码
            if (got_frame) {
                //1.lock window
                //设置缓冲区的属性；宽高、像素格式（需要与java层的格式一致）

                ANativeWindow_setBuffersGeometry(nativeWindow, codec_ctx->width, codec_ctx->height,
                                                 WINDOW_FORMAT_RGBA_8888);
                //2.fix buffer

                //初始化缓冲区
                //设置属性，像素格式、宽高
                //rgb_frame的缓冲区就是window的缓冲区，同一个。解锁的时候就会进行绘制
                ANativeWindow_lock(nativeWindow, &outBuffer, NULL);
                //av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize, pBuffer, AV_PIX_FMT_ARGB,
                //                     codec_ctx->width, codec_ctx->height, 1);
                avpicture_fill((AVPicture *) rgb_frame, (const uint8_t *) outBuffer.bits,
                               AV_PIX_FMT_RGBA,
                               codec_ctx->width,
                               codec_ctx->height);
                //YUV格式的数据转换成RGBA 8888格式的数据
                //FFmpeg可以转，但是会有问题，因此我们使用libyuv这个库来做
                //https://chromium.googlesource.com/external/libyuv
                //参数分别是数据、对应一行的大小
                /*libyuv::I420ToARGB(yuv_frame->data[0], yuv_frame->linesize[0],
                                   yuv_frame->data[1], yuv_frame->linesize[1],
                                   yuv_frame->data[2], yuv_frame->linesize[2],
                                   rgb_frame->data[0], rgb_frame->linesize[0],
                                   codec_ctx->width, codec_ctx->height);*/

                libyuv::I420ToARGB(yuv_frame->data[0], yuv_frame->linesize[0],
                                   yuv_frame->data[2], yuv_frame->linesize[2],
                                   yuv_frame->data[1], yuv_frame->linesize[1],
                                   rgb_frame->data[0], rgb_frame->linesize[0],
                                   codec_ctx->width, codec_ctx->height
                );

                //3.unlock window
                ANativeWindow_unlockAndPost(nativeWindow);


                frame_count++;
                LOGE("解码绘制第%d帧", frame_count);

            }
        }
        //释放资源
        av_free_packet(packet);
        usleep(16 * 1000);
    }
    av_frame_free(&yuv_frame);
    avcodec_close(codec_ctx);
    avformat_free_context(fmt_ctx);
    env->ReleaseStringUTFChars(path, path_str);

}

extern "C"
JNIEXPORT void

JNICALL
Java_com_fhl_ffmpegdemo_FFmpegVideoPlayer_convert(
        JNIEnv *env,
        jclass /* this */, jstring inputPath, jstring outputPath) {
    AVOutputFormat *outFmt = NULL;
    AVBitStreamFilterContext *bsfCtx = NULL;
    //定义输入。输出AVFormatContext
    AVFormatContext *ifmtCtx = NULL, *ofmtCtx = NULL;
    AVPacket pkt;
    const char *inFileName = env->GetStringUTFChars(inputPath, NULL);
    const char *outFileName = env->GetStringUTFChars(outputPath, NULL);
    int ret, i;
    int frame_index = 0;

    av_register_all();
    //打开媒体文件
    if ((ret = avformat_open_input(&ifmtCtx, inFileName, 0, 0)) < 0) {
        LOGE("Could not open input file.");
        goto end;
    }
    //获取视频信息
    if ((ret = avformat_find_stream_info(ifmtCtx, 0)) < 0) {
        LOGE("Failed to retrieve input stream information");
        goto end;
    }
//    MP4中使用的是H.264编码，而H.264编码有两种封装模式
//    一种是annexb模式，他是传统模式，有startcode，SPS和PPS在Element Stream
//    中；另一种是mp4模式，一般MP4、MKV、AVI都没有startcode,SPS和PPS以及其他信息被封装在容器中
//    每一帧前面是这一帧的长度值，很多解码器只支持annexb模式，因此需要对MP4模式做转换在FFmpeg中用h264_mp4toannexb_filter可以进行模式转换；
//    使用命令 -bsf h264_mp4toannexb就可实现转换
    bsfCtx = av_bitstream_filter_init("h264_mp4toannexb");
    av_dump_format(ifmtCtx, 0, inFileName, 0);
//    初始化输出视频码流的AVFormatContext
    avformat_alloc_output_context2(&ofmtCtx, NULL, NULL, outFileName);
    if (!ofmtCtx) {
        LOGE("Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        goto end;
    }
    outFmt = ofmtCtx->oformat;
    AVStream *in_stream;
    AVStream *out_stream;
    for (i = 0; i < ifmtCtx->nb_streams; ++i) {
        //通过输入的AVStream创建输出的AVStream
        in_stream = ifmtCtx->streams[i];
        //初始化AVStream
        out_stream = avformat_new_stream(ofmtCtx, in_stream->codec->codec);
        if (!out_stream) {
            LOGE("Failed allocating output stream\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }
        //复制AVCodecContext的设置属性
        if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0) {
            LOGE("Failed to copy context from input to output stream codec context\n");
            goto end;
        }
        out_stream->codec->codec_tag = 0;
        if (ofmtCtx->oformat->flags & AVFMT_GLOBALHEADER) {
            out_stream->codec->codec_tag |= CODEC_FLAG_GLOBAL_HEADER;
        }

        //输出信息
        av_dump_format(ofmtCtx, 0, outFileName, 1);
        //打开输出文件
        if (!(outFmt->flags & AVFMT_NOFILE)) {
            ret = avio_open(&ofmtCtx->pb, outFileName, AVIO_FLAG_WRITE);
            if (ret < 0) {
                LOGE("Could not open output file '%s'", outFileName);
                goto end;
            }
        }
        //写文件头
        if (avformat_write_header(ofmtCtx, NULL) < 0) {
            LOGE("Error occurred when opening output file\n");
            goto end;
        }
        while (1) {
            AVStream *in_stream, *out_stream;
            //得到一个AVPacket
            ret = av_read_frame(ifmtCtx, &pkt);
            if (ret < 0) {
                break;
            }
            in_stream = ifmtCtx->streams[pkt.stream_index];
            out_stream = ofmtCtx->streams[pkt.stream_index];

            //转换PTS/DTS
            pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base,
                                       (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base,
                                       (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
            pkt.pos = -1;

            if (pkt.stream_index == 0) {
                AVPacket fpkt = pkt;
                int a = av_bitstream_filter_filter(bsfCtx, out_stream->codec, NULL,
                                                   &fpkt.data, &fpkt.size, pkt.data, pkt.size,
                                                   pkt.flags & AV_PKT_FLAG_KEY);
                pkt.data = fpkt.data;
                pkt.size = fpkt.size;
            }
            //写AVPacket
            if (av_write_frame(ofmtCtx, &pkt) < 0) {
                //将AVPacket（存储音频压缩码流数据）写入文件
                LOGE("Error muxing packet\n");
                break;
            }
            LOGE("Write %d frames to out file\n", frame_index);
            av_packet_unref(&pkt);
            frame_index++;
        }
        //写文件尾
        av_write_trailer(ofmtCtx);
        end:
        avformat_close_input(&ifmtCtx);
        //关闭输出
        if (ofmtCtx && !(ofmtCtx->flags & AVFMT_NOFILE)) {
            avio_close(ofmtCtx->pb);
        }
        avformat_free_context(ofmtCtx);
        //system("pause");
        return;
    }

}