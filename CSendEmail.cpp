#include "CSendEmail.h"
#include "Base64.h"
#include <string.h>
#include "CRemind.h"
#include "CLog.h"
//we can use VMime or libcurl library to send or recv email

CSendEmail* CSendEmail::sendEmail = NULL;
CSendEmail* CSendEmail::getInstance()
{
	if(sendEmail== NULL)
		sendEmail = new CSendEmail();
	return sendEmail;
}
CSendEmail::CSendEmail()
{
	//clisocket = new CCliSocket();
}
CSendEmail::~CSendEmail()
{
	if(clisocket != NULL)
		delete clisocket;
}

bool CSendEmail::SendEmail(const std::string& msg)
{
	std::string SendEmailName = CRemind::SendEmail.substr(0, CRemind::SendEmail.find("@"));
	std::string SendEmailPwd = "xxxxxxx";
	
	char szBuf[1024];
	memset(szBuf,0,1024);
	char sendstr[3*1024];
	memset(sendstr, 0, 256);

	clisocket = new CCliSocket();
	clisocket->GetHostIp(CRemind::WebAddr.c_str());
	if(clisocket->Connect(CRemind::Port))
		LogPrint->Info("connect email server:%s %d", clisocket->getIp(), CRemind::Port); 
	clisocket->Recv(szBuf, sizeof(szBuf));// printf("\nData received OVER DATA: %s", szBuf);
	// send hello webaddr
    strcpy(sendstr, "HELO ");
	strcat(sendstr, CRemind::WebAddr.c_str());
	strcat(sendstr, " \r\n");

	clisocket->Send(sendstr, strlen(sendstr));
	clisocket->Recv(szBuf, sizeof(szBuf));// printf("\nData received2: %s", szBuf);
	
    //send login info
	clisocket->Send("AUTH LOGIN\r\n", strlen("AUTH LOGIN\r\n"));
	clisocket->Recv(szBuf, sizeof(szBuf));// printf("\nData received LOGIN: %s", szBuf);
	
	//send username & password
	SendEmailName = base64_encode(reinterpret_cast<const unsigned char*>(SendEmailName.c_str()), SendEmailName.length());
	SendEmailPwd = base64_encode(reinterpret_cast<const unsigned char*>(SendEmailPwd.c_str()), SendEmailPwd.length());
	memset(sendstr, 0, sizeof(sendstr));
	strcpy(sendstr, SendEmailName.c_str());
	strcat(sendstr, "\r\n");
	clisocket->Send(sendstr, strlen(sendstr));
	clisocket->Recv(szBuf, sizeof(szBuf));
	// printf("\nData received USERNAME: %s", szBuf);

	memset(sendstr, 0, sizeof(sendstr));
	strcpy(sendstr, SendEmailPwd.c_str());
	strcat(sendstr, "\r\n");
	clisocket->Send(sendstr, strlen(sendstr));	
	clisocket->Recv(szBuf, sizeof(szBuf));
	// printf("\nData received USERPASSWORD: %s", szBuf);
	
	//send send email
    memset(sendstr, 0, sizeof(sendstr));
	strcpy(sendstr, "MAIL FROM: <");
	strcat(sendstr, CRemind::SendEmail.c_str());
	strcat(sendstr, ">\r\n");
	clisocket->Send(sendstr, strlen(sendstr));
	clisocket->Recv(szBuf, strlen(szBuf));
	// printf("\nData received MAILFROM: %s", szBuf);
	
	//send recv email  //RCPT TO: <185081089@qq.com>\r\n
	memset(sendstr, 0, sizeof(sendstr));
	strcpy(sendstr, "RCPT TO: <");
    strcat(sendstr, CRemind::RecvEmail.c_str());
	strcat(sendstr, ">\r\n");
	clisocket->Send(sendstr, strlen(sendstr));
	clisocket->Recv(szBuf, sizeof(szBuf));
	// printf("\nData received TO MAIL: %s", szBuf);
	
	//send ready to recv email
	memset(sendstr, 0, sizeof(sendstr));
	strcpy(sendstr, "DATA\r\n");
	clisocket->Send(sendstr, strlen(sendstr));
	
	//send email title
	memset(sendstr, 0, sizeof(sendstr));
	strcpy(sendstr, "From: ");
	strcat(sendstr, CRemind::SendEmail.c_str());
	strcat(sendstr, "\r\nTo: ");
	strcat(sendstr, CRemind::RecvEmail.c_str());
	strcat(sendstr, "\r\nSubject: this is test email\r\n\r\n");
	clisocket->Send(sendstr, strlen(sendstr));
	//send email content
	memset(sendstr, 0, sizeof(sendstr));
	// strcpy(sendstr, "God!!! Lucky, you hit the jackpot!!!");
	strcpy(sendstr, msg.c_str());
	clisocket->Send(sendstr, strlen(sendstr));
	
	//send email over
	clisocket->Send("\r\n.\r\n", strlen("\r\n.\r\n"));
	clisocket->Recv(szBuf, sizeof(szBuf));
	// printf("\nData received OVER DATA: %s", szBuf);
	
	clisocket->Send("QUIT\r\n", strlen("QUIT\r\n"));
	clisocket->Recv(szBuf, sizeof(szBuf));
	// printf("\nData received Quit: %s", szBuf);
	//clisocket->Close();
	return true;
}
