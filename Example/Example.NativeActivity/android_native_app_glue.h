/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * ������������� �� �������� Apache License, ������ 2.0 ( "��������");
 *���� ���� ����� ������������ ������ � ������������ � ���������.
 *����� �������� ����� �������� �� ���-�����
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *���� ������ �� ��������� � ������������ � ���������� ����������������� ��� ����������� � ���������� ����, ����������� �����������
 * ���������������� � ������ �������� �� �������� "��� ����",
 * ��� �������� � ������� ������ ����, ���� ���������� � ���������������.
 * ��. �������� ��� ��������� ���������� �� ������������ ����������� �� ������������� ����� �
 * ������������ � ������ ��������.
 *
 */

#ifndef _ANDROID_NATIVE_APP_GLUE_H
#define _ANDROID_NATIVE_APP_GLUE_H

#include <poll.h>
#include <pthread.h>
#include <sched.h>

#include <android/configuration.h>
#include <android/looper.h>
#include <android/native_activity.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ��������� NativeActivity, ��������������� <android/native_activity.h>,
 * ������� �� ������ ��������������� ����������� �������� �������, ������� ����������
 *�������� ������� �������� ��� ������������� ������������ �������.
 *
 * ��� ��������, ��� �� ���� �� ������ �������� ������� _��_ _������_ �������������, � ��������� ������
 * ���������� ���� ��������������� �������� ���������� ��������. ��� ������ ����������������
 * ������, �������, �� ����� �����������.
 *
 * ����������� ���������� threaded_native_app ������������ ��� ����������� ������
 * ������ ����������, � ������� ���������� ����� ����������� ���� ����������� ���� �������� �������
 * � ������ ������ ������ �����. ��� �������� ���:
 *
 * 1/ ���������� ������ ������������ ������� � ������ android_main(), �������
 *    ����� ���������� ��� �������� �������� � ����� ������,
 *    ������������ �� ��������� ������ ��������.
 *
 * 2/ android_main() �������� ��������� �� ���������� ��������� android_app,
 *   ������� �������� ������ �� ������ ������ �������, �������� ��������� �������
 *    ANativeActivity, ��� ����������� ����������.
 *
 * 3/ ������ android_app �������� ��������� ALooper, ������� ���
 *    ������� ��� ������ ����:
 *
 *      - ������� ���������� ����� �������� (��������, "pause", "resume"). ��. ���������� APP_CMD_XXX
 * ����.
 *
 *      - ������� �������, ����������� �� ������� AInputQueue, �������������� � ��������.
 *
 *    ������ �� ���� ������� ������������� �������������� ALooper, ������������� 
 *    ALooper_pollOnce �� ���������� LOOPER_ID_MAIN � LOOPER_ID_INPUT,
 *, ��������������.
 *
 *    ���� ���������� ����� ������������ ��� �� ALooper ��� ������������� ��������������
 *    ������������ �����. ��� ����� ���� �������� ���� �� �������� �������, ���� ��������� � ���������������� ��������,
 * ������������� �  LOOPER_ID_USER.
 *
 * 4/ ��� ��������� ������� LOOPER_ID_MAIN ��� LOOPER_ID_INPUT 
 *   ������������ ������ ����� ��������� �� ��������� android_poll_source.  ��� ���
 *    ����� ������� ������� process() � ��������� android_app->onAppCmd
 *    � android_app->onInputEvent, ��� ���� ����� ��� ���������� ���  ����� ����������� ���������
 *    �������.
 *
 *    ������ ����� ����� ������� ������� ������� ������ ��� ������ � ���������
 *    ������ ���������������...  ���������� �� ���������� process_cmd() � process_input()
 *    � ������������, ����� ��������, ��� ��� ��������.
 *
 * ��. ������ "native-activity" � NDK �
 * ������ ������������� �������������. ����� ���������� JavaDoc � NativeActivity.
 */

struct android_app;

/**
 * ������, ��������� � ALooper fd, ������� ����� ������������ ��� outData
 * ��� ���������� ������ � ���� ���������.
 */
struct android_poll_source {
    // ������������� ������� ���������. ����� ���� LOOPER_ID_MAIN ���
    // LOOPER_ID_INPUT.
    int32_t id;

    // android_app, � ������� ������ ������ �������������.
    struct android_app* app;

    // �������, ���������� ��� ����������� ��������� ������ ��
    // ����� ���������.
    void (*process)(struct android_app* app, struct android_poll_source* source);
};

/**
 * ��� ��������� ������������ ���� ������������ ���������
 * ����������. � ���� ������ ��� ���������� �����������
 * � ����� ����������� ������, ��������� �� ��������� ������ ��������.
 * �� ��������� ����� ������� ������ � �� Java
 *, ���� ��� ���������� ��� ���������� ������� JNI �����
 * �������� Java.
 */
struct android_app {
    // ���������� ����� ��������� ��������� �� ���� ����������� ������ ���������
    // �����, ���� �����.
    void* userData;

    // ������� ����� ��� ������� ��� ��������� �������� ������ ���������� (APP_CMD_*)
    void (*onAppCmd)(struct android_app* app, int32_t cmd);

    // ������� ����� ��� ������� ��� ��������� ������� �������. � ���� ������
    // ������� ��� ���� �������������� ���������� � ����� ��������� ���
    // ��������. ������� 1, ���� ������� ����������, 0 � ��� ����� ���������������
    // �� ���������.
    int32_t (*onInputEvent)(struct android_app* app, AInputEvent* event);

    // ��������� ������� ANativeActivity, � ������� ����������� ��� ����������.
    ANativeActivity* activity;

    // ������� ������������, � ������� ����������� ��� ����������.
    AConfiguration* config;

    // ��� ��������� ����������� ��������� ����������, ��������������� �� ����� ��������.
    // �������� ����� NULL, ���� ��������� �� ����. ����� ������������ ��� �� ���� �������������;
    // ������ ��������� ��������� �� ������ android_app_exec_cmd() ���
    // APP_CMD_RESUME, ����� ���� ��� ����� �����������, � savedState ������� �������� NULL.
    // ��� ���������� ���������� �������� ������ ��� ��������� APP_CMD_SAVE_STATE,
    // ����� �� �������� ����� ������������������ � NULL � ����� ����� ��������� malloc ���
    // ��������� � ��������� ����� ����������. � ���� ������ ������ �����
    // ����������� �������.
    void* savedState;
    size_t savedStateSize;

    // ALooper, ��������� � ������� ����������.
    ALooper* looper;

    // ���� �������� �� ����� NULL, �� ��� ������� �������, �� ������� ���������� �����
    // �������� ������� ������� ������������.
    AInputQueue* inputQueue;

    // ���� �������� �� ����� NULL, �� ��� ����������� ����, � ������� ���������� ����� ��������.
    ANativeWindow* window;

    // ������� ������������� ����������� ����. ��� �������, � �������
    // ������ ���������� ���������� ����, ����� ��� ����� ������������.
    ARect contentRect;

    // ������� ��������� �������� ����������. ����� ���� APP_CMD_START,
    // APP_CMD_RESUME, APP_CMD_PAUSE ��� APP_CMD_STOP; ��. ����.
    int activityState;

    // �������� �� ����� ����, ����� NativeActivity ����������
    // ����������� � ������� ���������� ������ ����������.
    int destroyRequested;

    // -------------------------------------------------
    // ���� ������� "�������" ���������� ���� ����������.

    pthread_mutex_t mutex;
    pthread_cond_t cond;

    int msgread;
    int msgwrite;

    pthread_t thread;

    struct android_poll_source cmdPollSource;
    struct android_poll_source inputPollSource;

    int running;
    int stateSaved;
    int destroyed;
    int redrawNeeded;
    AInputQueue* pendingInputQueue;
    ANativeWindow* pendingWindow;
    ARect pendingContentRect;
};

enum {
    /**
     * ������������� ������ Looper ������, ����������� �� ��������� ������ ����������, �������
     * ������������ ��� ������������� �� ALooper_pollOnce().  ������ ��� ����� ��������������
     * �������� ���������� �� ��������� android_poll_source.
     * �� ����� ������� � ���������� � ������� android_app_read_cmd()
     * � android_app_exec_cmd().
     */
    LOOPER_ID_MAIN = 1,

    /**
     * ������������� ������ Looper �������, ����������� �� AInputQueue ����
     * ����������, ������� ������������ ��� ������������� ��
     * ALooper_pollOnce().  ������ ����� �������������� �������� ���������� �� ���������
     * android_poll_source. �� ����� ��������� ����� ������ inputQueue
     * ���������� android_app.
     */
    LOOPER_ID_INPUT = 2,

    /**
     * ������ ������������ ������������� ��������������� ALooper.
     */
    LOOPER_ID_USER = 3,
};

enum {
    /**
     * ������� �� ��������� ������: AInputQueue ��������.  ����� ���������
     * ���� ������� android_app->inputQueue ����� ��������� � ����� �������
     * (��� NULL).
     */
    APP_CMD_INPUT_CHANGED,

    /**
     * ������� �� ��������� ������: ����� ANativeWindow ������ � �������������.  �����
     * ��������� ���� ������� ���� android_app-> ����� ��������� ����� ����������� ����
     *.
     */
    APP_CMD_INIT_WINDOW,

    /**
     * ������� �� ��������� ������: ������������ ANativeWindow ����������
     * ����������. ����� ��������� ���� ������� ���� android_app->��-��������
     * �������� ������������ ����; ����� ������ android_app_exec_cmd
     * ��� ������� �������� NULL.
     */
    APP_CMD_TERM_WINDOW,

    /**
     * ������� �� ��������� ������: ������� ANativeWindow �������� ������.
     * ����������� �������� ����� �������.
     */
    APP_CMD_WINDOW_RESIZED,

    /**
     * ������� �� ��������� ������: ������� ����������, ����� ������� ANativeWindow
     * ���� ������������. ���������� ������������ ���� ����� �� ��������� �
     * android_app_exec_cmd(), ����� �������� ���������� ����� ���������.
     */
    APP_CMD_WINDOW_REDRAW_NEEDED,

    /**
     * ������� �� ��������� ������: ������� ����������� ���� ��������
     * ����� �������, ��� �� ��������������� ����� ���� ������������ ��� ����������.  �����
     * ����� ����� ������������� ����������� � android_app::contentRect.
     */
    APP_CMD_CONTENT_RECT_CHANGED,

    /**
     * ������� �� ��������� ������: ���� �������� ���������� ��������
     * ����� �����.
     */
    APP_CMD_GAINED_FOCUS,

    /**
     * ������� �� ��������� ������: ���� �������� ���������� ��������
     * ����� �����.
     */
    APP_CMD_LOST_FOCUS,

    /**
     * ������� �� ��������� ������: �������� �������  ������������ ����������.
     */
    APP_CMD_CONFIG_CHANGED,

    /**
     * ������� �� ��������� ������: ������� �� ������� ������.
     * ���������� ��������� ������������� ������.
     */
    APP_CMD_LOW_MEMORY,

    /**
     * ������� �� ��������� ������: �������� ���������� ���� ��������.
     */
    APP_CMD_START,

    /**
     * ������� �� ��������� ������: �������� ���������� ���� ������������.
     */
    APP_CMD_RESUME,

    /**
     * ������� �� ��������� ������: ���������� ������ ������� ����� ����������� ���������
     * ��� ����, ����� ����������������� �� ���� ������� � ������ �������������. ���� �� ��������� ���������,
     * �������� ��� � �������������� malloc � ��������� � android_app.savedState �
     * �������� android_app.savedStateSize.  ������ ����� �����������
     * �������.
     */
    APP_CMD_SAVE_STATE,

    /**
     * ������� �� ��������� ������: ����� � �������� ����������.
     */
    APP_CMD_PAUSE,

    /**
     * ������� �� ��������� ������: �������� ���������� ���� �����������.
     */
    APP_CMD_STOP,

    /**
     * ������� �� ��������� ������: �������� ���������� ������������,
     * � �������  ������� ������ ���������� � ������ ����� ����������.
     */
    APP_CMD_DESTROY,
};

/**
 * ��������, ����� ALooper_pollAll() ���������� LOOPER_ID_MAIN, ��� ������ ���������� ��������� �������
 *����������.
 */
int8_t android_app_read_cmd(struct android_app* android_app);

/**
 * �������� � ������� �������, ������������ android_app_read_cmd() ��� ����������
 * ��������� ��������������� ��������� ������ �������. ����� ��������� �����������
 * �������� ��� ������� ����� ������ ���� �������.
 */
void android_app_pre_exec_cmd(struct android_app* android_app, int8_t cmd);

/**
 * �������� � ������� �������, ������������ android_app_read_cmd(), ���
 * ������������� ��������������� ��������� ������ �������.  ���������� ��������� �����������
 * �������� � �������� �� ������ ���� �������.
 */
void android_app_post_exec_cmd(struct android_app* android_app, int8_t cmd);

/**
 * ��� �������, ������� ������ ����������� ��� ����������, ������������ �����
 * ������� ���� � ����������.
 */
extern void android_main(struct android_app* app);

#ifdef __cplusplus
}
#endif

#endif /* _ANDROID_NATIVE_APP_GLUE_H */
