// Copyright 2022 Shevelyova Darya photodoshfy@gmail.com

#include <crawler.hpp>

int main(int argc, const char *argv[])
{
  try
  {
    boost::program_options::options_description desc{"Options"};
    desc.add_options()
    ("help,h", "Help screen")
    ("url,u", boost::program_options::value<std::string>(), "URL website")
    ("depth,d", boost::program_options::value<int>()->default_value(0),
                "Depth find")
    ("network_threads,n", boost::program_options::
                                                 value<int>()->default_value(1),
                          "Number of threads for loading webpages")
    ("parser_threads,p", boost::program_options::value<int>()->default_value(1),
                         "Number of threads for processing webpages")
    ("output,o", boost::program_options::value<std::string>(),
                 "Path to outout file");

    boost::program_options::variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help"))
      std::cout << desc << '\n';
    else if (vm.count("url") && vm.count("depth") &&
             vm.count("network_threads")
             && vm.count("parser_threads") && vm.count("output")){
      Crawler crawl(vm["depth"].as<int>(), vm["output"].as<std::string>());
      crawl.start(vm["url"].as<std::string>(),
                  vm["network_threads"].as<int>(),
                  vm["parser_threads"].as<int>());
    } else
      throw boost::program_options::error(
          "**********BAD SYNTAX**********\n"
          "Look to --help or -h and write all options");
  } catch (const boost::program_options::error &ex)
  {
    std::cerr << ex.what() << '\n';
  }
  //  Crawl crawl(1, "../cmake-build-debug/out.txt");
  //  crawl.start("http://wokandbox.ru/", 1, 1);
}