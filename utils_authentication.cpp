/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_authentication.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fulloa-s <fulloa-s@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/02 15:06:25 by fulloa-s          #+#    #+#             */
/*   Updated: 2022/03/08 14:05:52 by fulloa-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"
#include "server.hpp"

void    Server::authentication_welcome(Client &_client)
{
	std::string reply;
	
	if( !(_client.get_nick().empty()) && _client.get_pass() == true && !(_client.get_user().empty()))
	{
		_client.set_auth();
		reply = ":myserv 001 " + _client.get_nick() + " :Welcome to the Internet Relay Network " + _client.get_nick() + "!" + _client.get_user() + "@" + _client.ip() + "\r\n";
		send(_client.get_fd(), reply.c_str(), reply.size(), 0);
	}
}

int Server::first_authentication(Client &_client, char *message)
{
	std::string _msg = message, tmp;
    
	if(_client.get_auth() == true)
		return 1;
	if(_msg.find("CAP LS", 0) != std::string::npos)
	{
		send(_client.get_fd(), "CAP * LS :multi-prefix sasl\n", strlen("CAP * LS :multi-prefix sasl\n"), 0);
		return 0;
	}	
	if(_msg.find("CAP REQ :multi-prefix", 0) != std::string::npos)
	{
		send(_client.get_fd(), "CAP * ACK multi-prefix\n", strlen("CAP * ACK multi-prefix\n"), 0);
		return 0;
	}
	return 1;
}
		
		
