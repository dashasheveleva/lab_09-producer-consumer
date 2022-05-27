// Copyright 2022 Shevelyova Darya photodoshfy@gmail.com

#ifndef INCLUDE_DOWNLOADER_HPP_
#define INCLUDE_DOWNLOADER_HPP_

#include <crawler.hpp>
#include <string>

class Downloader{
 public:
  explicit Downloader(const int& num_workers) : loaders(num_workers){}

  Html load_html_list(const std::string& url);

  static Html loading_https(std::string& host, const std::string& target);

  static Html loading_http(const std::string& host, const std::string& target);

 private:
  ThreadPool loaders;
};
#endif  // INCLUDE_DOWNLOADER_HPP_
