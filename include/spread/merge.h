#ifndef __SPREAD_MERGE_H__
#define __SPREAD_MERGE_H__

#include <ostream>
#include <vector>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include <spread/progress.h>

namespace spread {

/**
 * reads frames from endpoint and merges them to an output
 */
class merge : public boost::enable_shared_from_this< merge >
{
public:

   merge(boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor,
         std::ostream & out,
         boost::shared_ptr<progress> _progress);

   void accept(size_t remaining);

private:

   void handle_accept(size_t remaining, const boost::system::error_code& error);

   void handle_read_frame_size(const boost::system::error_code& error);

   void handle_read_frame(const boost::system::error_code& error);

   boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
   boost::asio::ip::tcp::socket socket_;
   std::ostream& out_;
   boost::shared_ptr<progress> progress_;
   unsigned int frame_size_;
   std::vector<char> data_;
};

} // spread

#endif // __SPREAD_MERGE_H__
