#include <spread/exploder.h>
#include <iostream>
#include <boost/bind.hpp>

using namespace std;
using namespace boost;
using namespace spread;

exploder::exploder(boost::asio::io_service& io,
                   const boost::asio::ip::tcp::endpoint& server,
                   const vector<boost::asio::ip::tcp::endpoint>& clients,
                   ostream& out,
                   boost::shared_ptr<progress> _progress /*= boost::shared_ptr<progress>()*/)
: acceptor_(new boost::asio::ip::tcp::acceptor(io, server)),
  progress_(_progress)
{
   // wind up the acceptor
   merge_ptr(new merge(acceptor_, out, _progress))->accept(clients.size());

   // connect to every machine out there that will connect to us
   for (vector<boost::asio::ip::tcp::endpoint>::const_iterator it = clients.begin(); it != clients.end(); ++it)
   {
      partitions_.push_back(partition_ptr(new partition(io, *it, _progress)));
      partitions_.back()->connect();
   }
}

exploder & exploder::operator<<(const string& data)
{
   if (progress_)
      progress_->add_bytes_read(data.size());
   for (vector< partition_ptr >::const_iterator it = partitions_.begin(); it != partitions_.end(); ++it)
      (*it)->write(data);
   return *this;
}

exploder::~exploder()
{
   for (vector< partition_ptr >::iterator it = partitions_.begin(); it != partitions_.end(); ++it)
      (*it)->close();
}
