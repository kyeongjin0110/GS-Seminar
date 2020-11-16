#pragma once

#define WIN32_LEAN_AND_MEAN
#include <memory>
#include <stdint.h>
#include <Windows.h>
#include "SocketAddress.h"
#include "SocketUtil.h"


// TCP 통신은 편지에 비유할 수 있다.
// packet -> 통화 내용
// socket -> 전화기
// ip address -> 주소

// TCP 통신은 다음과 같은 과정으로 이루어진다
//
//	   Server							   Client
//	1. socket (전화기) 					1. socket (전화기)
//	2. bind	(전화기에 주소 등록)			
//	3. listen (케이블에 등록)				
//	4. accept (수신을 기다림)		 ->		5. connect (전화를 건다)
//	6. send/recv (통화)			<->		6. send/recv (통화)
//	7. close							7. close



// Tcp 통신은 연결을 유지하는 방식으로 진행된다.
// 연결을 유지한다는 것은 client가 새로 들어오면,
// 그에 맞게 새로운 socket을 열어주어야 한다는 것을 의미한다. (실제 전화기로 비유해봐도 그렇다.)

// 그 과정을 가능하게 하려면, 새 연결 요청만 담당하는 하나의 문지기 socket을 만들고,
// 문지기 socket을 통해 연결이 들어올 때마다 새로운 client를 위한 socket들을 만들고,
// 새로운 client와의 통신만을 담당하게 해 주어야 한다.  

// 하지만 위와 같은 동작방식은 기존의 socket의 동작방식과는 다르다.
// 그렇기 때문에 먼저 listen 함수를 호출하여 socket을 수동 대기 모드로 바꾸어 준다.
// (실제로는 three-stage handshake 가 가능하도록 socket을 설정한다.)

// 그 후 server 에서는 accept를 통해 새 연결을 기다리고,
// client는 connect를 통해 새 연결을 요청한다.

// accept는 연결요청이 들어올 때까지 block 상태에 들어가고,
// 연결이 성공적으로 이루어지면 새 socket을 만들어서 client를 담당하게 한다.




// (중요)
// 여기서 짚고 넘어가야 할 부분이 있다.
// listen을 통해서 socket이 수동 대기 모드로 들어갔다고 했었다.
// 여기서 이 문지기 socket이 사용하는 port는 처음 주소를 등록해 줄 때의 번호일것이다.
// 그렇다면, accept함수를 통해 만들어지는 새로운 socket들은 어떤 port를 사용하는가?

// 결론적으로 말하자면, 같은 port 번호를 사용한다. 
// 카카오톡 서버를 예로 들어보자.
// 만약 새 client가 접속할 때마다 새 port를 할당해야만 한다면, 
// 카카오톡 서버에 접속한 client는 수백만 개가 넘을 것인데,
// 컴퓨터가 할당할 수 있는 port 개수는 65535이기 때문에 구현 자체가 불가능해진다.

// 그렇다면 tcp 통신에서는 같은 port를 통해서 data를 구분해주는 메커니즘이 필요할 것이다.
// 실제로는 들어오는 packet의 header에 적힌 ip, port(client의 ip, port)를 통해 같은 port로 들어오는 data들을 구분하고,
// session 계층에서 이를 각 socket으로 분배해 주도록 구현되어 있다.

// 결론적으로 tcp 통신 구조에서 n개의 client가 server에 연결된다면 n + 1개의 socket과 1개의 port가 열리게 된다.
// (물론 어떻게 구현하느냐에 따라 달라질 수도 있다.)


class TCPSocket
{
public:
	// TCP socket의 생성
	static TCPSocket*				create(SocketUtil::AddressFamily family);
	~TCPSocket();

	// 주소를 socket에 할당한다.
	int								bind(const SocketAddress& to_addr);

	// 입력받은 주소로 연결을 요청한다.
	int								connect(const SocketAddress& address);

	// Socket을 수동 대기 모드로 바꾼다.
	// 이후로 이 socket은 accept 함수를 사용하여 새 연결을 받을 수 있게 된다.
	int								listen(int back_num = 32);

	// 새 연결을 받기 위해 block 상태에 들어간다.(blocking socket의 경우)
	// 연결을 요청한 client와의 연결이 성공적일 경우에는,
	// 그 client와 통신하는 socket을 반환한다.
	// 그리고 client의 주소를 from_addr에 담는다.
	TCPSocket*						accept(SocketAddress& from_addr);

	// Data를 현재 socket이 연결된 client에게 보낸다.
	int32_t							send(const void* data, size_t length);

	// Data를 현재 socket이 연결된 client로부터 받는다.
	int32_t							receive(void* data, size_t length);

private:

	TCPSocket( SOCKET socket ) : _socket(socket) {}
	SOCKET		_socket;
};

