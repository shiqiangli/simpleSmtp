/************************************************************************
* �����ʼ�ģ��
*���Է����ı��͸�����֧�ֶ������һ���ͣ�
*Date:2016-01-07
************************************************************************/
#include "SimpleSmtpEmail.h"
#include <fstream>
#include <atlenc.h>
#include <sys/stat.h>
#include <sstream>
#include <iostream>
#pragma comment(lib,"WSOCK32")

SimpleSmtpEmail::SimpleSmtpEmail(void)
{
	m_pcFileBuff = "";
	memset(m_cSendBuff, 0, sizeof(m_cSendBuff));
	memset(m_cReceiveBuff, 0, sizeof(m_cReceiveBuff));
}


SimpleSmtpEmail::~SimpleSmtpEmail(void)
{
	DeleteAllPath();
}


void SimpleSmtpEmail::Char2Base64(char * pBuff64, char * pSrcBuff, int iLen)
{
	//1   1   1   1   1   1   1   1
	// �����pBuff64  �� �����pBuff64+1
	//		   point���ڵ�λ��
	static char Base64Encode[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";//base64��ӳ����ַ���
	int point = 2;//ÿһ��Դ�ַ���ֵ�λ�ã���ȡ2,4,6����ʼΪ2
	int i;
	int iIndex;//base64�ַ�������
	char n = 0;//��һ��Դ�ַ��Ĳ���ֵ
	for (i = 0; i<iLen; i++) {
		if (point == 2) {
			iIndex = ((*pSrcBuff) >> point) & 0x3f;//ȡ��pSrcBuff�ĸ�pointλ
		} else if (point == 4) {
			iIndex = ((*pSrcBuff) >> point) & 0xf;//ȡ��pSrcBuff�ĸ�pointλ
		} else if (point == 6) {
			iIndex = ((*pSrcBuff) >> point) & 0x3;//ȡ��pSrcBuff�ĸ�pointλ
		}
		iIndex += n;//��pSrcBuff-1�ĵ�point������Base64������
		*pBuff64++ = Base64Encode[iIndex];//��������õ�pBuff64
		n = ((*pSrcBuff) << (6 - point));//����Դ�ַ��еĲ���ֵ
		n = n & 0x3f;//ȷ��n�������λΪ0
		point += 2;//Դ�ַ��Ĳ��λ������2
		if (point == 8) { //������λ��Ϊ8˵��pSrcBuff��6λ�������������һ��������Base64�ַ�������ֱ�������һ��
			iIndex = (*pSrcBuff) & 0x3f;//��ȡ��6λ�������������
			*pBuff64++ = Base64Encode[iIndex];//
			n = 0;//����ֵΪ0
			point = 2;//���λ����Ϊ2
		}
		pSrcBuff++;
	}
	if (n != 0){
		*pBuff64++ = Base64Encode[n];
	}
	if (iLen % 3 == 2) { //���Դ�ַ������Ȳ���3�ı���Ҫ��'='��ȫ
		*pBuff64 = '=';
	} else if (iLen % 3 == 1) {
		*pBuff64++ = '=';
		*pBuff64 = '=';
	}
}


/*base64���ñ��˵ı���,�������ⲻ���ص㣬�ص�����������ҵ�һ���ȽϺõ��ʼ����Ϳͻ���*/
char* SimpleSmtpEmail::base64Encode(char const* origSigned, unsigned origLength) {
	static char Base64Encode[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";//base64��ӳ����ַ���

	unsigned char const* orig = (unsigned char const*)origSigned; // in case any input bytes have the MSB set
	if (orig == NULL) return NULL;

	unsigned const numOrig24BitValues = origLength / 3;
	bool havePadding = origLength > numOrig24BitValues * 3;
	bool havePadding2 = origLength == numOrig24BitValues * 3 + 2;
	unsigned const numResultBytes = 4 * (numOrig24BitValues + havePadding);
	char* result = new char[numResultBytes + 3]; // allow for trailing '/0'

	// Map each full group of 3 input bytes into 4 output base-64 characters:
	unsigned i;
	for (i = 0; i < numOrig24BitValues; ++i) {
		result[4 * i + 0] = Base64Encode[(orig[3 * i] >> 2) & 0x3F];
		result[4 * i + 1] = Base64Encode[(((orig[3 * i] & 0x3) << 4) | (orig[3 * i + 1] >> 4)) & 0x3F];
		result[4 * i + 2] = Base64Encode[((orig[3 * i + 1] << 2) | (orig[3 * i + 2] >> 6)) & 0x3F];
		result[4 * i + 3] = Base64Encode[orig[3 * i + 2] & 0x3F];
	}

	// Now, take padding into account.  (Note: i == numOrig24BitValues)
	if (havePadding) {
		result[4 * i + 0] = Base64Encode[(orig[3 * i] >> 2) & 0x3F];
		if (havePadding2) {
			result[4 * i + 1] = Base64Encode[(((orig[3 * i] & 0x3) << 4) | (orig[3 * i + 1] >> 4)) & 0x3F];
			result[4 * i + 2] = Base64Encode[(orig[3 * i + 1] << 2) & 0x3F];
		} else {
			result[4 * i + 1] = Base64Encode[((orig[3 * i] & 0x3) << 4) & 0x3F];
			result[4 * i + 2] = '=';
		}
		result[4 * i + 3] = '=';
	}

	result[numResultBytes] = '\0';
	return result;
}


void SimpleSmtpEmail::AddFilePath(string pcFilePath)//��Ӹ���·��
{
	if (pcFilePath == "")
		return;
	vector<string>::iterator findit = find(this->m_pcFilePathList.begin(), this->m_pcFilePathList.end(), pcFilePath);
	if (findit == this->m_pcFilePathList.end()) {
		this->m_pcFilePathList.push_back(pcFilePath);
	}
}

void SimpleSmtpEmail::DeleteFilePath(string pcFilePath)//ɾ������·��
{
	vector<string>::iterator findit = find(this->m_pcFilePathList.begin(), this->m_pcFilePathList.end(), pcFilePath);
	if (findit != this->m_pcFilePathList.end()) {
		this->m_pcFilePathList.erase(findit);
	}
}


void SimpleSmtpEmail::DeleteAllPath(void)
{
	this->m_pcFilePathList.clear();
	//m_pcFilePathList.RemoveAll();
}

int SimpleSmtpEmail::GetFileData(string FilePath)
{
	m_pcFileBuff = NULL;
	if (FilePath == ""){
		return 0;
	}

	// ��ȡ�ļ�����
	struct _stat info;
	_stat(FilePath.c_str(), &info);
	int len = info.st_size;
	m_pcFileBuff = new char[len + 1];
	memset(m_pcFileBuff, 0, len + 1);

	ifstream fin(FilePath, ios::in);
	while (!fin.eof()) {
		fin >> m_pcFileBuff;
	}

	fin.close();

	return len;
}

void SimpleSmtpEmail::GetFileName(string &fileName, string &filePath)
{
	
	//if (filePath == "" || fileName == "")
	if (filePath == "")
		return;
	
	std::size_t found = filePath.find_last_of("\\");
	if (found != string::npos) {
		fileName = filePath.substr(found + 1, filePath.size());
	}
}

bool SimpleSmtpEmail::createSocket(SOCKET &sock)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
		return false;
	if ((LOBYTE(wsaData.wVersion) != 2) || (HIBYTE(wsaData.wVersion) != 2)) {
		WSACleanup();
		return false;
	}
	//cout << WSAGetLastError() << endl;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (sock == INVALID_SOCKET) {
		//cout << "1111" << endl;
		cout << WSAGetLastError() << endl;
		return false;
	}

	sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(sockaddr_in));
	servaddr.sin_family = AF_INET;
	if (m_sMailInfo.m_pcPort == 0) 
		servaddr.sin_port = htons(25);//���ʼ�һ�㶼��25�˿�
	else 
		servaddr.sin_port = htons(m_sMailInfo.m_pcPort);
	if (m_sMailInfo.m_pcIPName == "")
		servaddr.sin_addr.s_addr = inet_addr(m_sMailInfo.m_pcIPAddr.c_str());//ֱ��ʹ��IP��ַ
	else
	{
		struct hostent *hp = gethostbyname(m_sMailInfo.m_pcIPName.c_str());//ʹ������
		servaddr.sin_addr.s_addr = *(int*)(*hp->h_addr_list);
		char* ip = inet_ntoa(servaddr.sin_addr);
		string temp(ip);
		m_sMailInfo.m_pcIPAddr = temp;
	}

	int ret = connect(sock, (sockaddr*)&servaddr, sizeof(servaddr));//��������
	if (ret == SOCKET_ERROR) {
		//cout << "2222222:" << ret << endl;
		return false;
	}
	//cout << "ret=" << ret << endl;

	return true;
}


bool SimpleSmtpEmail::login(SOCKET &sock) {

	int ret = recv(sock, m_cReceiveBuff, 1024, 0);

	memset(m_cSendBuff, 0, sizeof(m_cSendBuff));
	sprintf_s(m_cSendBuff, "HELO %s\r\n", m_sMailInfo.m_pcIPAddr.c_str());
	ret = send(sock, m_cSendBuff, (int)strlen(m_cSendBuff), 0);//��ʼ�Ự  
	memset(m_cReceiveBuff, 0, sizeof(m_cReceiveBuff));
	ret = recv(sock, m_cReceiveBuff, 1024, 0);
	if (m_cReceiveBuff[0] != '2' || m_cReceiveBuff[1] != '5' || m_cReceiveBuff[2] != '0'){
		std::cout << "login(): session begin failure!" << std::endl;
		return false;
	}

	memset(m_cSendBuff, 0, sizeof(m_cSendBuff));
	sprintf_s(m_cSendBuff, "AUTH LOGIN\r\n");
	ret = send(sock, m_cSendBuff, (int)strlen(m_cSendBuff), 0);//�����¼  
	ret = recv(sock, m_cReceiveBuff, 1024, 0);
	if (m_cReceiveBuff[0] != '3' || m_cReceiveBuff[1] != '3' || m_cReceiveBuff[2] != '4') {
		std::cout << "login(): ask login failure!" << std::endl;
		return false;
	}

	memset(m_cSendBuff, 0, sizeof(m_cSendBuff));
	char *c = new char[m_sMailInfo.m_pcUserName.length() + 1];
	strcpy_s(c, m_sMailInfo.m_pcUserName.length() + 1, m_sMailInfo.m_pcUserName.c_str());
	Char2Base64(m_cSendBuff, c, (int)strlen(c));
	m_cSendBuff[strlen(m_cSendBuff)] = '\r';
	m_cSendBuff[strlen(m_cSendBuff)] = '\n';
	ret = send(sock, m_cSendBuff, (int)strlen(m_cSendBuff), 0);//�����û���  
	//char * sendBuf = base64Encode(c, strlen(c));
	//sendBuf[strlen(sendBuf)] = '\r';
	//sendBuf[strlen(sendBuf)] = '\n';
	//ret = send(sock, sendBuf, (int)strlen(sendBuf), 0);
	ret = recv(sock, m_cReceiveBuff, 1024, 0);
	if (m_cReceiveBuff[0] != '3' || m_cReceiveBuff[1] != '3' || m_cReceiveBuff[2] != '4') {
		std::cout << "login(): send username failure!" << std::endl;
		return false;
	}

	memset(m_cSendBuff, 0, sizeof(m_cSendBuff));
	char *c2 = new char[m_sMailInfo.m_pcUserPassWord.length() + 1];
	strcpy_s(c2, m_sMailInfo.m_pcUserPassWord.length() + 1, m_sMailInfo.m_pcUserPassWord.c_str());
	Char2Base64(m_cSendBuff, c2, (int)strlen(c2));
	m_cSendBuff[strlen(m_cSendBuff)] = '\r';
	m_cSendBuff[strlen(m_cSendBuff)] = '\n';
	ret = send(sock, m_cSendBuff, (int)strlen(m_cSendBuff), 0);//�����û�����  
	//char * sendBuf2 = base64Encode(c2, strlen(c2));
	//sendBuf2[strlen(sendBuf2)] = '\r';
	//sendBuf2[strlen(sendBuf2)] = '\n';
	//ret = send(sock, sendBuf2, (int)strlen(sendBuf2), 0);
	ret = recv(sock, m_cReceiveBuff, 1024, 0);
	if (m_cReceiveBuff[0] != '2' || m_cReceiveBuff[1] != '3' || m_cReceiveBuff[2] != '5') {
		std::cout << "login(): send password failure!" << std::endl;
		return false;
	}
	return true;//��¼�ɹ�
}


bool SimpleSmtpEmail::SendHead(SOCKET &sock) {
	int rt;
	memset(m_cSendBuff, 0, sizeof(m_cSendBuff));
	sprintf_s(m_cSendBuff, "MAIL FROM:<%s>\r\n", m_sMailInfo.m_pcSender.c_str());
	rt = send(sock, m_cSendBuff, (int)strlen(m_cSendBuff), 0);

	if (rt != strlen(m_cSendBuff))
		return false;
	rt = recv(sock, m_cReceiveBuff, 1024, 0);

	memset(m_cSendBuff, 0, sizeof(m_cSendBuff));
	sprintf_s(m_cSendBuff, "RCPT TO:<%s>\r\n", m_sMailInfo.m_pcReceiver.c_str());
	rt = send(sock, m_cSendBuff, (int)strlen(m_cSendBuff), 0);
	if (rt != strlen(m_cSendBuff))
		return false;
	rt = recv(sock, m_cReceiveBuff, 1024, 0);

	memset(m_cSendBuff, 0, sizeof(m_cSendBuff));
	memcpy(m_cSendBuff, "DATA\r\n", (int)strlen("DATA\r\n"));
	rt = send(sock, m_cSendBuff, (int)strlen(m_cSendBuff), 0);
	if (rt != strlen(m_cSendBuff))
		return false;
	rt = recv(sock, m_cReceiveBuff, 1024, 0);

	memset(m_cSendBuff, 0, sizeof(m_cSendBuff));
	sprintf_s(m_cSendBuff, "From:\"%s\"<%s>\r\n", m_sMailInfo.m_pcSenderName.c_str(), m_sMailInfo.m_pcSender.c_str());
	sprintf_s(&m_cSendBuff[strlen(m_cSendBuff)], 150, "To:\"INVT.COM.CN\"<%s>\r\n", m_sMailInfo.m_pcReceiver.c_str());
	sprintf_s(&m_cSendBuff[strlen(m_cSendBuff)], 150, "Subject:%s\r\nMIME-Version: 1.0\r\nContent-Type: multipart/mixed;   boundary=\"INVT\"\r\n\r\n", m_sMailInfo.m_pcTitle.c_str());
	rt = send(sock, m_cSendBuff, (int)strlen(m_cSendBuff), 0);
	if (rt != strlen(m_cSendBuff))
		return false;

	return true;
}

bool SimpleSmtpEmail::SendTextBody(SOCKET &sock)
{
	int rt;
	memset(m_cSendBuff, 0, sizeof(m_cSendBuff));
	sprintf_s(m_cSendBuff, "--INVT\r\nContent-Type: text/plain;\r\n  charset=\"gb2312\"\r\n\r\n%s\r\n\r\n", m_sMailInfo.m_pcBody.c_str());
	rt = send(sock, m_cSendBuff, (int)strlen(m_cSendBuff), 0);
	if (rt != strlen(m_cSendBuff))
		return false;
	else
		return true;
}

bool SimpleSmtpEmail::SendFileBody(SOCKET &sock) {
	int i;
	string filePath, fileName;
	int rt;
	int len;
	int pt = 0;
	int nDestLen = 0;
	for (i = 0; i < (int)m_pcFilePathList.size(); i++) {
		pt = 0;
		fileName = "";
		filePath = m_pcFilePathList[i];
		//memset(fileName, 0, 128);
		//filePath = m_pcFilePathList.GetAt(m_pcFilePathList.FindIndex(i));
		len = GetFileData(filePath);
		GetFileName(fileName, filePath);

		sprintf_s(m_cSendBuff, "--INVT\r\nContent-Type: application/octet-stream;\r\n  name=\"%s\"\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: attachment;\r\n  filename=\"%s\"\r\n\r\n", 
			fileName.c_str(), fileName.c_str());
		rt = send(sock, m_cSendBuff, (int)strlen(m_cSendBuff), 0);
		ifstream ifs(filePath, ios::in | ios::binary);
		if (!ifs.is_open()) {
			cout << "File " << fileName << " open failed!" << endl;
			return false;
		}

		char *rdBuf = new char[4000];
		char *chSendBuff;
		memset(rdBuf, 0, sizeof(rdBuf));
		memset(m_cSendBuff, 0, sizeof(chSendBuff));
		while (ifs.read(rdBuf, 4000)) {
			//cout << ifs.gcount() << endl;
			chSendBuff = base64Encode(rdBuf, 4000);
			//Char2Base64(m_cSendBuff, rdBuf, (int)strlen(rdBuf));
			chSendBuff[strlen(chSendBuff)] = '\r';
			chSendBuff[strlen(chSendBuff)] = '\n';
			rt = send(sock, chSendBuff, (int)strlen(chSendBuff), 0);
			memset(chSendBuff, 0, sizeof(chSendBuff));
		}
		//cout << ifs.gcount() << endl;
		chSendBuff = base64Encode(rdBuf, ifs.gcount());
		//Char2Base64(m_cSendBuff, rdBuf, (int)strlen(rdBuf));
		chSendBuff[strlen(chSendBuff)] = '\r';
		chSendBuff[strlen(chSendBuff)] = '\n';
		rt = send(sock, chSendBuff, (int)strlen(chSendBuff), 0);

		if (rt != strlen(chSendBuff)) {
			cout << "Send file " << fileName << " ERROR!" << endl;
			return false;
		}

	}

	return true;
}

bool SimpleSmtpEmail::SendEnd(SOCKET &sock)
{
	sprintf_s(m_cSendBuff, "--INVT--\r\n.\r\n");
	int rt = send(sock, m_cSendBuff, (int)strlen(m_cSendBuff), 0);

	sprintf_s(m_cSendBuff, "QUIT\r\n");
	rt = send(sock, m_cSendBuff, (int)strlen(m_cSendBuff), 0);
	closesocket(sock);
	WSACleanup();
	return true;
}


bool SimpleSmtpEmail::SendMail(sMailInfo &smailInfo)
{
	m_sMailInfo = smailInfo;
	//memcpy(&m_sMailInfo, &smailInfo, sizeof(smailInfo));
	if (m_sMailInfo.m_pcBody == ""
		//|| m_sMailInfo.m_pcIPAddr == ""
		|| m_sMailInfo.m_pcIPName == ""
		|| m_sMailInfo.m_pcReceiver == ""
		|| m_sMailInfo.m_pcSender == ""
		|| m_sMailInfo.m_pcSenderName == ""
		|| m_sMailInfo.m_pcTitle == ""
		|| m_sMailInfo.m_pcUserName == ""
		|| m_sMailInfo.m_pcUserPassWord == "")
	{
		return false;
	}
	SOCKET sock;
	if (!createSocket(sock))//��������
	{
		std::cout << "createSocket() failure!" << endl;
		return false;
	}

	if (!login(sock))//��¼����
	{
		return false;
	}

	if (!SendHead(sock))//�����ʼ�ͷ
	{
		return false;
	}

	if (!SendTextBody(sock))//�����ʼ��ı�����
	{
		return false;
	}

	if (!SendFileBody(sock))//���͸���
	{
		return false;
	}

	if (!SendEnd(sock))//�����ʼ������ر�sock
	{
		return false;
	}

	return true;
}