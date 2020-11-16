#include "SocketAddress.h"
#include "SocketUtil.h"
#include <sstream>
#include <iostream>
using namespace std;

// SocketAddress�� ����� ���忡�� ���� ���� ���ڿ��� ���� ������Ű�� �Լ�
SocketAddress* SocketAddress::createFromString(const string& addr)
{
	auto pos = addr.find_last_of(':');
	string host, service;
	if (pos != string::npos)
	{
		host = addr.substr(0, pos);
		service = addr.substr(pos + 1);
	}
	else
	{
		host = addr;
		//use default port...
		service = "0";
	}
	addrinfo hint;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;

	addrinfo* result;
	int error = getaddrinfo(host.c_str(), service.c_str(), &hint, &result);
	if (error != 0 && result != nullptr)
	{
		SocketUtil::reportError("SocketAddressFactory::CreateIPv4FromString");
		return nullptr;
	}

	while (!result->ai_addr && result->ai_next)
	{
		result = result->ai_next;
	}

	if (!result->ai_addr)
	{
		return nullptr;
	}

	auto ret = new SocketAddress(*result->ai_addr);

	freeaddrinfo(result);

	return ret;
}


// SocketAddress�� � ������ ����ִ��� ����� ���忡�� ���ϵ��� ���ڿ��� ����� ��ȯ
std::string	SocketAddress::toString() const
{
#if _WIN32
	const sockaddr_in* s = asSockAddrIn();
	char destinationBuffer[128];
	InetNtop(s->sin_family, const_cast< in_addr* >(&s->sin_addr), destinationBuffer, sizeof(destinationBuffer));

	std::stringstream ss;
	ss << destinationBuffer << ":" << ntohs(s->sin_port);
	return ss.str();
#else
	//not implement on mac for now...
	return string("not implemented on mac for now");
#endif
}