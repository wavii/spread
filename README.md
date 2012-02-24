# Spread

Spread is a missing link in the unix toolchain to do simple distributed data mining.  Spread partitions data according to key across a fleet.  Spread shines when used in conjunction with fleet management/coordination/persistence tools like:

* [Chef](http://www.opscode.com/chef/)
* [gnu parallel](http://www.gnu.org/software/parallel/), [xargs -P](http://matpalm.com/blog/2009/11/06/xargs-parallel-execution/), or [dsh](http://www.netfort.gr.jp/~dancer/software/dsh.html.en)
* [s3cmd](http://s3tools.org/s3cmd)

## Quickstart

You'll need [CMake](http://www.cmake.org/) and [Boost](http://www.boost.org/) to build spread:

```bash
sudo apt-get install -y cmake libboost-all-dev
```

Then you can fetch and install spread:

```bash
git clone git@github.com:wavii/spread.git
cd spread
cmake . && make && sudo make install
```

## Examples

Given a hostfile on each machine with the format `host1\nhost2\nhost3\n...`, here is a distributed wordcount using spread:

```bash
tr ' ' '\n' < input | spread -f hosts | sort | uniq -c > output
```

Spread can also be used as a library for writing your own map programs in C++:

```c++
void map(tcp::endpoint& local_endpoint, vector<tcp::endpoint>& remote_endpoints)
{
   asio::io_service io;
   spreader<> s(io, local_endpoint, remote_endpoints, cout);
   string line;
   while (getline(cin, line))
      s << line.substr(line.find("USER: ")); // map the user in a log line
}
```

Spread works well with cloud concepts.  Here's an example of a bash script you could run on each host to operate on [S3](http://aws.amazon.com/s3/) data in parallel:

```bash
#!/bin/bash

# grab fleet index and size, perhaps this was provided on each machine by Chef
fleetsize=$(wc -l .fleet-hosts | awk '{print $1}')
fleetsize=$(echo "obase=16; $fleetsize" | bc)
id=$(cat .fleet-id)

for uri in `s3cmd ls s3://data-warehouse/ | awk '{print $4}'`; do
   md5id=$(echo $uri | md5sum | awk '{print toupper($1)}')
   remainder=$(echo "ibase=16; $md5id % $fleetsize" | bc)
   if [ $remainder -eq $id ]; then
      s3cmd get --no-progress --force $uri /mnt/input/
   fi
done

# now spread the input we've collected on each machine
cat /mnt/input/* | tr ' ' '\n' | spread -f hosts | sort | uniq -c > output
```

## License

Spread via the [MIT License](https://github.com/wavii/spread/blob/master/LICENSE).  Have fun!
