// Copyright 2022 Shevelyova Darya photodoshfy@gmail.com

#ifndef INCLUDE_CRAWLER_HPP_
#define INCLUDE_CRAWLER_HPP_

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <chrono>
#include <queue>
#include <deque>
#include <gumbo.h>
#include <boost/program_options.hpp>
#include "../third-party/root_certificates.h"
#include "../third-party/ThreadPool.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <utility>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <cstdlib>


struct Html{
  std::string protocol{};
  std::string host{};
  std::string html{};
};

class Crawler{
 public:
  Crawler(int depth_, std::string path) :
                                          depth(depth_),
                                          file(std::move(path)){}
  ~Crawler();

  void start(const std::string& url, const int& num_loaders, const int&
                                                                 num_parsers);

  void print_in_file(const std::string& file_path);
 private:
  int depth;
  std::string file;
  std::vector<std::string> img;
  std::deque<std::pair<std::deque<std::string>, std::deque<Html>>> levels;
};
#endif  // INCLUDE_CRAWLER_HPP_
