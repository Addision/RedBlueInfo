#ifndef __SEND_EMAIL_H
#define __SEND_EMAIL_H

#include <string>
#include "CSocket.h"

class CSendEmail
{
public:
    CSendEmail();
    virtual ~CSendEmail();
	bool SendEmail(const std::string& msg);
	static CSendEmail* getInstance();
private:
	static CSendEmail* sendEmail;
	CSendEmail(const CSendEmail&);
	CSendEmail& operator=(const CSendEmail&);
	CCliSocket* clisocket;
};

#endif //__SEND_EMAIL_H
