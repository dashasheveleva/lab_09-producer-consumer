// Copyright 2022 Shevelyova Darya photodoshfy@gmail.com

#include <downloader.hpp>

Html Downloader::loading_https(std::string& host, const std::string& target) {
  try
  {
    const std::string port = "443"; // https - 443, http - 80
    const int version = 11;
    boost::asio::io_context ioc;
    ssl::context ctx{ ssl::context::sslv23_client };
    load_root_certificates(ctx);
    boost::asio::ip::tcp::resolver resolver{ ioc };
    ssl::stream<boost::asio::ip::tcp::socket> stream{ ioc, ctx };
    if (!SSL_set_tlsext_host_name(stream.native_handle(),
                                  host.c_str()))
    {
      boost::system::error_code ec{ static_cast<int>(::ERR_get_error()),
                                   boost::asio::error::get_ssl_category() };
      throw boost::system::system_error{ ec };
    }
    auto const results = resolver.resolve(host, port);
    boost::asio::connect(stream.next_layer(), results.begin(),
                         results.end());
    stream.handshake(ssl::stream_base::client);
    boost::beast::http::request<boost::beast::http::string_body> req{
        boost::beast::http::verb::get, target,
        version };
    req.set(boost::beast::http::field::host, host);
    req.set(boost::beast::http::field::user_agent,
            BOOST_BEAST_VERSION_STRING);
    boost::beast::http::write(stream, req);
    boost::beast::flat_buffer buffer;
    boost::beast::http::response<boost::beast::http::dynamic_body> res;
    boost::beast::http::read(stream, buffer, res);

    std::string string_body = boost::beast::buffers_to_string(res.body()
                                                                  .data());
    boost::system::error_code ec;
    stream.shutdown(ec);
    if (ec == boost::asio::error::eof)
    {
      ec.assign(0, ec.category());
    }
    if (ec) {
      throw boost::system::system_error{ec};
    }
    return {"https", host, string_body};
    // If we get here then the connection is closed gracefully
  }
  catch (std::exception const& e)
  {
    std::cerr << "Error on https: " << e.what() << std::endl;
    return Html{};
  }
}

Html Downloader::loading_http(const std::string& host,
                              const std::string& target) {
  try
  {
    const std::string  port = "80"; // https - 443, http - 80
    const int version = 11;
    boost::asio::io_context ioc;
    boost::asio::ip::tcp::socket socket{ ioc };
    boost::asio::ip::tcp::resolver resolver{ ioc };
    auto const results = resolver.resolve(host, port);
    boost::asio::connect(socket, results.begin(), results.end());
    boost::beast::http::request<boost::beast::http::string_body> req{
        boost::beast::http::verb::get, target,
        version };
    req.set(boost::beast::http::field::host, host);
    req.set(boost::beast::http::field::user_agent,
            BOOST_BEAST_VERSION_STRING);
    boost::beast::http::write(socket, req);
    boost::beast::flat_buffer buffer;
    boost::beast::http::response<boost::beast::http::dynamic_body> res;
    boost::beast::http::read(socket, buffer, res);
    std::string string_body = boost::beast::buffers_to_string(res.body()
                                                                  .data());
    boost::system::error_code ec;
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    if (ec == boost::asio::error::eof)
    {
      ec.assign(0, ec.category());
    }
    if (ec)
      throw boost::system::system_error{ ec };
    return {"http", host, string_body};
  }
  catch (std::exception const& e)
  {
    std::cerr << "Error on http: " << e.what() << std::endl;
    return Html{};
  }
}

Html Downloader::load_html_list(const std::string& url_) {
  auto res = loaders.enqueue([&](const std::string& url){
    std::string host = url
                       .substr(url.find(':')+3)
                       .substr(0, url.substr(url.find(':')+3).find('/'));
    if (url.find(':') == 5){
      std::string target = (url == std::string{"https://"
                            + host + url.substr(url.find(':')+3)
                                        .substr(url.substr(url.find(':')+3)
                                                           .find('/'))}) ? "/" :
              url.substr(url.find(':')+3).substr(
                  url.substr(url.find(':')+3).find('/'));
      return loading_https(host, target);
    } else if (url.find(':') == 4) {
      std::string target =
          (url == std::string{
                      "http://" + host + url.substr(url.find(':')+3).substr(
                                                 url.substr(url.find(':')+3)
                                                     .find('/'))}) ? "/" :
              url.substr(url.find(':')+3).substr(url.substr(url.find(':')+3)
                                                     .find('/'));
      return loading_http(host, target);
    } else {
      return Html{};
    }
  }, url_);
  return res.get();
}
