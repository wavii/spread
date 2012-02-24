#ifndef __SPREAD_ENDPOINT_H__
#define __SPREAD_ENDPOINT_H__

#include <vector>
#include <string>

#include <boost/asio.hpp>

namespace spread {

void address_to_endpoint(int default_port, const std::vector<std::string>& addresses,
                         std::vector<boost::asio::ip::tcp::endpoint>& endpoints);

} // spread

#endif // __SPREAD_ENDPOINT_H__
