// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� MSERIALSKERNEL_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// MSERIALSKERNEL_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef MSERIALSKERNEL_EXPORTS
#define MSERIALSKERNEL_API __declspec(dllexport)
#else
#define MSERIALSKERNEL_API __declspec(dllimport)
#endif

// �����Ǵ� MSerialsKernel.dll ������
class MSERIALSKERNEL_API CMSerialsKernel {
public:
	CMSerialsKernel(void);
	// TODO:  �ڴ�������ķ�����
};

extern MSERIALSKERNEL_API int nMSerialsKernel;

MSERIALSKERNEL_API int fnMSerialsKernel(void);
