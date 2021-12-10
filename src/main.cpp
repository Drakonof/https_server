//https://www.youtube.com/watch?v=A4y97MTqO6c

#include <iostream>

#include <experimental/filesystem>

#include "https_server.h"

namespace fs = std::experimental::filesystem;

int main (int argc, char **argv) {
    int port {11000};
    
    if (argc >= 2) {
        port = std::stoi(argv[1]); 
    }    
        
        
    Https_server https_server(port);

    fs::path cer_path = fs::current_path() / "selfsigned.crt";
    fs::path key_path = fs::current_path() / "selfsigned.key";
    
    https_server.set_cer_path(cer_path.string());
    https_server.set_key_path(key_path.string());
    
    if (false == https_server.start()) {
        std::cerr << "start failed" << std::endl;
    }

	return 0;
}
