#include <spread/partition.h>

#include <iostream>

#include <boost/bind.hpp>

using namespace boost;
using namespace spread;

partition::partition(asio::io_service & io,
         const asio::ip::tcp::endpoint& endpoint,
         shared_ptr<progress> _progress)
: io_(io),
  endpoint_(endpoint),
  progress_(_progress),
  socket_(new asio::ip::tcp::socket(io)),
  writing_(false),
  connected_(false),
  closing_(false)
{
}

void partition::connect()
{
   socket_->async_connect(endpoint_,
                          bind(&partition::handle_connect, this->shared_from_this(), asio::placeholders::error));
}

void partition::write(const std::string & data)
{
   mf_.push_back(data);
   if (mf_.full_frame())
      io_.post(bind(&partition::do_write, this->shared_from_this()));
}

void partition::close()
{
   io_.post(bind(&partition::do_close, this->shared_from_this()));
}

void partition::handle_connect(const system::error_code& error)
{
  if (!error)
  {
     connected_ = true;
     do_write();
  }
  else if (error == system::errc::connection_refused) // that's okay, other host isn't up yet
  {
     socket_->close();
     socket_.reset(new asio::ip::tcp::socket(io_));
     socket_->async_connect(endpoint_,
                            bind(&partition::handle_connect, this->shared_from_this(), asio::placeholders::error));
  }
  else
     std::cerr << "socket error in partition::handle_connect: " << error << std::endl;
}

void partition::do_close()
{
  closing_ = true;
  do_write();
}

void partition::do_write()
{
  if (writing_ || !connected_)
     return;
  if (!mf_.size() && closing_)
     socket_->close();
  if (mf_.full_frame() || (closing_ && mf_.size())) // write either if we have a full frame or if we are closing
  {
     writing_ = true;
     asio::async_write(*socket_,
                       mf_.pop_front(),
                       bind(&partition::handle_write_frame, this->shared_from_this(), asio::placeholders::error,
                       asio::placeholders::bytes_transferred));
  }
}

void partition::handle_write_frame(const system::error_code& error, size_t bytes_transferred)
{
  if (!error)
  {
     if (progress_)
        progress_->add_bytes_sent(bytes_transferred);
     if (!mf_.size() && closing_)
        socket_->close();
     if (mf_.full_frame() || (closing_ && mf_.size())) // write either if we have a full frame or if we are closing
        asio::async_write(*socket_,
                          mf_.pop_front(),
                          bind(&partition::handle_write_frame, this->shared_from_this(), asio::placeholders::error,
                          asio::placeholders::bytes_transferred));
     else
        writing_ = false;
  }
  else
     std::cerr << "socket error in partition::write_frame: " << error << std::endl;
}
