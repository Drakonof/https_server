#ifndef HTTPS_REQUEST
#    define HTTPS_REQUEST

#    ifdef __cplusplus

#include <sstream>

class Http_request {
public:
    enum class eType {
        Unknown,
        GET
    };
    
    Http_request() {}
    
    void parse(const char *);
    
    const eType &type() const;
    const std::string &uri() const;

private:
    void  parse_request_line(std::string &);
    
    eType type_ {eType::Unknown};
    std::string uri_;
};




#    endif //__cplusplus
#endif // HTTPS_REQUEST
