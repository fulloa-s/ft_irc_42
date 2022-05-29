/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_kick.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vspinell <vspinell@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/02 15:06:25 by fulloa-s          #+#    #+#             */
/*   Updated: 2022/03/19 17:54:51 by vspinell         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"
#include "server.hpp"


bool Server::channel_exist(std::string ch_name)
{
	for(s_channel_iterator s_it = channels.begin(); s_it != channels.end(); s_it++)
	{
		if(ch_name == s_it->first)
			return(true);
	}
	return(false);
}

bool Server::is_channel_client(Client &client, Channel &chan)
{
	for(channel_iterator ch_it = chan.get_map_begin(); ch_it != chan.get_map_end(); ch_it++)
	{
		if(client.get_nick() == (*(ch_it->first)).get_nick())
			return (true);
	}
	return(false);
}

Client* Server::is_channel_client_byname(std::string name, Channel &chan)
{
	for(channel_iterator ch_it = chan.get_map_begin(); ch_it != chan.get_map_end(); ch_it++)
	{
		if(name == (*(ch_it->first)).get_nick())
			return (ch_it->first);
	}
	return(NULL);
}

Channel* Server::find_channel(std::string ch_name)
{
	Channel *ret = NULL;

	for(s_channel_iterator c_it = channels.begin(); c_it != channels.end(); c_it++)
	{
		if(c_it->first == ch_name)
			ret = &(c_it->second);
	}
	return(ret);
}

Server::channel_iterator Server::find_client_in_channel(Channel &chan, std::string client_name)
{
	channel_iterator ret = chan.get_map_end();
	for(channel_iterator ch_it = chan.get_map_begin(); ch_it != chan.get_map_end(); ch_it++)
	{
		if(client_name == (*(ch_it->first)).get_nick())
		{
			ret = ch_it;
			return (ret);
		}
	}
	return(ret);
}

void Server::kick_clients(Client &client, std::vector<std::string> &client_list, Channel *channel, std::string opt_msg)
{
	std::string msg;
	Client *cl_ptr;
	
	for(std::vector<std::string>::iterator it = client_list.begin(); it != client_list.end(); it++)
	{
		if((cl_ptr = is_channel_client_byname(*it, *channel)))
		{
			msg = client.client_id() + " KICK #" + channel->get_name() + " " + *it + " ";
			if(opt_msg.empty())
				msg += ":" + *it + "\r\n";
			else
				msg += opt_msg + "\r\n";
			send(cl_ptr->get_fd(), msg.c_str(), msg.size(),0);
			clean_one_channel(cl_ptr, find_ch_ptr(cl_ptr, channel->get_name()), msg);
		}
		else
			send_error(client, 401, 1, (*it).c_str());
	}
		
}
