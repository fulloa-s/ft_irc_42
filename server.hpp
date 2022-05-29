/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vspinell <vspinell@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/16 14:46:13 by vspinell          #+#    #+#             */
/*   Updated: 2022/03/19 17:56:52 by vspinell         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
#define SERVER_H

#include "Header.hpp"
#include "client.hpp"
#include "channel.hpp"

struct client_info;

class Server
{
	private:
		
		std::string             					_port;
		std::string             					_password;
		struct addrinfo	       						hints, *res;
		fd_set                  					master;
 		fd_set                  					read_fds;
		std::map<int, Client>   					clients;
		std::map<std::string, Channel>				channels;
		int                     					socketfd, clientfd, fdmax;
		
		//Pointer to functition for parsing the commands

		int	(Server::*checker[13])(Client &client, std::vector<std::string> &ve, int index);
		typedef std::map<int, Client>::iterator					client_iterator;
		typedef std::map<std::string, Channel>      			s_channel_map;
		typedef s_channel_map::iterator      					s_channel_iterator;
		typedef std::map<Client *, Channel::client_info>		channel_map;
		typedef channel_map::iterator							channel_iterator;
		typedef std::vector<Channel *>::iterator				client_ch_iterator;
	
	public:
		
		Server(std::string port, std::string password) : _port(port), _password(password)
		{	
			memset(&hints, 0, sizeof hints);
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_flags = AI_PASSIVE;
			this->checker[0] = &Server::check_nick;
			this->checker[1] = &Server::check_pass;
			this->checker[2] = &Server::check_user;
			this->checker[3] = &Server::join_channel;
			this->checker[4] = &Server::privmsg;
			this->checker[5] = &Server::quit;
			this->checker[6] = &Server::ping;
			this->checker[7] = &Server::part;
			this->checker[8] = &Server::mode;
			this->checker[9] = &Server::kick;
			this->checker[10] = &Server::notice;
			this->checker[11] = &Server::list;
			this->checker[12] = &Server::who;
		};

		~Server()
		{	
			freeaddrinfo(this->res);
			close(this->socketfd);
		}		
	public:

		int					setup();
		int 				start();
		void				acceptConnections();
		void				receiveData(int i);
		void				server_log(Client &_client, char *buf);
		void 				parse_message(Client &_client, char *message);
		int					commands_handler(Client &client, std::vector<std::string> &ve);
		void				ch_info_toclient(Channel &ch, Client &client);
		void				create_channel(std::string ch_name, Client &client, std::string ch_key);
		void				check_channel(std::string ch_name, Client &client,  std::string ch_key = std::string());
		int 				join_channel(Client &client, std::vector<std::string> &ve, int index);
		int 				join_zero(Client &client);
		int					existing_nick(Client &client, std::string new_nick);
		int					check_nick( Client &client, std::vector<std::string> &ve, int index );
		void 				send_message_all_channels(Client &client, std::string reply);
		void				send_message_single_channel(Client &client, Channel &ch, std::string reply);
		int					check_pass( Client &client, std::vector<std::string> &ve, int index );
		int					check_user( Client &client, std::vector<std::string> &ve, int index);
		int					quit(Client &client, std::vector<std::string> &ve, int index );
		int					ping(Client &client, std::vector<std::string> &ve, int index );
		int					privmsg(Client &client, std::vector<std::string> &ve, int index);
		int					who( Client &client, std::vector<std::string> &ve, int index );
		int					send_error(Client &requester, int err, int n_param, ...);
		int					part(Client &client, std::vector<std::string> &ve, int index);
		void				part_sendmsg(Client &client, std::vector<std::string>::iterator it, std::vector<std::string> &ve);
		void    			authentication_welcome(Client &_client);
		int					first_authentication(Client &_client, char *message);
		void    			clean_one_channel(Client *_client, client_ch_iterator ch_todestroy, std::string msg);
		void    			clean_all_channels(Client *_client, std::string msg);
		client_ch_iterator	find_ch_ptr(Client *client, std::string ch_name);
		size_t 				vector_split(std::vector<std::string> &ve, const char &c); // return size of vector
		std::string 		create_msg(std::vector<std::string> &ve, int from);
		int					mode(Client &client, std::vector<std::string> &ve, int index);
		int					notice(Client &client, std::vector<std::string> &ve, int index);
		int					kick(Client &client, std::vector<std::string> &ve, int index);
		bool 				channel_exist(std::string ch_name);
		bool				is_channel_client(Client &client, Channel &chan);
		Client * 			is_channel_client_byname(std::string name, Channel &chan);
		Channel* 			find_channel(std::string ch_name);
		channel_iterator 	find_client_in_channel(Channel &chan, std::string client_name);
		int					parse_mode(Client &client, std::vector<std::string> &ve);
		int					fd_byname(std::string &name);
		int					mode_arg(std::string flags, Client &client, std::string ch_name);
		int					dest_nick(std::string nick, Client &client);
		int					change_permission(std::vector<std::string> &ve, size_t pos, Channel *Ch);
		int					list(Client &client, std::vector<std::string> &ve, int index);
		void				send_message(std::string prefix, std::string modes, std::vector<std::string> names, Channel *Ch);
		void 				kick_clients(Client &client, std::vector<std::string> &client_list, Channel *channel, std::string opt_msg);
		Client*				cl_byname(std::string name);
		int					find_is_Opbyname(std::string &name);
		int 				getfd();
		fd_set&				getfdmaster();
		int					getfdmax();

	};

#endif