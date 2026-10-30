#include "Network.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

class WsaSession
{
private:
	WSADATA m_data;

	friend class Router;
public:
	WsaSession()
	{
		WSADATA data{};
		int rc = WSAStartup(MAKEWORD(2, 2), &data);
		if (rc != 0) throw std::runtime_error("WSAStartup failed: " + std::to_string(rc));
	}
	~WsaSession()
	{
		WSACleanup();
	}
	WsaSession(const WsaSession&) = delete;
	WsaSession& operator=(const WsaSession&) = delete;

};

class NetSocket 
{
private:
	int m_s_ = INVALID_SOCKET;
	sockaddr_in m_addRemote = {};
	friend class Router;
	addrinfo m_Hints{};

public:
	NetSocket() = default;
	explicit NetSocket(SOCKET s) : m_s_(s) {}
	~NetSocket() { reset(); }
	NetSocket(NetSocket&& o) noexcept : m_s_(o.m_s_) { o.m_s_ = INVALID_SOCKET; }
	NetSocket& operator=(NetSocket&& o) noexcept {
		if (this != &o) { reset(); m_s_ = o.m_s_; o.m_s_ = INVALID_SOCKET; }
		return *this;
	}
	NetSocket(const NetSocket&) = delete;
	NetSocket& operator=(const NetSocket&) = delete;

	void reset() { if (m_s_ != INVALID_SOCKET) { closesocket(m_s_); m_s_ = INVALID_SOCKET; } }
	SOCKET get() const { return m_s_; }
	bool valid() const { return m_s_ != INVALID_SOCKET; }
};

class Router :NetSocket, WsaSession
{
private:
	int m_result;
	int m_rc;
	

public:

	Router() = default;
	Router(SOCKET _socket) 
	{
		
	}
	~Router()
	{
		WSACleanup();
	}
	Router(const Router&) = delete;
	Router& operator=(const Router&) = delete;

};