#ifndef HTTPS_SERVER
#    define HTTPS_SERVER

#    ifdef __cplusplus

#    include <iostream>
#    include <fstream>
#    include <arpa/inet.h>
#    include <openssl/ssl.h>
#    include <openssl/err.h>

static void *get_in_addr(sockaddr *client) {
    if (client->sa_family == AF_INET) return &(((sockaddr_in *)client)->sin_addr);
    
    return &(((sockaddr_in6 *)client)->sin6_addr);
}

static std::string get_ip_addr(const sockaddr_in &client) {

    char ip[INET6_ADDRSTRLEN] = {};
    
    inet_ntop(client.sin_family, get_in_addr((sockaddr *) &client), ip, sizeof ip);
    return std::string(ip);
}

static std::string get_date_time(void) {

    char buffer[100]{};
    time_t curr_time;
    tm *p_curr_time;
    
    time(&curr_time);
    
    p_curr_time = localtime(&curr_time);
    
   strftime(buffer, 100, "%T %d.%m.%Y", p_curr_time);
   return std::string(buffer);
}

class Https_server {
public:
	Https_server(int port = 443); // 443 is used fot https
//	~Https_server();

	bool start(void);

	void set_cer_path(std::string);
	void set_key_path(std::string);
	void set_ca_path(std::string);
private:
	int socket_{-1};
	int port_ {443};

	std::string cer_path_;
	std::string key_path_;
	std::string ca_path_;
	std::string html_data_;

	std::string start_date_time_;
	size_t req_cnt_ {0};

    SSL_METHOD *p_ssl_method_ {nullptr};
    SSL_CTX *p_ssl_contex_ {nullptr};

    std::vector<char> icon_buf_;

	bool ssl_init_(void);
	bool socket_init_(void);
	void client_processing_(int, std::string);
};


#    endif
#endif
