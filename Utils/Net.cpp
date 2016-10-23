#include <vector>
#include <arpa/inet.h>
#include <algorithm>

#include "Net.hpp"

namespace hnet {

bool is_ip_v4(const std::string &ip)
{
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr))!=0;
}


}
