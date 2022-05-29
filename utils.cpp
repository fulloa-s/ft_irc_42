/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fulloa-s <fulloa-s@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/02 13:27:02 by vspinell          #+#    #+#             */
/*   Updated: 2022/03/02 17:28:06 by fulloa-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"

int	ft_perror(std::string message)
{
	std::cout << message << std::endl;
	return (1);
}

std::string	ft_toupper(std::string str)
{
	std::string ret;
	for (std::string::size_type i=0; i<str.length(); i++)
		ret += toupper(str[i]);
	return (ret);
}
