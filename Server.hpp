/*
 *  This file is part of ICon4. Please see README for details.
 *  Copyright (C) 2021 Marek Zalewski aka Drwalin
 *
 *  ICon4 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ICon4 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <set>

#include "ASIO.hpp"
#include "Socket.hpp"

namespace boost {
	namespace asio {
		namespace ip {
#ifndef BOOST_ASIO_IP_TCP_HPP
			namespace tcp {
				class acceptor;
			}
#endif
		}
		namespace ssl {
#ifndef BOOST_ASIO_SSL_CONTEXT_HPP
			class context;
#endif
		}
	}
}

class Server {
public:
	
	Server();
	~Server();
	
	void Close();
	
	void StartAccepting(Endpoint tcpEndpoint, Endpoint sslEndpoint,
			Endpoint udpEndpoint, const char* certChainFile,
			const char* privateKeyFile, const char* dhFile);
	
	void SendToAllTcp(...);
	void SendToAllSsl(...);
	void SendToAllUdp(...);
	void SendBroadcast(...);
	
	void SetOnReceiveTcp(...);
	void SetOnReceiveSsl(...);
	void SetOnReceiveUdp(...);
	void SetOnReceiveBroadcast(...);
	
	void SetOnError(...);
	
	void SetOnAcceptNewUdp(...);
	void SetOnAcceptNewTcp(...);
	void SetOnAcceptNewSsl(...);
	
	void ConnectUdp(...);
	void ConnectTcp(...);
	void ConnectSsl(...);
	
public:

	boost::asio::ssl::context* sslContext;
	boost::asio::ip::tcp::acceptor* acceptorSsl;
	
	boost::asio::ip::tcp::acceptor* acceptorTcp;
	
	Socket* udpSocket;
	Socket* broadcastSocket;
	
	std::set<Socket*> tcpSockets, sslSockets;
	std::map<Endpoint, void*> endpointUserPointers;
};

#endif

