#pragma once

#ifndef uint_8
#define uint_8 unsigned char
#endif

typedef uint_8 uchar;

#ifndef MAX_DEVICE_MUN
#define MAX_DEVICE_MUN 8
#endif

//method的后四位是用来比较相机的排列顺序
#define SORT_METHOD (0xF)

enum SORT_CAMERA
{
	AS_USER_ID = 0, AS_SERIALS, AS_NAME,
};

class Camera
{
public:
	Camera();
	virtual ~Camera();

	virtual int error_info() { return 0; }

	virtual const char * vendor() { return "none"; }

	virtual const char * sdk_version() { return "sdk version none"; }

	virtual const char * CamerasInfo() { return "none"; }

	virtual int device_quantity() = 0;
	//
	virtual int Snap(int &width, int &height, unsigned char **data, int &ch, int camera_index = 0, int delay = 0) { width = 0; height = 0; ch = 1; *data = nullptr; return -1; }

	virtual void set_exposure(double exposure = 2000.0, int camera_index=0) { return; }
	//virtual void Grap,子类必须实现
	virtual int refresh_list() = 0;// { return 0; }

	//子类实现关闭相机
	virtual void close() { return; }
};

