// sudo openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout selfsigned.key -out selfsigned.crt
//  |
// oz


//chrome://flags/#allow-insecure-localhost           -> enable
//https://localhost:11000
//https://localhost:11000/hello

#include <unistd.h>
#include <sstream>
#include <experimental/filesystem>
#include <string.h>

#include "https_server.h"
#include "http_request.h"

namespace fs = std::experimental::filesystem;

Https_server::Https_server(int port) : port_ {port} {


}

bool Https_server::start(void) {
    std::ifstream html_file;
    fs::path path_html_file = fs::current_path() / "index.html"; //??????????????
    html_file.open(path_html_file.c_str(), std::ios::binary | std::ios::ate);
    
    if (!html_file.is_open()) {
        std::cerr << "html_file.open failed" << std::endl;
        return false;
    }
    
    html_data_.reserve(html_file.tellg());
    html_file.seekg(0, std::ios::beg);
    html_data_.assign((std::istreambuf_iterator<char>(html_file)), std::istreambuf_iterator<char>());
    html_file.close();
    
    std::ifstream icon_file;
    fs::path path_icon_file = fs::current_path() / "favicon.png";
    icon_file.open(path_html_file.c_str(), std::ios::binary | std::ios::ate);
    
    if (!icon_file.is_open()) {
        std::cerr << "icon_file.open failed" << std::endl;
    }
    
    std::streamsize size = icon_file.tellg();
    icon_buf_.resize(size);
    icon_file.seekg(0, std::ios::beg);
    
    if (!icon_file.read(icon_buf_.data(), size)) {
        std::cerr << "icon_file read failed" << std::endl;
        return false;
    }
    icon_file.close();
    
    
    if (!ssl_init_()) {
        std::cerr << "ssl init failed" << std::endl;
        return false;
    }
    
    if (!socket_init_()) {
        std::cerr << "socket init failed" << std::endl;
        return false;
    }
    
    std::cout << "https started. port: " << port_ << std::endl;
    start_date_time_ = get_date_time();
    
    while (true) {
        int sock_client;
        sockaddr_in client;
        socklen_t client_len = sizeof(client);
        
        sock_client = accept(socket_, (sockaddr *) &client, &client_len);
        
        if (-1 == sock_client) {
            std::cerr << "accept failed " << port_ << std::endl;
            continue;
        }
        else std::cout << sock_client << std::endl;
           
        
        std::string ip = get_ip_addr(client); 
        client_processing_(sock_client, ip);
    }
    
    return true;
}

void Https_server::set_cer_path(std::string cer_path) {
    cer_path_ = cer_path;
}

void Https_server::set_key_path(std::string key_path) {
    key_path_ = key_path;
}

void Https_server::set_ca_path(std::string ca_path) {
    ca_path_ = ca_path;
}

void Https_server::client_processing_(int socket_c, std::string ip_c) {
    SSL *p_ssl;
    
    p_ssl = SSL_new(p_ssl_contex_);
    SSL_set_fd(p_ssl, socket_c);
    
    char buffer[1024 * 16] {};
    int read_bytes {0};
    
    if (SSL_accept(p_ssl) == -1) {
        std::cerr << "SSL_accept failed" << std::endl;
        
        SSL_free(p_ssl);
        close(socket_c);
        
        return;
    }
    
    read_bytes = SSL_read(p_ssl, buffer, sizeof(buffer));
    
    if (read_bytes <= 0) {
        SSL_free(p_ssl);
        close(socket_c);
        
        return;
    }
    
    buffer[read_bytes] = '\0';
    
    Http_request req;
    req.parse(buffer);
    
    std::cout << "connection from(" << ip_c << ")" << "request uri \"" 
              << req.uri() << "\"" << std::endl;
    
    std::cout << "num: " << read_bytes << std::endl << std::string(buffer) << std::endl;
    
    
    if (req.type() != Http_request::eType::GET) {
        std::cout << "Unknown type of request. Not support. EXIT" << std::endl;
        return;
    }
    
    ++req_cnt_;
    
    switch(req.uri().length()) {
        case 1: {  // "/"
            if (req.uri() == "/") {
                std::stringstream response;
                std::cout << "=====================1===============" << std::endl;
                response << "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: text/html\r\nContent-Lenght: " 
                << html_data_.size() << "\r\n\r\n";
                
                
                int sended = 0;
                sended = SSL_write(p_ssl, response.str().c_str(), response.str().length());
                
                if (sended > 0) {
                    sended = SSL_write(p_ssl, html_data_.data(), html_data_.size());
                }
                
            }

            break; 
        }
        
        case 12: {  // "/favicon.ico"   
            
            if (req.uri() == "/favicon.ico") {
                std::stringstream response;
                std::cout << "=====================12==============="<< std::endl;
                response << "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: image/png\r\nContent-Lenght: " 
                         << icon_buf_.size() << "\r\n\r\n";
                
                
                int sended = 0;
                sended = SSL_write(p_ssl, response.str().c_str(), response.str().length());
                
                if (sended > 0) {
                    sended = SSL_write(p_ssl, icon_buf_.data(), icon_buf_.size());
                }
            }
            
            
        break;    
        }
        case 38: {  // "/how_many_requests_were_from_beginning"   
            std::cout << "=====================Ddd===============" << std::endl;
            if (req.uri() == "/how_many_requests_were_from_beginning" ) {
                std::stringstream response;
                std::cout << "=====================38===============" << std::endl;
                response << "server strarted : " << start_date_time_ << "<br>" 
                         << "count of GET request : " << req_cnt_ << "<br>";
                
                         
                std::stringstream ok_response;        
                ok_response << "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: text/html\r\nContent-Lenght: " 
                          << response.str().length() << "\r\n\r\n";
                
                
                int sended = 0;
                sended = SSL_write(p_ssl, ok_response.str().c_str(), ok_response.str().length());
                
                if (sended > 0) {
                    sended = SSL_write(p_ssl, response.str().data(), response.str().length());
                }
            }
            
            
        break;    
        }
        
    }
}

bool Https_server::ssl_init_(void) {
	SSL_library_init();
    
    SSL_load_error_strings();
    
    p_ssl_method_ = const_cast<SSL_METHOD *>(TLSv1_2_server_method());
    
    p_ssl_contex_ = SSL_CTX_new(p_ssl_method_);
    
    if (nullptr == p_ssl_method_) {
        std::cerr << "p_ssl_method_" << std::endl;
        return false;
    }
    
     if (SSL_CTX_use_certificate_file(p_ssl_contex_, cer_path_.c_str(), SSL_FILETYPE_PEM) <= 0) {
         std::cerr << "SSL_CTX_use_certificate_file" << std::endl;
         return false;
     }
     
     if (SSL_CTX_use_PrivateKey_file(p_ssl_contex_, key_path_.c_str(), SSL_FILETYPE_PEM) <= 0) {
         std::cerr << "SSL_CTX_use_PrivateKey_file" << std::endl;
         return false;
     }
     
     if (!SSL_CTX_check_private_key(p_ssl_contex_)) {
         std::cerr << "SSL_CTX_check_private_key" << std::endl;
         return false;
     }
     
     if (ca_path_.size() > 0) {
         if (0 == SSL_CTX_load_verify_locations(p_ssl_contex_, ca_path_.c_str(), NULL)) {
             std::cerr << "SSL_CTX_load_verify_locations" << std::endl;
         return false;
         } 
     }
    
    return true;
}


bool Https_server::socket_init_(void) {
    if ((socket_ = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "socket failed " << port_ << std::endl;
        return false;
    }
    
    sockaddr_in serv;
    memset(&serv, 0, sizeof(sockaddr_in));
    
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(port_);
    
    if (bind(socket_, (sockaddr *) &serv, sizeof(serv)) != 0) {
        std::cerr << "bind failed " << port_ << std::endl;
        return false;
    }
    
    if (listen(socket_, 1000) != 0) {
        std::cerr << "listen failed " << std::endl;
        return false;
    }
    
    return true;
}
