#include "SimpleSmtpEmail.h"
#include <iostream>

int main() {
	SimpleSmtpEmail mail;
	sMailInfo info;
	info.m_pcIPAddr = "";
	info.m_pcIPName = "smtp.qq.com";
	info.m_pcReceiver = "349472362@qq.com";
	info.m_pcSender = "349472362@qq.com";
	info.m_pcUserName = "349472362@qq.com";
	info.m_pcUserPassWord = "xxxx";
	info.m_pcPort = 0;

	info.m_pcSenderName = "shiqiang";
	info.m_pcTitle = "test";
	info.m_pcBody = "test simple email relealized via C++.";
//	mail.AddFilePath("e:\\3.zip");
	mail.SendMail(info);

	std::cout << "Run End..." << std::endl;
	return 0;
}