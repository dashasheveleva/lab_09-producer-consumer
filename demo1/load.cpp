// Copyright 2022 Shevelyova Darya photodoshfy@gmail.com

#include <crawler.hpp>

int loading_html() {
  try
  {
    auto const host = "www.reddit.com";
    auto const port = "443"; // https - 443, http - 80
    auto const target = "/r/aww/"; //
    int version = 11;

    // io_context требуется для всех операций ввода/вывода
    boost::asio::io_context ioc;

    // Требуется контекст SSL и содержит сертификаты
    ssl::context ctx{ ssl::context::sslv23_client };

    // Здесь хранится корневой сертификат, используемый для проверки
    load_root_certificates(ctx);

    // Эти объекты выполняют наш ввод-вывод
    boost::asio::ip::tcp::resolver resolver{ ioc };
    ssl::stream<boost::asio::ip::tcp::socket> stream{ ioc, ctx };

    // Установить имя хоста SNI (многим хостам это нужно для успешного рукопожатия)
    if (!SSL_set_tlsext_host_name(stream.native_handle(), host))
    {
      boost::system::error_code ec{ static_cast<int>(::ERR_get_error()),
                                   boost::asio::error::get_ssl_category() };
      throw boost::system::system_error{ ec };
    }

    // Ищем доменное имя
    auto const results = resolver.resolve(host, port);

    // Устанавливаем соединение по IP-адресу, полученному из поиска
    boost::asio::connect(stream.next_layer(), results.begin(), results.end());

    // Выполняем рукопожатие SSL
    stream.handshake(ssl::stream_base::client);

    // Настройка сообщения запроса HTTP GET
    boost::beast::http::request<boost::beast::http::string_body> req{
        boost::beast::http::verb::get, target,
        version };
    req.set(boost::beast::http::field::host, host);
    req.set(boost::beast::http::field::user_agent,
            BOOST_BEAST_VERSION_STRING);

    // Отправляем HTTP-запрос на удаленный хост
    boost::beast::http::write(stream, req);

    // Этот буфер используется для чтения и должен быть сохранен
    boost::beast::flat_buffer buffer;

    // Объявить контейнер для хранения ответа
    boost::beast::http::response<boost::beast::http::dynamic_body> res;

    // Получаем HTTP-ответ
    boost::beast::http::read(stream, buffer, res);

    // Записываем сообщение на стандартный вывод
    std::ofstream ofs{"out.txt"}; // запись html-страницы в файл
    ofs << res;
    ofs.close();

    /// Изящно закрыть поток
    boost::system::error_code ec;
    stream.shutdown(ec);
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
