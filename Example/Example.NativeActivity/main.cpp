/*
* Copyright (C) 2010 The Android Open Source Project
*
* ������������� � ������������ � ��������� Apache License, ������ 2.0 ("��������");
* ���� ���� ����� ������������ ������ � ������������ � ���������.
* ����� �������� ����� �������� �� ���-�����
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* ���� ������ �� ��������� �������� ����������� ���������������� ��� �� ����������� � ���������� ����, ����������� �����������
* ���������������� � ������ �������� �� �������� "��� ����",
* ��� �������� � ������� ������ ����, ���� ���������� ��� ���������������.
* ��. �������� ��� ��������� ���������� �� ������������ ����������� ��� ����� �
* ������������ � ������ ��������.
*
*/

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "AndroidProject1.NativeActivity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "AndroidProject1.NativeActivity", __VA_ARGS__))

/**
* ���� ����������� ������ ���������.
*/
struct saved_state {
	float angle;
	int32_t x;
	int32_t y;
};

/**
* ����� ��������� ������ ����������.
*/
struct engine {
	struct android_app* app;

	ASensorManager* sensorManager;
	const ASensor* accelerometerSensor;
	ASensorEventQueue* sensorEventQueue;

	int animating;
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	int32_t width;
	int32_t height;
	struct saved_state state;
};

/**
* ���������������� �������� EGL ��� �������� �����������.
*/
static int engine_init_display(struct engine* engine) {
	// ���������������� OpenGL ES � EGL

	/*
	* ����� ����������� �������� ������ ������������.
	* ���� �� �������� EGLConfig � �� �����, ��� 8 ������ �� ��������� �����
	*, ����������� � ����� �� ������
	*/
	const EGLint attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_NONE
	};
	EGLint w, h, format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display, 0, 0);

	/* ����� ���������� �������� ������ ��� ������������. � ������
	* ������� � ��� ����� ���������� ������� ������, ����� ����������
	* ������ EGLConfig, ������� ������������� ����� ��������� */
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);

	/* EGL_NATIVE_VISUAL_ID �������� ��������� EGLConfig, ������� ��������������
	* ����������� ANativeWindow_setBuffersGeometry().
	* ��������� �� ������� EGLConfig, ����� ��������� �������� ������������ �������
	* ANativeWindow, ����� ���������� ������������ � ������� EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

	surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
	context = eglCreateContext(display, config, NULL, NULL);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		LOGW("Unable to eglMakeCurrent");
		return -1;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	engine->display = display;
	engine->context = context;
	engine->surface = surface;
	engine->width = w;
	engine->height = h;
	engine->state.angle = 0;

	// ���������������� ��������� GL.
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_DEPTH_TEST);

	return 0;
}

/**
* ������ ������� ����� � �����������.
*/
static void engine_draw_frame(struct engine* engine) {
	if (engine->display == NULL) {
		// ��� �����������.
		return;
	}

	// ������ ��������� ����� ������.
	glClearColor(((float)engine->state.x) / engine->width, engine->state.angle,
		((float)engine->state.y) / engine->height, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	eglSwapBuffers(engine->display, engine->surface);
}

/**
* ������������� �������� EGL, ��������� � ��������� ����� � ������������.
*/
static void engine_term_display(struct engine* engine) {
	if (engine->display != EGL_NO_DISPLAY) {
		eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (engine->context != EGL_NO_CONTEXT) {
			eglDestroyContext(engine->display, engine->context);
		}
		if (engine->surface != EGL_NO_SURFACE) {
			eglDestroySurface(engine->display, engine->surface);
		}
		eglTerminate(engine->display);
	}
	engine->animating = 0;
	engine->display = EGL_NO_DISPLAY;
	engine->context = EGL_NO_CONTEXT;
	engine->surface = EGL_NO_SURFACE;
}

/**
* ���������� ��������� ������� �������.
*/
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
	struct engine* engine = (struct engine*)app->userData;
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
		engine->state.x = AMotionEvent_getX(event, 0);
		engine->state.y = AMotionEvent_getY(event, 0);
		return 1;
	}
	return 0;
}

/**
* ���������� ��������� �������� �������.
*/
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
	struct engine* engine = (struct engine*)app->userData;
	switch (cmd) {
	case APP_CMD_SAVE_STATE:
		// ������� ��������� ��������� ������� ���������. ��������� ���.
		engine->app->savedState = malloc(sizeof(struct saved_state));
		*((struct saved_state*)engine->app->savedState) = engine->state;
		engine->app->savedStateSize = sizeof(struct saved_state);
		break;
	case APP_CMD_INIT_WINDOW:
		// ���� ������������, ���������� ���.
		if (engine->app->window != NULL) {
			engine_init_display(engine);
			engine_draw_frame(engine);
		}
		break;
	case APP_CMD_TERM_WINDOW:
		// ���� ���������� ��� �����������, ������� ���.
		engine_term_display(engine);
		break;
	case APP_CMD_GAINED_FOCUS:
		// ��� ��������� ������ ����������� �� �������� ���������� �������������.
		if (engine->accelerometerSensor != NULL) {
			ASensorEventQueue_enableSensor(engine->sensorEventQueue,
				engine->accelerometerSensor);
			// ���������� �������� 60 ������� � � ������� (� ���).
			ASensorEventQueue_setEventRate(engine->sensorEventQueue,
				engine->accelerometerSensor, (1000L / 60) * 1000);
		}
		break;
	case APP_CMD_LOST_FOCUS:
		// ��� ������ ������ ����������� �� ���������� ���������� �������������.
		// ��� �������� ��������� ����� ������������, ����� �� ������������.
		if (engine->accelerometerSensor != NULL) {
			ASensorEventQueue_disableSensor(engine->sensorEventQueue,
				engine->accelerometerSensor);
		}
		// ����� ���������� ��������.
		engine->animating = 0;
		engine_draw_frame(engine);
		break;
	}
}

/**
* ��� ������� ����� ����� ���������� � �������� ����, ������� ����������
* android_native_app_glue.  ��� ����������� � ����� ����������� ������ �� ����� �����������
* ������ ��������� ������� ��� ��������� ������� ������� � ���������� ������ ��������.
*/
void android_main(struct android_app* state) {
	struct engine engine;

	memset(&engine, 0, sizeof(engine));
	state->userData = &engine;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;
	engine.app = state;

	// ����������� ���������� �������������
	engine.sensorManager = ASensorManager_getInstance();
	engine.accelerometerSensor = ASensorManager_getDefaultSensor(engine.sensorManager,
		ASENSOR_TYPE_ACCELEROMETER);
	engine.sensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager,
		state->looper, LOOPER_ID_USER, NULL, NULL);

	if (state->savedState != NULL) {
		// �������� � ����������� ������������ ���������; ��������������� �� ����.
		engine.state = *(struct saved_state*)state->savedState;
	}

	engine.animating = 1;

	// ���� ������� ���������� ��������.

	while (1) {
		// ������ ���� ���������� �������.
		int ident;
		int events;
		struct android_poll_source* source;

		// ��� �������� �� ����������� �������� �������� �������.
		// ��� �������� �� ������������� �� ���������� ���� �������, � ����� ����������
		// �������� ��������� ���� ��������.
		while ((ident = ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
			(void**)&source)) >= 0) {

			// ���������� ��� �������.
			if (source != NULL) {
				source->process(state, source);
			}

			// ���� � ������� ���� ������, ���������� �� ������.
			if (ident == LOOPER_ID_USER) {
				if (engine.accelerometerSensor != NULL) {
					ASensorEvent event;
					while (ASensorEventQueue_getEvents(engine.sensorEventQueue,
						&event, 1) > 0) {
						LOGI("accelerometer: x=%f y=%f z=%f",
							event.acceleration.x, event.acceleration.y,
							event.acceleration.z);
					}
				}
			}

			// ���������, ��� ����������� �����.
			if (state->destroyRequested != 0) {
				engine_term_display(&engine);
				return;
			}
		}

		if (engine.animating) {
			// ��������� � ���������. ����� �������� ��������� ���� ��������.
			engine.state.angle += .01f;
			if (engine.state.angle > 1) {
				engine.state.angle = 0;
			}

			// ��������� ������������ ��������� ���������� ������, �������
			// ����� �� ����� ��������� �������������.
			engine_draw_frame(&engine);
		}
	}
}
