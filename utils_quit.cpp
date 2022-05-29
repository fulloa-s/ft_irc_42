/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_quit.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fulloa-s <fulloa-s@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/02 15:06:25 by fulloa-s          #+#    #+#             */
/*   Updated: 2022/03/04 18:58:39 by fulloa-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"
#include "server.hpp"


void    Server::clean_one_channel(Client *_client, client_ch_iterator ch_todestroy, std::string msg)
{
	channel_iterator tmp;
	
	for(channel_iterator c_it = (*ch_todestroy)->get_map_begin(); c_it != (*ch_todestroy)->get_map_end(); c_it++)
	{
		if((*(c_it->first)).get_nick() != (_client)->get_nick())
			send((*(c_it->first)).get_fd(), msg.c_str(), msg.size(), 0);
		else
			tmp = c_it;
	}
	(*ch_todestroy)->remove_client(tmp);
	if(!(*ch_todestroy)->map_size())
			channels.erase((*ch_todestroy)->get_name());
	 _client->remove_channel(ch_todestroy);	
}



void    Server::clean_all_channels(Client *_client, std::string msg) 
{
	for(size_t vec_size; (vec_size = (_client)->get_vec_size() != 0); )
		clean_one_channel(_client, _client->get_vec_begin(), msg);
}