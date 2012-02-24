#include <iostream>
#include <fstream>
#include <vector>

#include <boost/asio.hpp>
#include <boost/scoped_array.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/program_options.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <spread/spreader.hpp>
#include <spread/progress.h>
#include <spread/endpoint.h>

using namespace spread;
using namespace boost;
using boost::asio::ip::tcp;
using namespace std;
namespace po = boost::program_options;

template<typename Hasher>
void run(spreader<Hasher>& p, istream& in)
{
   // any larger, and we may try to write to a multi_frame that never has enough space
   static const int BUF_SIZE = multi_frame::MAX_SIZE - frame::FRAME_SIZE;
   scoped_array<char> buffer(new char[BUF_SIZE]);

   ios::sync_with_stdio(false); // makes a big difference on buffered i/o

   for (int line = 1; !in.eof(); ++line)
   {
      in.getline(buffer.get(), BUF_SIZE - 1); // leave 1 for us to inject back the newline
      if (buffer[0] == '\0')
         continue;
      if (in.fail()) // line was too long?
      {
         cerr << "Skipping line <" << line << ">: line is probably too long" << endl;
         in.clear(); // clear state
         in.ignore(numeric_limits<streamsize>::max(), '\n');
         continue;
      }
      buffer[in.gcount() - 1] = '\n'; // inject back the newline
      buffer[in.gcount()] = '\0';
      p << buffer.get();
   }
}

int main(int argc, char * argv[])
{
   int local_port = 0;
   string separator;
   vector<string> remotes;
   string remotes_filename;
   string input_filename;
   string output_filename;

   po::options_description desc("Allowed options");
   desc.add_options()
      ("help,h", "produce help message")
      ("port,p", po::value<int>(&local_port)->default_value(9999), "The local port")
      ("separator,d", po::value<string>(&separator)->default_value("", "\\s+"), "The separator between the key and the rest")
      ("host,h", po::value< vector<string> >(&remotes)->composing(),  "An address, in a host:port format.  Can specify multiple -r args.")
      ("hostfile,f", po::value< string >(&remotes_filename),  "A file containing addresses, in host:port format.  Can combine with -r args.")
      ("input,i",  po::value<string>(&input_filename), "The input file. If not specified it will use stdin")
      ("ouput,o",  po::value<string>(&output_filename), "The output file. If not specified it will use stdout")
      ("progress",  "Print transmission stats once every 5 seconds.")
   ;

   po::variables_map vm;
   try 
   {
      po::store(po::parse_command_line(argc, argv, desc), vm);
      po::notify(vm);
   }
   catch(std::exception& e)
   {
      cerr << e.what() << "\n";
      return 1;
   } 

   if (!remotes_filename.empty())
   {
       ifstream in(remotes_filename.c_str());
       copy(istream_iterator<string>(in), istream_iterator<string>(), back_inserter(remotes));
   }

   if (vm.count("help") || !local_port || remotes.empty()) 
   {
      cerr << "Usage: spread [options] < input\n";
      cerr << desc;
      return 0;
   }

   vector<tcp::endpoint> endpoints;
   address_to_endpoint(9999, remotes, endpoints);

   ifstream in(input_filename.c_str());
   ofstream out(output_filename.c_str());

   asio::io_service io;
   shared_ptr<thread> pt; // give the io_service a separate thread, so we can do blocking i/o on the main thread

   if (separator.empty())
   {
      spreader<ToSpace> p(io, tcp::endpoint(tcp::v4(), local_port), endpoints, output_filename.empty() ? cout : out,
                             shared_ptr<progress>(vm.count("progress") ? new progress(io) : NULL), ToSpace());
      pt.reset(new thread(bind(&asio::io_service::run, &io)));
      run(p, input_filename.empty() ? cin : in);
   }
   else
   {
      spreader<ToSeparator> p(io, tcp::endpoint(tcp::v4(), local_port), endpoints, output_filename.empty() ? cout : out,
                                 shared_ptr<progress>(vm.count("progress") ? new progress(io) : NULL), ToSeparator(separator[0]));
      pt.reset(new thread(bind(&asio::io_service::run, &io)));
      run(p, input_filename.empty() ? cin : in);
   }

   pt->join(); // wait for any pending io_service events to flush

   return 0;
}
