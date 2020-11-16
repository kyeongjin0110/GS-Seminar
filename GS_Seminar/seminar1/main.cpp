#include <iostream>
#include "UDP.h"
#include "TCP.h"
using namespace std;

int main(int argc, char * argv[])
{
	if (argc == 2)
	{
		string str = argv[1];
		if (str == "UDPClient")
		{
			_ex1::UDPClient();
			return 0;
		}
		else if (str == "UDPServer")
		{
			_ex1::UDPServer();
			return 0;
		}
		else if (str == "TCPClient")
		{
			_ex2::TCPClient();
			return 0;
		}
		else if (str == "TCPServer")
		{
			_ex2::TCPServer();
			return 0;
		}
	}

	std::cout << "input argument = [\"UDPClient\" , \"UDPServer\", \"TCPClient\", \"TCPServer\"]";

	return 0;
}
