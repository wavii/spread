# Spread

Spread is a missing link in the unix toolchain to do simple distributed data mining.  Spread partitions data according to key across a fleet.

Here is a distributed wordcount using spread:

```
tr ' ' '\n' < input | spread -f hosts | sort | uniq -c > output
```

Spread shines when used in conjunction with fleet management/coordination/persistence tools like:

* [Chef](http://www.opscode.com/chef/)
* [gnu parallel](http://www.gnu.org/software/parallel/), [xargs -P](http://matpalm.com/blog/2009/11/06/xargs-parallel-execution/), or [dsh](http://www.netfort.gr.jp/~dancer/software/dsh.html.en)
* [s3cmd](http://s3tools.org/s3cmd)

## Quickstart

You'll need [CMake](http://www.cmake.org/) and [Boost](http://www.boost.org/) to build spread:

```
sudo apt-get install -y cmake libboost-all-dev
```

Then you can fetch and install spread:

```
git clone git@github.com:wavii/spread.git
cd spread
cmake . && make && sudo make install
```

## License

Spread via the [MIT License](https://github.com/wavii/spread/blob/master/LICENSE).  Have fun!
