/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_join.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fulloa-s <fulloa-s@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/02 15:06:25 by fulloa-s          #+#    #+#             */
/*   Updated: 2022/03/07 13:30:08 by fulloa-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"
#include "server.hpp"

void	Server::ch_info_toclient(Channel &ch, Client &client)
{
	std::string msg = client.client_id() + " JOIN #" +  ch.get_name() + "\r\n";
	send(client.get_fd(), msg.c_str(), msg.size(), 0);
	for(channel_iterator it = ch.get_map_begin(); it != ch.get_map_end(); it++)
	{
		if(it->first->get_fd() != client.get_fd())
			send(it->first->get_fd(), msg.c_str(), msg.size(), 0);
	}
	msg = ":myserv 331 " + client.get_nick() + " #" + ch.get_name() + " :No topic is set\r\n";
	send(client.get_fd(), msg.c_str(), msg.size(), 0);
	msg = ":myserv 353 " + client.get_nick() + " = #" + ch.get_name() + " :";
	for(channel_iterator it = ch.get_map_begin(); it != ch.get_map_end(); it++)
	{
		if(it->second.is_Op())
			msg += "@" + (it->first)->get_nick() + " ";
		else
			msg += (it->first)->get_nick() + " ";
	}
	msg += "\r\n";
	send(client.get_fd(), msg.c_str(), msg.size(), 0);
	msg = ":myserv 366 " + client.get_nick() + " #" +  ch.get_name() + " :End of /NAMES list\r\n";
	send(client.get_fd(), msg.c_str(), msg.size(), 0);
	
}

void	Server::create_channel(std::string ch_name, Client &client, std::string ch_key)
{
	Client *ptr = &client;
	Channel *ptr_c;
	
	this->channels.insert(std::make_pair(ch_name, Channel(ch_name,ch_key)));
	for(s_channel_iterator it = channels.begin(); it != channels.end(); it++)
	{
		if(ch_name == (it->second).get_name())
		{
			ptr_c = &it->second;
			client.insert_channel(ptr_c);
			(it->second).create(ptr);
			ch_info_toclient(it->second, client);
		}
	}
}

void	Server::check_channel(std::string ch_name, Client &client,  std::string ch_key)
		{
			Client *ptr = &client;
			Channel *ptr_c;
			
			for(client_ch_iterator it = client.get_vec_begin(); it != client.get_vec_end(); it++)
			{
				if((*it)->get_name() == ch_name)
					return ;
			}
			for(s_channel_iterator it = channels.begin(); it != channels.end(); it++)
			{
				if(ch_name == (it->second).get_name())
				{
					if (!(it->second).get_key().empty())
					{
						if ((it->second).get_key() == ch_key )
						{
							ptr_c = &it->second;
							client.insert_channel(ptr_c);
							(it->second).insert(ptr);
							ch_info_toclient(it->second, client);
						}
						else
							send_error(client, 475, 1, (it->second).get_name().c_str());
					}
					else
					{
						ptr_c = &it->second;
						client.insert_channel(ptr_c);
						(it->second).insert(ptr);
						ch_info_toclient(it->second, client);
					}
					return ;
				}
			}
			create_channel(ch_name, client, ch_key);
		}


size_t Server::vector_split(std::vector<std::string> &ve, const char &c)
{
	size_t pos;
	std::string sub_str;
	size_t vec_size;
	
	for(std::vector<std::string>::size_type it = 0; it != ve.size(); it++)
	{
		if(ve[it].find_last_of(c) == (ve[it].length() - 1))
			ve[it].erase(ve[it].length() - 1);
		vec_size = ve.size();
		while((pos = ve[it].find(c)) != std::string::npos)
		{
			sub_str = ve[it].substr(0, pos);
			ve[it].erase(0,ve[it].substr(0, pos).size() + 1);
			for(std::vector<std::string>::iterator v_it = ve.begin(); v_it != ve.end(); v_it++)
			{
				if(*v_it == ve.at(it) && !sub_str.empty())
				{
					ve.insert(v_it, sub_str);
					it++;
					break;
				}
			}	
		}
		if(vec_size != ve.size())
			it = 0;	
	}
	return(ve.size());
}

int Server::join_zero(Client &client)
{
	Client *_client = &client;
	std::string msg;
	
	for(size_t vec_size; (vec_size = client.get_vec_size() != 0); )
	{
		msg = client.client_id() + " PART #" + (*(*(client.get_vec_begin()))).get_name() + " :Left all channels\r\n";
		send(client.get_fd(), msg.c_str(), msg.size(), 0);
		clean_one_channel(_client, _client->get_vec_begin(), msg);
	}
	return(0);
}