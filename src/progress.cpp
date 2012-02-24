#include <iostream>
#include <sstream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <sstream>

#include <spread/progress.h>

using namespace std;
using namespace spread;
using namespace boost;

progress::progress(asio::io_service & io)
: timer_(io, posix_time::seconds(5))
{
   for (size_t i = 0; i != 3; ++i)
       bytes_read_[i] = bytes_recv_[i] = bytes_sent_[i] = 0;

   timer_.async_wait(bind(&progress::display_tick, this, asio::placeholders::error));
}

progress::~progress()
{
   timer_.cancel();
}

void progress::add_bytes_read(size_t bytes)
{
   lock_guard<mutex> lock(mutex_);
   bytes_read_[0] += bytes;
}

void progress::add_bytes_recv(size_t bytes)
{
   bytes_recv_[0] += bytes;
}

void progress::add_bytes_sent(size_t bytes)
{
   bytes_sent_[0] += bytes;
}

string progress::pretty_size(uint64_t size)
{
   ostringstream oss;

   if (size >= 1073741824)
      oss << static_cast<float>(size) / 1073741824 << "GB";
   else if (size >= 1048576)
      oss << static_cast<float>(size) / 1048576 << "MB";
   else if (size >= 1024)
      oss << static_cast<float>(size) / 1024 << "KB";
   else
      oss << size << "bytes";

   return oss.str();
}

void progress::display_tick(const system::error_code& error)
{
   if (!error)
   {
      {
         lock_guard<mutex> lock(mutex_);
         size_t bytes_read_rate = (bytes_read_[0] - bytes_read_[2]) / (bytes_read_[2] ? 15 : bytes_read_[1] ? 10 : 5);
         cerr << pretty_size(bytes_read_[0]) << " read [" << pretty_size(bytes_read_rate) << "/sec], ";
         bytes_read_[2] = bytes_read_[1];  bytes_read_[1] = bytes_read_[0];
      }

      size_t bytes_recv_rate = (bytes_recv_[0] - bytes_recv_[2]) / (bytes_recv_[2] ? 15 : bytes_recv_[1] ? 10 : 5);
      cerr << pretty_size(bytes_recv_[0]) << " recv [" << pretty_size(bytes_recv_rate) << "/sec], ";
      bytes_recv_[2] = bytes_recv_[1];  bytes_recv_[1] = bytes_recv_[0];

      size_t bytes_sent_rate = (bytes_sent_[0] - bytes_sent_[2]) / (bytes_sent_[2] ? 15 : bytes_sent_[1] ? 10 : 5);
      cerr << pretty_size(bytes_sent_[0]) << " sent [" << pretty_size(bytes_sent_rate) << "/sec]" << endl;
      bytes_sent_[2] = bytes_sent_[1];  bytes_sent_[1] = bytes_sent_[0];

      timer_.expires_from_now(posix_time::seconds(5));
      timer_.async_wait(bind(&progress::display_tick, this, asio::placeholders::error));
   }
}
