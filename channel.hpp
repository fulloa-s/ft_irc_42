/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vspinell <vspinell@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/23 12:00:20 by fulloa-s          #+#    #+#             */
/*   Updated: 2022/03/09 17:44:35 by vspinell         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_H
#define CHANNEL_H

#include "Header.hpp"
#include "client.hpp"

class Client;

class Channel {

	public:
	
		struct client_info {

				private:

				int _Op;

				public:
				
				client_info(const int &x) { _Op = x;};
				int		is_Op(){ return (_Op); }
				void	set_Op(char sign) { _Op = (sign == '-')? 0 : 1 ;}
				
		};

		Channel(std::string name) : _name(name), ch_modes("+tnCN") {};
		Channel(std::string name, std::string key) : _name(name), _key(key), ch_modes("+tnCN") {};
		
		//t = Only ops and half-ops can change channel topic when this mode is set. This mode is set by default on newly created channels.
		//n = Users must actually be on channel to send any messages to it. If this mode is unset, users can send to channel without being on it by using /MSG #channel text. This mode is set by default on newly created channels.
		//C = Blocks users from sending CTCP requests to channel.
		//N = Users that are on channel won't be able to change their nick.
		
		~Channel(){};
		std::string get_name() const { return(this->_name); }
		std::string	gt_ch_modes() const { return this->ch_modes ;}
		std::string get_key() const { return(this->_key); }
		void create(Client *creator) { ch_clients.insert(std::make_pair(creator, client_info(1))) ;}
		void insert(Client *user) { ch_clients.insert(std::make_pair(user, client_info(0))); }
		
		
	private:
	
		typedef std::map<Client *, client_info>			channel_map;
		typedef channel_map::iterator					channel_iterator;
		std::string										_name;
		std::string										_key;
		std::map<Client *, client_info>					ch_clients;
		std::string										ch_modes;

	public:
	
		channel_iterator	get_map_begin() { return(ch_clients.begin()); }
		channel_iterator	get_map_end() { return(ch_clients.end()); }
		channel_iterator	remove_client(channel_iterator it){ch_clients.erase(it); return (get_map_begin());}
		size_t				map_size(){ return (ch_clients.size());}
};

#endif