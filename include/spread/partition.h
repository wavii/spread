#ifndef __SPREAD_PARTITION_H__
#define __SPREAD_PARTITION_H__

#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include <spread/buffer.h>
#include <spread/progress.h>

namespace spread {

/**
 * writes frames to a socket
 */
class partition  : public boost::enable_shared_from_this< partition >
{
public:

   partition(boost::asio::io_service& io,
             const boost::asio::ip::tcp::endpoint& endpoint,
             boost::shared_ptr<progress> _progress);

   void connect();

   void write(const std::string& data);

   void close();

private:

   void handle_connect(const boost::system::error_code& error);

   void do_close();

   void do_write();

   void handle_write_frame(const boost::system::error_code& error, size_t bytes_transferred);

   boost::asio::io_service & io_;
   boost::asio::ip::tcp::endpoint endpoint_;
   boost::shared_ptr<progress> progress_;
   boost::shared_ptr<boost::asio::ip::tcp::socket> socket_;
   multi_frame mf_;
   bool writing_;
   bool connected_;
   bool closing_;
};

} // spread

#endif // __SPREAD_PARTITION_H__
