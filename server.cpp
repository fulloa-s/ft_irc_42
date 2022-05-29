/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gneri <gneri@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/07 13:05:48 by gneri             #+#    #+#             */
/*   Updated: 2022/03/08 15:06:00 by gneri            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

int	Server::setup()
{
	FD_ZERO(&this->read_fds);
	FD_ZERO(&this->master);

	if (getaddrinfo(NULL, (this->_port).c_str() , &this->hints, &this->res))
		throw ft_perror("Error: getaddrinfo");
	if ((socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
		throw ft_perror("Error: socket");
	int opt = 1;
	if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1)
		throw ft_perror("setsockopt");
	while(bind(socketfd, res->ai_addr, res->ai_addrlen) == -1)
	{
		std::cout <<"Please wait (PORT)..." << std::endl;
		sleep(15);
	}
	std::cout <<"Port bound!" << std::endl;
	listen(socketfd, 10);
	FD_SET(socketfd, &master);
	fdmax = socketfd;
	return 0;
}

void	Server::acceptConnections()
{
	struct sockaddr_in	client_addr;
	socklen_t			addr_size;
	
	addr_size = sizeof client_addr;
	if ((clientfd = accept(socketfd, (struct sockaddr *)&client_addr, &addr_size)) < 0)
		throw ft_perror("accept");
	else 
	{
		clients.insert(std::make_pair(clientfd, Client(client_addr, clientfd)));
		FD_SET(clientfd, &master);
		if (clientfd > fdmax)
			fdmax = clientfd;
		printf("server: got connection from %s port %d\n",
		inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
	}
}

void	Server::receiveData(int i)
{
	int 			nbytes;
	char 			buf[512];
	std::string		quit_msg;
	
	Client *_client = &clients.find(i)->second;
	if ((nbytes = recv(i, buf, sizeof buf, MSG_PEEK)) <= 0)  //MSG_PEEK non pulisce il buffer
	{
		if (nbytes == 0) 
			printf("selectserver: socket %d hung up\n", i);
		if(FD_ISSET(i, &read_fds))
		{
			quit_msg = _client->client_id() + " QUIT :Quit \r\n";
			clean_all_channels(_client, quit_msg);
			close(i);
    		FD_CLR(i, &master);
			clients.erase(clients.find(_client->get_fd()));
		}
	}
	if (strchr(buf, '\n') == NULL)	// finche non trova invio non chiude la stringa
		return ;
	if ((nbytes = recv(i, buf, sizeof buf, 0)))
	{
		if (buf[0] == '\n')
			return ;	
		server_log(*_client, buf);						
		parse_message(*_client, buf);
		if(_client->get_auth() == false)
			authentication_welcome(*_client);
		bzero(buf, sizeof buf);
	}
}

int	Server::start()
{
	int	i;
	
	std::cout << "OK, LET'S GO!" << std::endl;
	signal(SIGINT, handle_signal);
	while(FD_ISSET(this->socketfd, &this->master))
	{
		read_fds = master;
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1)
		{
			if (errno == EINTR) //se uscito con SEGNALE
				continue ;
			return ft_perror("select");
		}
		for(i = 0; i <= fdmax; i++)
		{
			if (FD_ISSET(i, &read_fds)) 
			{
				if (i == socketfd) //SERVER
					acceptConnections();
	 			else //CLIENT
					receiveData(i);
			}
	 	}
	}
	return 0;
}

void Server::server_log(Client &_client, char *buf)
{
	std::cout << "--------------------------------------------------\n";
	std::cout << "FROM IP	: " << _client.ip() << std::endl;
	std::cout << "FROM PORT	: " << _client.port() << std::endl;
	std::cout << "NICK		: " << _client.get_nick() << std::endl;
	std::cout << "USER		: " << _client.get_user() << std::endl;
	std::cout << "LOGIN		: " << _client.get_auth() << std::endl;
	std::cout << "MSG		: " << buf << std::endl;
	std::cout << "--------------------------------------------------\n";
	for(s_channel_iterator it = channels.begin(); it != channels.end(); it++)
	{
		std::cout << "_______CHANNEL NAME: " << it->first << "_______" << std::endl;
		for(channel_map::iterator ex = (it->second).get_map_begin(); ex != it->second.get_map_end() ; ex++)
		{
			std::cout << "CLIENT NAME: " << ex->first->get_nick() << "  ";
			std::cout << "OPERATOR: " << 	ex->second.is_Op() << std::endl;;
		}
		std::cout << std::endl;
	}
	std::cout << "--------------------------------------------------\n";
	
}
void Server::parse_message(Client &_client, char *message)
{
	std::string 				tmp, _msg = message;
	std::vector<std::string>	msg_vector;
	size_t 						found = 0;
	size_t						pos = 0;
	size_t						next_pos = 0;

	if(!first_authentication(_client, message))
	{
		ft_perror("Failed authentication");
		return ;
	}
	for(size_t r ; (r = _msg.find_first_of('\r', 0)) != std::string::npos ;)
		_msg.erase(r, 1);
	for (size_t i = 0; (found = _msg.find('\n', i)) != std::string::npos; i = found + 1)
    {
        tmp = _msg.substr(i, found - i);
        msg_vector.clear();
		
		pos = 0, next_pos = 0;
		while((pos = tmp.find(' ', pos)) != std::string::npos)
		{
			if (tmp[next_pos] == ':' && next_pos != 0)
				break ;
			if (!tmp.substr(next_pos, pos-next_pos).empty())
				msg_vector.push_back(tmp.substr(next_pos, pos-next_pos));
		    next_pos = ++pos;
		}
		if (!tmp.substr(next_pos, tmp.length()).empty())
			msg_vector.push_back(tmp.substr(next_pos, tmp.length()));
		commands_handler(_client, msg_vector);
    }
}

int Server::getfd()
{
	return this->socketfd;
}

fd_set&	Server::getfdmaster()
{
	return this->master;
}

int		Server::getfdmax()
{
	return this->fdmax;
}