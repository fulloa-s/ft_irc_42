/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: francesco <francesco@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/15 12:02:41 by francesco         #+#    #+#             */
/*   Updated: 2022/03/19 16:04:38 by francesco        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <iostream>

void send_joke(int sockfd)
{
    std::string msg;
    
    srand(time(NULL));
    int r = rand() % 3;
    switch (r)
    {
        case 0:
        {
            msg = "PRIVMSG #bonus :What’s the best thing about Switzerland? I don’t know, but the flag is a big plus.\r\n";
            break;
        }
        case 1:
        {
            msg = "PRIVMSG #bonus :Hear about the new restaurant called Karma? There’s no menu: You get what you deserve.\r\n";
            break;
        }
        case 2:
        {
            msg = "PRIVMSG #bonus :Why don’t scientists trust atoms? Because they make up everything.\r\n";
            break;
        }
        default:
            break;
    }
    send(sockfd, msg.c_str(), msg.length(), 0);
}

int	ft_perror(std::string message)
{
	std::cout << message << std::endl;
	return (1);
}

int main(int argc, char *argv[])
{
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    fd_set    master;
 	fd_set    read_fds;

    FD_ZERO(&read_fds);
	FD_ZERO(&master);

    if (argc < 4) 
    {
       std::cout << "usage bot hostname port password" << std::endl;
       return(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        return(ft_perror("ERROR opening socket"));
    server = gethostbyname(argv[1]);
    if (server == NULL) 
    {
        std::cout << "ERROR, no such host"<< std::endl;
        return(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        return(ft_perror("ERROR connecting"));
    
    char buf[512];
    std::string msg, psw = argv[3];
    msg = "NICK bot\r\nUSER user_bot\r\nPASS " + psw + "\r\n";
    send(sockfd, msg.c_str(), msg.length(), 0);
    recv(sockfd, buf, sizeof buf, 0);
    msg = buf;
    if(msg.find("464") != std::string::npos)
    {
        close(sockfd);
        return(ft_perror("WRONG PASSWORD, RETRY"));
    }
    msg = "JOIN #bonus\r\n";
    send(sockfd, msg.c_str(), msg.length(), 0);
    FD_SET(sockfd, &master);
    while(FD_ISSET(sockfd, &master))
	{
		read_fds = master;
		if (select(sockfd + 1, &read_fds, NULL, NULL, NULL) < 1)
            break;
		if (FD_ISSET(sockfd, &read_fds)) 
        {
            if(recv(sockfd, buf, sizeof buf, 0) < 1)
            {
                close(sockfd);
                return(ft_perror("SERVER HUNG UP"));
            }
            msg = buf;
            if(msg.find(":JOKE\r\n") != std::string::npos)
                send_joke(sockfd);
            bzero(buf, sizeof buf);
        }
	}
    return 0;
}