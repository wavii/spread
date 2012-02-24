#ifndef __SPREAD_PROGRESS_H__
#define __SPREAD_PROGRESS_H__

#include <vector>
#include <string>

#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/cstdint.hpp>

namespace spread {

// records & reports read/write transmission stats
class progress
{
public:

   progress(boost::asio::io_service & io);

   ~progress();

   void add_bytes_read(size_t bytes);

   void add_bytes_recv(size_t bytes);

   void add_bytes_sent(size_t bytes);

private:

   std::string pretty_size(boost::uint64_t size);

   void display_tick(const boost::system::error_code& error);

   boost::asio::deadline_timer timer_;

   mutable boost::mutex mutex_;

   boost::uint64_t bytes_read_[3];
   boost::uint64_t bytes_recv_[3];
   boost::uint64_t bytes_sent_[3];
};

} // spread

#endif // __SPREAD_PROGRESS_H__
