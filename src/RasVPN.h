#pragma once
class RasVPN
{
public:
	RasVPN();
	~RasVPN();
private:
	BOOL connectVPN(const QString& ip, const QString& userName, const QString& password);
	BOOL Dial(const QString& ip, const QString& userName, const QString& password);
public:
	BOOL connectVPN()
	{
		return connectVPN(ip_, userName_, password_);
	}
private:
	HRASCONN rasConn_;
private:
	BOOL CreateVPNEntry(const QString& entry, const QString& serverName, DWORD vpnType);
	BOOL HangUp(const QString& entry);
	int DeleteVPNEntry(const QString& entry);
private:
	const QString anyun_vpn_name_;// ("AnyunVPN");
private:
	QString ip_;
	QString userName_;
	QString password_;

public:
	void setIP(const QString& ip)
	{
		ip_ = ip;
	}

	void setUserName(const QString& userName)
	{
		userName_ = userName;
	}

	void setPassword(const QString& password)
	{
		password_ = password;
	}


};

