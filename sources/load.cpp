// Copyright 2022 Shevelyova Darya photodoshfy@gmail.com

#include <crawler.hpp>

int loading_html() {
  try
  {
    auto const host = "www.boost.org";
    auto const port = "443"; // https - 443, http - 80
    auto const target = "/doc/libs/1_69_0/libs/beast/example/http/client/"
        "sync-ssl/http_client_sync_ssl.cpp"; //
    int version = 11;

    // The io_context is required for all I/O
    boost::asio::io_context ioc;

    // The SSL context is required, and holds certificates
    ssl::context ctx{ ssl::context::sslv23_client };

    // This holds the root certificate used for verification
    load_root_certificates(ctx);

    // These objects perform our I/O
    boost::asio::ip::tcp::resolver resolver{ ioc };
    ssl::stream<boost::asio::ip::tcp::socket> stream{ ioc, ctx };

    // Set SNI Hostname (many hosts need this to handshake successfully)
    if (!SSL_set_tlsext_host_name(stream.native_handle(), host))
    {
      boost::system::error_code ec{ static_cast<int>(::ERR_get_error()),
                                   boost::asio::error::get_ssl_category() };
      throw boost::system::system_error{ ec };
    }

    // Look up the domain name
    auto const results = resolver.resolve(host, port);

    // Make the connection on the IP address we get from a lookup
    boost::asio::connect(stream.next_layer(), results.begin(),
                         results.end());

    // Perform the SSL handshake
    stream.handshake(ssl::stream_base::client);

    // Set up an HTTP GET request message
    boost::beast::http::request<boost::beast::http::string_body> req{
        boost::beast::http::verb::get, target,
        version };
    req.set(boost::beast::http::field::host, host);
    req.set(boost::beast::http::field::user_agent,
            BOOST_BEAST_VERSION_STRING);

    // Send the HTTP request to the remote host
    boost::beast::http::write(stream, req);

    // This buffer is used for reading and must be persisted
    boost::beast::flat_buffer buffer;

    // Declare a container to hold the response
    boost::beast::http::response<boost::beast::http::dynamic_body> res;

    // Receive the HTTP response
    boost::beast::http::read(stream, buffer, res);

    // Write the message to out
    std::ofstream ofs{"out.txt"}; // запись html-страницы в файл
    ofs << res;
    ofs.close();

    // Gracefully close the stream
    boost::system::error_code ec;
    stream.shutdown(ec);
    if (ec == boost::asio::error::eof)
    {
      ec.assign(0, ec.category());
    }
    if (ec)
      throw boost::system::system_error{ ec };

    // If we get here then the connection is closed gracefully
  }
  catch (std::exception const& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int main(){
  loading_html();
}
