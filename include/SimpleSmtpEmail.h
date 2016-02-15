/***********************************************************************
*�����ʼ�ģ��ͷ�ļ�
*���Է����ı��͸�����֧�ֶ������һ���ͣ�
*�ο�http://blog.csdn.net/dragoo1/article/details/29361339
*************************************************************************/
#pragma once
#include <string>
#include <WinSock2.h>
#include <vector>
//#include <afxtempl.h>

using namespace std;

struct sMailInfo { //�ʼ���Ϣ
	string	m_pcUserName;//�û���¼���������
	string	m_pcUserPassWord;//�û���¼���������
	string	m_pcSenderName;//�û�����ʱ��ʾ������
	string	m_pcSender;//�����ߵ������ַ
	string	m_pcReceiver;//�����ߵ������ַ
	string	m_pcTitle;//�������
	string	m_pcBody;//�ʼ��ı�����
	string	m_pcIPAddr;//��������IP
	unsigned short  m_pcPort; // �˿ں�
	string	m_pcIPName;//�����������ƣ�IP�����ƶ�ѡһ������ȡ���ƣ�
	sMailInfo(){ memset(this, 0, sizeof(sMailInfo)); }
};
class SimpleSmtpEmail {
public:
	SimpleSmtpEmail(void);
	~SimpleSmtpEmail(void);

public:
	bool SendMail(sMailInfo &smailInfo);//�����ʼ�����Ҫ�ڷ��͵�ʱ���ʼ���ʼ���Ϣ
	void AddFilePath(string pcFilePath);//��Ӹ����ľ���·���������б���
	void DeleteFilePath(string pcFilePath);//ɾ������·��������еĻ�
	void DeleteAllPath(void);//ɾ��ȫ��������·��

protected:
	void GetFileName(string &fileName, string &filePath);//�Ӹ�����·���л�ȡ�ļ�����
	void Char2Base64(char* pBuff64, char* pSrcBuff, int iLen);//��char����ת����Base64����
	char* base64Encode(char const* origSigned, unsigned origLength);
	bool createSocket(SOCKET &sock);//����socket����
	bool login(SOCKET &sock);//��¼���䣬��Ҫ���з��ʼ�ǰ��׼������
	int GetFileData(string FilePath);//���ļ�·����ȡ��������

	bool SendHead(SOCKET &sock);//�����ʼ�ͷ
	bool SendTextBody(SOCKET &sock);//�����ʼ��ı�����
	bool SendFileBody(SOCKET &sock);//�����ʼ�����
	bool SendEnd(SOCKET &sock);//�����ʼ���β
protected:
	vector<string> m_pcFilePathList;//��¼����·��

	char  m_cSendBuff[4096];//���ͻ�����
	char  m_cReceiveBuff[1024];
	char* m_pcFileBuff;//ָ�򸽼�����
	sMailInfo m_sMailInfo;
};