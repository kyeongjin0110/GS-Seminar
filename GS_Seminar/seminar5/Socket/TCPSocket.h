#pragma once

#define WIN32_LEAN_AND_MEAN
#include <memory>
#include <stdint.h>
#include <Windows.h>
#include "SocketAddress.h"
#include "SocketUtil.h"


// TCP ����� ������ ������ �� �ִ�.
// packet -> ��ȭ ����
// socket -> ��ȭ��
// ip address -> �ּ�

// TCP ����� ������ ���� �������� �̷������
//
//	   Server							   Client
//	1. socket (��ȭ��) 					1. socket (��ȭ��)
//	2. bind	(��ȭ�⿡ �ּ� ���)			
//	3. listen (���̺� ���)				
//	4. accept (������ ��ٸ�)		 ->		5. connect (��ȭ�� �Ǵ�)
//	6. send/recv (��ȭ)			<->		6. send/recv (��ȭ)
//	7. close							7. close



// Tcp ����� ������ �����ϴ� ������� ����ȴ�.
// ������ �����Ѵٴ� ���� client�� ���� ������,
// �׿� �°� ���ο� socket�� �����־�� �Ѵٴ� ���� �ǹ��Ѵ�. (���� ��ȭ��� �����غ��� �׷���.)

// �� ������ �����ϰ� �Ϸ���, �� ���� ��û�� ����ϴ� �ϳ��� ������ socket�� �����,
// ������ socket�� ���� ������ ���� ������ ���ο� client�� ���� socket���� �����,
// ���ο� client���� ��Ÿ��� ����ϰ� �� �־�� �Ѵ�.  

// ������ ���� ���� ���۹���� ������ socket�� ���۹�İ��� �ٸ���.
// �׷��� ������ ���� listen �Լ��� ȣ���Ͽ� socket�� ���� ��� ���� �ٲپ� �ش�.
// (�����δ� three-stage handshake �� �����ϵ��� socket�� �����Ѵ�.)

// �� �� server ������ accept�� ���� �� ������ ��ٸ���,
// client�� connect�� ���� �� ������ ��û�Ѵ�.

// accept�� �����û�� ���� ������ block ���¿� ����,
// ������ ���������� �̷������ �� socket�� ���� client�� ����ϰ� �Ѵ�.




// (�߿�)
// ���⼭ ¤�� �Ѿ�� �� �κ��� �ִ�.
// listen�� ���ؼ� socket�� ���� ��� ���� ���ٰ� �߾���.
// ���⼭ �� ������ socket�� ����ϴ� port�� ó�� �ּҸ� ����� �� ���� ��ȣ�ϰ��̴�.
// �׷��ٸ�, accept�Լ��� ���� ��������� ���ο� socket���� � port�� ����ϴ°�?

// ��������� �����ڸ�, ���� port ��ȣ�� ����Ѵ�. 
// īī���� ������ ���� ����.
// ���� �� client�� ������ ������ �� port�� �Ҵ��ؾ߸� �Ѵٸ�, 
// īī���� ������ ������ client�� ���鸸 ���� ���� ���ε�,
// ��ǻ�Ͱ� �Ҵ��� �� �ִ� port ������ 65535�̱� ������ ���� ��ü�� �Ұ���������.

// �׷��ٸ� tcp ��ſ����� ���� port�� ���ؼ� data�� �������ִ� ��Ŀ������ �ʿ��� ���̴�.
// �����δ� ������ packet�� header�� ���� ip, port(client�� ip, port)�� ���� ���� port�� ������ data���� �����ϰ�,
// session �������� �̸� �� socket���� �й��� �ֵ��� �����Ǿ� �ִ�.

// ��������� tcp ��� �������� n���� client�� server�� ����ȴٸ� n + 1���� socket�� 1���� port�� ������ �ȴ�.
// (���� ��� �����ϴ��Ŀ� ���� �޶��� ���� �ִ�.)


class TCPSocket
{
public:
	// TCP socket�� ����
	static TCPSocket*				create(SocketUtil::AddressFamily family);
	~TCPSocket();

	// �ּҸ� socket�� �Ҵ��Ѵ�.
	int								bind(const SocketAddress& to_addr);

	// �Է¹��� �ּҷ� ������ ��û�Ѵ�.
	int								connect(const SocketAddress& address);

	// Socket�� ���� ��� ���� �ٲ۴�.
	// ���ķ� �� socket�� accept �Լ��� ����Ͽ� �� ������ ���� �� �ְ� �ȴ�.
	int								listen(int back_num = 32);

	// �� ������ �ޱ� ���� block ���¿� ����.(blocking socket�� ���)
	// ������ ��û�� client���� ������ �������� ��쿡��,
	// �� client�� ����ϴ� socket�� ��ȯ�Ѵ�.
	// �׸��� client�� �ּҸ� from_addr�� ��´�.
	TCPSocket*						accept(SocketAddress& from_addr);

	// Data�� ���� socket�� ����� client���� ������.
	int32_t							send(const void* data, size_t length);

	// Data�� ���� socket�� ����� client�κ��� �޴´�.
	int32_t							receive(void* data, size_t length);

private:

	TCPSocket( SOCKET socket ) : _socket(socket) {}
	SOCKET		_socket;
};

