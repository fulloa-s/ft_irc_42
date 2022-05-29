/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vspinell <vspinell@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/18 11:05:12 by fulloa-s          #+#    #+#             */
/*   Updated: 2022/03/09 18:53:19 by vspinell         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_H
#define CLIENT_H

#include "Header.hpp"
#include "channel.hpp"


class Channel;

class Client
{

	private:

		sockaddr_in									_info;
		std::string									_nickname;
		bool										s_password;
		int											self_fd;
		bool										auth;
		std::string									_user;
		std::vector<Channel *>						channels;
		typedef std::vector<Channel *>::iterator	client_ch_iterator;
	
	public:
		Client();
		Client(sockaddr_in info, const int &fd) : _info(info), s_password(0), self_fd(fd), auth(0) {};
		~Client(){};
				
		const char*			ip() const { return(inet_ntoa(_info.sin_addr)); }
		int					port() const { return (ntohs(_info.sin_port)); }
		std::string			get_nick() const { return this->_nickname; }
		int					get_fd() const { return this->self_fd ;}
		sockaddr_in			get_info() const { return this->_info ;}
		std::string			get_user() const { return this->_user ;}
		bool				get_pass() const { return this->s_password ;}
		bool				get_auth() const { return this->auth ;}
		void				set_nick(const std::string &new_nick) { this->_nickname = new_nick; }
		void				set_pass(){ if(this->s_password == false) this->s_password = true; }
		void				set_auth(){ if(this->auth == false) this->auth = true; }
		void				set_user(const std::string &new_user) { this->_user = new_user ;}
		std::string			client_id() const { std::string info; return( info =  ":" + get_nick() + "!" + get_user() + "@" + ip()) ;}
		void				insert_channel(Channel *ch){ channels.push_back(ch);}
		client_ch_iterator 	get_vec_begin() { return(channels.begin()); }
		client_ch_iterator 	get_vec_end() { return(channels.end()); }
		size_t				get_vec_size() { return(channels.size());}
		void				remove_channel(client_ch_iterator it) { channels.erase(it);}
		Channel*			get_last_channel(Client *cli);
		
};

#endif