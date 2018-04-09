#pragma once
#include <atlstr.h>
#include <string>
#include <vector>
#include "Camera.h"

class CImageCard
{
private:
	CImageCard();
	virtual ~CImageCard();
	void FreeMemory();

public:

	//����ģʽ����������������פ�����ض��ڴ���
	static CImageCard* GetIns() { static CImageCard l_ic; return &l_ic;}

	//���ݳ������л��������к�
	const char * get_vendor(int manufactory);

	const char * get_sdk_ver_by_vendor(int _vendor);

	//�ؼ���������������ͼ�������ͼ������ָ�룬 ͼ��ͨ���������������Ϊ���̣������ţ������ӳ�
	int Snap(int & width, int & height, unsigned char **data, int & ch, int manufactory, int camera_index, int delay);

	void Close();

	//ˢ�����
	int RefreshList(int method = 0);

	int get_vendors_num();

	int get_cameras_num_by_vendor(int _vendor);

	//��ȡ��Ӧ������
	const char * get_vendor_name(int manufactory);

	const char * get_camera_info_by_index(int manufactory, int camera_idx);

private:
	int error_code;
	int m_camera_index;
	//����б�
	std::vector<Camera*> m_camera;
};

