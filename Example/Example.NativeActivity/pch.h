//
// pch.h
// ��������� ����������� ��������� ���������� ������.
//
// ������������ �������� ������ ��� �������� �������������������� ���������. �������� ��������, ���
// pch.cpp �� �����, � pch.h ������������� ���������� � ������ ���� ������ cpp,
// ������� �������� ������ �������
//

#include <jni.h>
#include <errno.h>

#include <string.h>
#include <unistd.h>
#include <sys/resource.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>

#include <android/log.h>
#include "android_native_app_glue.h"
