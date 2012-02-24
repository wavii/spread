#include <spread/buffer.h>

using namespace std;
using namespace boost;
using namespace spread;

const size_t frame::FRAME_SIZE = 32768;
const size_t multi_frame::MAX_SIZE = 1048576;

frame::frame(const string& s)
: data_(new vector<char>(sizeof(unsigned int)))
{
   data_->reserve(sizeof(unsigned int) + frame::FRAME_SIZE);
   push_back(s);
}

size_t frame::size() const
{
   return data_->size() - sizeof(unsigned int);
}

void frame::push_back(const string& s)
{
   size_t old_size = data_->size();
   data_->resize(data_->size() + s.size());
   copy(s.c_str(), s.c_str() + s.size(), data_->begin() + old_size);
   *reinterpret_cast<unsigned int*>(&((*data_)[0])) = data_->size() - sizeof(unsigned int);
   buffer_ = asio::buffer(*data_);
}

const asio::const_buffer* frame::begin() const
{
    return &buffer_;
}

const asio::const_buffer* frame::end() const
{
    return &buffer_ + 1;
}

// multi_frame

multi_frame::multi_frame(size_t max_size /* = multi_frame::MAX_SIZE */, size_t frame_size /* = frame::FRAME_SIZE */)
: max_size_(max_size),
  frame_size_(frame_size),
  size_(0)
{
}

size_t multi_frame::size() const
{
   lock_guard<mutex> lock(mutex_);
   return size_;
}

bool multi_frame::full_frame() const
{
   return size() >= frame_size_;
}

void multi_frame::push_back(const string& s)
{
   unique_lock<mutex> lock(mutex_);
   while (max_size_ != 0 && size_ + s.size() > max_size_)
      cond_.wait(lock);
   size_ += s.size();
   if (frames_.empty() || frames_.back().size() + s.size() > frame_size_)
      frames_.push_back(frame(s));
   else
      frames_.back().push_back(s);
}

frame multi_frame::pop_front()
{
   lock_guard<mutex> lock(mutex_);
   frame f = frames_.front();
   size_ -= f.size();
   frames_.pop_front();
   if (size_ < max_size_)
      cond_.notify_all();
   return f;
}
