#include <spread/endpoint.h>

#include <algorithm>

using namespace std;
using namespace boost;
using namespace boost::asio::ip;

void spread::address_to_endpoint(int default_port, const vector<string>& addresses, vector<tcp::endpoint>& endpoints)
{
   vector<string> sorted_addresses(addresses.begin(), addresses.end());
   sort(sorted_addresses.begin(), sorted_addresses.end()); // order must be consistent across each machine

   asio::io_service io;
   tcp::resolver r(io);
   std::ostringstream ssport;
   ssport << default_port;

   for (vector<string>::const_iterator it = sorted_addresses.begin(); it != sorted_addresses.end(); ++it)
   {
      size_t colon = it->find(':');
      if (colon == string::npos)
         endpoints.push_back(*r.resolve(tcp::resolver::query(it->c_str(), ssport.str().c_str())));
      else
         endpoints.push_back(*r.resolve(tcp::resolver::query(it->substr(0, colon).c_str(), it->substr(colon + 1).c_str())));
   }
}
