#include <spread/merge.h>

#include <iostream>

#include <boost/bind.hpp>

using namespace std;
using namespace boost;
using namespace spread;

merge::merge(shared_ptr<asio::ip::tcp::acceptor> acceptor,
             ostream & out,
             shared_ptr<progress> _progress)
: acceptor_(acceptor),
  socket_(acceptor->get_io_service()),
  out_(out),
  progress_(_progress)
{
}

void merge::accept(size_t remaining)
{
   acceptor_->async_accept(socket_,
                           bind(&merge::handle_accept, this->shared_from_this(), remaining, asio::placeholders::error));
}

void merge::handle_accept(size_t remaining, const system::error_code& error)
{
   if (!error)
   {
      if (--remaining)
         shared_ptr<merge>(new merge(acceptor_, out_, progress_))->accept(remaining);
      asio::async_read(socket_,
                       asio::buffer(&frame_size_, sizeof(unsigned int)),
                       bind(&merge::handle_read_frame_size, this->shared_from_this(), asio::placeholders::error));
   }
   else
      cerr << "socket error in merge::handle_accept: " << error << endl;
}

void merge::handle_read_frame_size(const system::error_code& error)
{
   if (!error)
   {
      data_.resize(frame_size_);
      asio::async_read(socket_,
                       asio::buffer(&data_[0], data_.size()),
                       bind(&merge::handle_read_frame, this->shared_from_this(), asio::placeholders::error));
   }
   else if (error != asio::error::eof) // eof is fine, connection closed cleanly by peer
      cerr << "socket error in merge::handle_read_frame_size: " << error << endl;

}

void merge::handle_read_frame(const system::error_code& error)
{
   if (!error)
   {
      if (progress_)
         progress_->add_bytes_recv(sizeof(unsigned int) + data_.size());
      out_.write(&data_[0], data_.size());
      asio::async_read(socket_,
                       asio::buffer(&frame_size_, sizeof(unsigned int)),
                       bind(&merge::handle_read_frame_size, this->shared_from_this(), asio::placeholders::error));
   }
   else
      cerr << "socket error in merge::handle_read_frame: " << error << endl;
}
