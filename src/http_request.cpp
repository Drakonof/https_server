#include "http_request.h"


void Http_request::parse(const char *data_str) {
    std::istringstream iss(data_str);
    
    std::string line;
    std::getline(iss, line);
    
    parse_request_line(line);
    
    std::string str;
    
    while (std::getline(iss, line)) {
          std::istringstream iss_line(line);
          iss_line >> str;
    }
    
    str = "";
    
}

void Http_request::parse_request_line(std::string &data_str) {
    type_ = eType::Unknown;
    
    std::istringstream iss(data_str);
    std::string type;
    
    iss >> type;
    
    switch (type.length()) {
        case 3: {
           if (type == "GET") {
               type_ = eType::GET;
           }
           
           break;
        }
    }
    
    iss >> uri_;
}


const Http_request::eType &Http_request::type() const {
    return type_;
}


const std::string &Http_request::uri() const {
    return uri_;
}
