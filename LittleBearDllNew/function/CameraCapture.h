//#define CAMERA_VIDEO_FILE_NAME		"CameraVideo.avi"
//#define CAMERA_PHOTO_FILE_NAME		"CameraPhoto.jpg"

#pragma once
#ifndef CAMERACAPTURE_H_H_H
#define CAMERACAPTURE_H_H_H

#include <windows.h>

int __stdcall CameraPhotoCapture();

int __stdcall CameraVideoCapture(unsigned long DelayTime);

int WaitCameraFree(unsigned long* pFlag);

int SendCameraFile(char* szFileName, int cmd);

#endif