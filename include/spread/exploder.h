#ifndef __SPREAD_EXPLODER_H__
#define __SPREAD_EXPLODER_H__

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
 * exploder spreads data across a fleet so that every node has a full copy of its own and all its neighbor's data
 */
class exploder
{
public:

   exploder(boost::asio::io_service& io,
            const boost::asio::ip::tcp::endpoint& server,
            const std::vector<boost::asio::ip::tcp::endpoint>& clients,
            std::ostream& out,
            boost::shared_ptr<progress> _progress);

   exploder & operator<<(const std::string& data);

   ~exploder();

private:

   typedef boost::shared_ptr< merge > merge_ptr;
   typedef boost::shared_ptr< partition > partition_ptr;

   boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
   boost::shared_ptr<progress> progress_;
   std::vector< partition_ptr > partitions_;
};

} // spread

#endif // __SPREAD_EXPLODER_H__
