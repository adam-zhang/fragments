bool Network::ping(const char* address, int* time)
{
	assert(address && strlen(address) > 0);
	HANDLE hIcmpFile;
    unsigned long ipaddr = INADDR_NONE;
    DWORD dwRetVal = 0;
    char SendData[32] = "Data Buffer";
    LPVOID ReplyBuffer = NULL;
    DWORD ReplySize = 0;
    

    ipaddr = inet_addr(address);
    if (ipaddr == INADDR_NONE) 
        return false;
    
    hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE) {
        return false;
    }    

    ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
    ReplyBuffer = (VOID*) malloc(ReplySize);
    if (ReplyBuffer == NULL) {
        return false;
    }    
    
    
    dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), 
        NULL, ReplyBuffer, ReplySize, 1000);
    if (dwRetVal != 0) {
        PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
        struct in_addr ReplyAddr;
        ReplyAddr.S_un.S_addr = pEchoReply->Address;
		*time = pEchoReply->RoundTripTime;
    }
    else 
	{
        return false;
    }
	return true;
}
