// Copyright 2022 Shevelyova Darya photodoshfy@gmail.com

#include <crawler.hpp>
#include <downloader.hpp>
#include <parser.hpp>

void Crawler::start(const std::string& url, const int& num_loaders,
                    const int& num_parsers) {
  Downloader downloader(num_loaders);
  Parser parser(num_parsers, img);
  std::deque<std::string> first_all_links;
  std::deque<Html> first_htmls;
  first_all_links.push_back(url);
  levels.emplace_back(first_all_links, first_htmls);
  for (int i = 0; i < depth; ++i){
    std::deque<std::string> next_all_links;
    std::deque<Html> next_htmls;
    levels.emplace_back(next_all_links, next_htmls);
    while (!levels[i].first.empty() || !levels[i].second.empty()){
      if (!levels[i].first.empty()) {
        levels[i].second.push_back(downloader.load_html_list(
            levels[i].first.front()));
        levels[i].first.pop_front();
      }
      if (!levels[i].second.empty()){
        for (auto & a : parser.parse(levels[i].second.front(),
                                    (i + 1 == depth))){
          levels[i+1].first.push_back(a);
        }
        levels[i].second.pop_front();
      }
    }
  }
}



Crawler::~Crawler() {
  print_in_file(file);
}
void Crawler::print_in_file(const std::string& file_path) {
  std::ofstream ofs{file_path};
  for (const auto& img_ : img){
    ofs << img_ << std::endl;
  }
  ofs.close();
}
