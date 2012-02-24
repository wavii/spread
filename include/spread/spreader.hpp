#ifndef __SPREAD_SPREADER_HPP__
#define __SPREAD_SPREADER_HPP__

#include <vector>
#include <string>
#include <ostream>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include <spread/merge.h>
#include <spread/hashers.h>
#include <spread/partition.h>
#include <spread/progress.h>

namespace spread {

/*
 * spreader spreads data across a fleet so that every key is guaranteed to be on the same node
 */
template<typename Hash = ToSpace>
class spreader
{
public:
   // types
   typedef spreader<Hash> spreader_type;
   typedef Hash   hasher_type;

   spreader(boost::asio::io_service& io,
            const boost::asio::ip::tcp::endpoint& server,
            const std::vector<boost::asio::ip::tcp::endpoint>& clients,
            std::ostream& out,
            boost::shared_ptr<progress> _progress = boost::shared_ptr<progress>(),
            const hasher_type& hfn = hasher_type())
   : acceptor_(new boost::asio::ip::tcp::acceptor(io, server)),
     progress_(_progress),
     hfn_(hfn)
   {
      // wind up the acceptor
      merge_ptr(new merge(acceptor_, out, _progress))->accept(clients.size());

      // connect to every machine out there that will connect to us
      for (std::vector<boost::asio::ip::tcp::endpoint>::const_iterator it = clients.begin(); it != clients.end(); ++it)
      {
         partitions_.push_back(partition_ptr(new partition(io, *it, _progress)));
         partitions_.back()->connect();
      }
   }

   spreader_type & operator<<(const std::string& data)
   {
      if (progress_)
         progress_->add_bytes_read(data.size());
      partitions_[hfn_(data) % partitions_.size()]->write(data);
      return *this;
   }

   ~spreader()
   {
      for (typename std::vector< partition_ptr >::iterator it = partitions_.begin(); it != partitions_.end(); ++it)
         (*it)->close();
   }

private:

   typedef boost::shared_ptr< merge > merge_ptr;
   typedef boost::shared_ptr< partition > partition_ptr;

   boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
   boost::shared_ptr<progress> progress_;
   const hasher_type & hfn_;
   std::vector< partition_ptr > partitions_;
};

} // spread

#endif // __SPREAD_SPREADER_HPP__
