/***********************************************************************
*发送邮件模块头文件
*可以发送文本和附件（支持多个附件一起发送）
*参考http://blog.csdn.net/dragoo1/article/details/29361339
*************************************************************************/
#pragma once
#include <string>
#include <WinSock2.h>
#include <vector>
//#include <afxtempl.h>

using namespace std;

struct sMailInfo { //邮件信息
	string	m_pcUserName;//用户登录邮箱的名称
	string	m_pcUserPassWord;//用户登录邮箱的密码
	string	m_pcSenderName;//用户发送时显示的名称
	string	m_pcSender;//发送者的邮箱地址
	string	m_pcReceiver;//接收者的邮箱地址
	string	m_pcTitle;//邮箱标题
	string	m_pcBody;//邮件文本正文
	string	m_pcIPAddr;//服务器的IP
	unsigned short  m_pcPort; // 端口号
	string	m_pcIPName;//服务器的名称（IP与名称二选一，优先取名称）
	sMailInfo(){ memset(this, 0, sizeof(sMailInfo)); }
};
class SimpleSmtpEmail {
public:
	SimpleSmtpEmail(void);
	~SimpleSmtpEmail(void);

public:
	bool SendMail(sMailInfo &smailInfo);//发送邮件，需要在发送的时候初始化邮件信息
	void AddFilePath(string pcFilePath);//添加附件的决定路径到附件列表中
	void DeleteFilePath(string pcFilePath);//删除附件路径，如果有的话
	void DeleteAllPath(void);//删除全部附件的路径

protected:
	void GetFileName(string &fileName, string &filePath);//从附件的路径中获取文件名称
	void Char2Base64(char* pBuff64, char* pSrcBuff, int iLen);//把char类型转换成Base64类型
	char* base64Encode(char const* origSigned, unsigned origLength);
	bool createSocket(SOCKET &sock);//建立socket连接
	bool login(SOCKET &sock);//登录邮箱，主要进行发邮件前的准备工作
	int GetFileData(string FilePath);//由文件路径获取附件内容

	bool SendHead(SOCKET &sock);//发送邮件头
	bool SendTextBody(SOCKET &sock);//发送邮件文本正文
	bool SendFileBody(SOCKET &sock);//发送邮件附件
	bool SendEnd(SOCKET &sock);//发送邮件结尾
protected:
	vector<string> m_pcFilePathList;//记录附件路径

	char  m_cSendBuff[4096];//发送缓冲区
	char  m_cReceiveBuff[1024];
	char* m_pcFileBuff;//指向附件内容
	sMailInfo m_sMailInfo;
};