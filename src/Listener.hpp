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

#ifndef LISTENER_HPP
#define LISTENER_HPP

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

class Listener {
public:
	
	enum Type {
		TCP,
		SSL,
		Invalid
	};
	
	Listener();
	~Listener();
	
	void Close();
	void StopListening();
	
	void StartAccepting(Endpoint endpoint,
			std::function<bool(Socket*)> function, bool enableHeader,
			boost::system::error_code& err);
	void StartAccepting(Endpoint endpoint, const char* certChainFile,
			const char* privateKeyFile, const char* dhFile,
			std::string password, std::function<bool(Socket*)> function,
			bool enableHeader, boost::system::error_code& err);
	
	void SetOnError(
			std::function<void(const boost::system::error_code&)> function);
	
	inline bool IsValid() const {
		return ptr!=NULL && (type==TCP || type==SSL);
	}
	
	inline bool IsTcp() const { return ptr!=NULL && type==TCP; }
	inline bool IsSsl() const { return ptr!=NULL && type==SSL; }
	
private:
	
	union {
#ifdef GENERIC_LISTENER_HPP
		GenericListenerTcp* tcp;
		GenericListenerSsl* ssl;
#endif
		void* ptr;
	};
	
	Type type;
};

#endif

