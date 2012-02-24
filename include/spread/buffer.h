#ifndef __SPREAD_BUFFER_H__
#define __SPREAD_BUFFER_H__

#include <list>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

namespace spread {

// A reference-counted non-modifiable buffer class.
class frame
{
public:

   static const size_t FRAME_SIZE;

   typedef boost::asio::const_buffer value_type;
   typedef const boost::asio::const_buffer* const_iterator;

   explicit frame(const std::string& s);

   size_t size() const;

   void push_back(const std::string& s);

   // implements the ConstBufferSequence requirements.
   const boost::asio::const_buffer* begin() const;
   const boost::asio::const_buffer* end() const;

private:

   boost::shared_ptr< std::vector<char> > data_;
   boost::asio::const_buffer buffer_;

};

// A buffer list with a thread-safe size-wait semantic
class multi_frame
{
public:

   static const size_t MAX_SIZE;

   multi_frame(size_t max_size = MAX_SIZE, size_t frame_size = frame::FRAME_SIZE);

   size_t size() const;

   bool full_frame() const;

   void push_back(const std::string & s);

   frame pop_front();

private:

   std::list<frame> frames_;
   boost::condition_variable cond_;
   mutable boost::mutex mutex_;
   size_t max_size_;
   size_t frame_size_;
   size_t size_;

};

} // spread

#endif // __SPREAD_BUFFER_H__
