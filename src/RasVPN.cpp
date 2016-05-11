#include "stdafx.h"
#include "RasVPN.h"

//LPCTSTR ANYUN_VPN_NAME = TEXT("AnyunVPN");

RasVPN::RasVPN()
	: anyun_vpn_name_("ANYUN VPN")
	, rasConn_(0)
{
}


RasVPN::~RasVPN()
{
	DeleteVPNEntry(anyun_vpn_name_);
}



BOOL RasVPN::connectVPN(const QString& ip, const QString& userName, const QString& password)
{
	DeleteVPNEntry(anyun_vpn_name_);
	if (CreateVPNEntry(anyun_vpn_name_, ip, 0))
		return Dial(anyun_vpn_name_, userName, password);
	return FALSE;
}

BOOL RasVPN::Dial(const QString& entry, const QString& userName, const QString& password) 
{
	
	RASDIALPARAMS rasDialParams;

	//ATLTRACE("WINVER:%d\n", WINVER);
	// Set up the RASDIALPARAMS structure for the entry we
	// want to dial
	memset(&rasDialParams, 0, sizeof(RASDIALPARAMS));

	rasDialParams.dwSize = sizeof(RASDIALPARAMS);
	wsprintf(rasDialParams.szEntryName, entry.toStdWString().c_str());
	wsprintf(rasDialParams.szUserName, userName.toStdWString().c_str());
	wsprintf(rasDialParams.szPassword, password.toStdWString().c_str());

	DWORD ret = RasDial(NULL, NULL, &rasDialParams, 0xFFFFFFFF, 0, &rasConn_);
	if (ret != 0) 
	{
			//int err = GetLastError();
			//MessageBox(NULL, TEXT("Could not connect w/ server"),
			//	TEXT("RAS Dial Error"), MB_ICONERROR|MB_OK);
		QMessageBox::information(NULL, "", QString("Error:%1, WINVER:%2").arg(ret).arg(WINVER));
		return FALSE;
	}
	return TRUE;
}

BOOL RasVPN::CreateVPNEntry(const QString& pszEntryName, const QString& pszServerName, DWORD dwVpnType)
{

	RASENTRY rasEntry;
	memset(&rasEntry, 0, sizeof(rasEntry));

	rasEntry.dwSize = sizeof(rasEntry);
	rasEntry.dwType = RASET_Vpn;
	rasEntry.dwRedialCount = 1;
	rasEntry.dwRedialPause = 60;
	rasEntry.dwfNetProtocols = RASNP_Ip;
	rasEntry.dwEncryptionType = ET_Optional;

	wcscpy(rasEntry.szLocalPhoneNumber, pszServerName.toStdWString().c_str());
	wcscpy(rasEntry.szDeviceType, RASDT_Vpn);
	rasEntry.dwVpnStrategy = VS_PptpFirst;
	rasEntry.dwfOptions = RASEO_RequireMsEncryptedPw | RASEO_RequireDataEncryption | RASEO_ModemLights | RASEO_ShowDialingProgress | RASEO_RemoteDefaultGateway;


	DWORD ret = RasSetEntryProperties(NULL, pszEntryName.toStdWString().c_str(), &rasEntry, rasEntry.dwSize, NULL, 0);
	return ret == ERROR_SUCCESS;
}

BOOL RasVPN::HangUp(const QString& entry)
{
	int index;                 // An integer index
	DWORD dwError,             // Error code from a function call 
		dwRasConnSize,       // Size of RasConn in bytes
		dwNumConnections;    // Number of connections found 
	RASCONN RasConn[20];       // Buffer for connection state data 
	// Assume the maximum number of entries is 20. 
	// Assume no more than 20 connections.
	RasConn[0].dwSize = sizeof (RASCONN);
	dwRasConnSize = 20 * sizeof (RASCONN);
	//返回指向VPN数组的指针 Find all connections.
	if (dwError = RasEnumConnections (RasConn, &dwRasConnSize, &dwNumConnections) || dwNumConnections == 0){
		return FALSE;
	}
	// Terminate all of the remote access connections.
	for (index = 0; index < (int)dwNumConnections; ++index)
	{
		if (wcsstr(RasConn[index].szEntryName, entry.toStdWString().c_str())!=NULL)
		{
			if (dwError = RasHangUp (RasConn[index].hrasconn))
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

int RasVPN::DeleteVPNEntry(const QString& entry)
{
	if(rasConn_)
	{
		::RasHangUp(rasConn_);
		rasConn_ = NULL;
	}
	HangUp(entry);
	DWORD ret =  ::RasDeleteEntry(NULL, entry.toStdWString().c_str());
	return ret == ERROR_SUCCESS;
}
