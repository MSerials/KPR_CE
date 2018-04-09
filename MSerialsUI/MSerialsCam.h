#pragma once
#define CAMERADLL __declspec(dllexport)

#ifdef _WIN64
#pragma comment(lib,"..\\x64\\Release\\MSerialsCamera.lib")
#else

#endif

//枚举相机,返回相机数量，相当于初始化,参数是提供了相机排序方式，默认为按相机名称排列
CAMERADLL int			enum_cameras(int method = 0);
//获取供相机的应商数量
CAMERADLL int			get_vendors_num();
//获取供相机的SDK信息
CAMERADLL const char*	get_sdk_ver_by_vendor(int _vendor);
//获取供相机的该供应商品牌相机的数量
CAMERADLL int			get_cameras_num_by_vendor(int _vendor);
//获取供应商名字
CAMERADLL const char*	get_vendor_name(int manufactory = 0);
//获取相机信息
CAMERADLL const char*	get_camera_info_by_index(int manufactory = 0, int camera_idx = 0);
//含义，从第manufactory的制造商的第camera_index相机获得图片
CAMERADLL int			Snap(int & width, int & height, unsigned char **data, int & ch, int manufactory = 0, int camera_index = 0, int delay = 0);
//关闭相机，以免内存泄漏
CAMERADLL void			close_device();

