#ifndef NETSTREAM_HPP
#define NETSTREAM_HPP

/* Copyright (C) 2015 Chris Lamberson.
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#if defined(_WIN32)
# include <windows.h>
# include <winsock2.h>
#else
# include <sys/types.h>
# include <sys/socket.h>
# include <unistd.h>
# include <netdb.h>
# include <netinet/in.h>
# define SOCKET int
# define SD_SEND SHUT_WR
# define closesocket close
#endif

#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <iterator>

namespace net {
  template<typename _char_t, typename _traits>
  std::basic_ostream<_char_t, _traits>&
  endl(std::basic_ostream<_char_t, _traits>&);

  template<typename _char_t, typename _traits = std::char_traits<_char_t> >
  class basic_sockbuf : public std::basic_streambuf<_char_t, _traits> {
  private:
    enum { MAXBUF = 1024 };
    typedef std::basic_streambuf<_char_t, _traits> _parent_t;
    typedef SOCKET _sock_t;

  public:
    typedef _char_t char_type;
    typedef _traits traits_type;
    typedef typename _traits::int_type int_type;

    virtual ~basic_sockbuf();

  protected:
    basic_sockbuf();
    _sock_t getsock();
    void setsock(_sock_t);
    virtual int_type overflow(int_type = traits_type::eof());
    virtual int_type underflow();
    virtual int sync();

  private:
    _sock_t _sockfd;
    char_type _pbuf[MAXBUF];
    char_type _gbuf[MAXBUF];

    basic_sockbuf(basic_sockbuf const&);
  };

  template<typename _char_t, typename _traits = std::char_traits<_char_t> >
  class basic_httpbuf : public basic_sockbuf<_char_t, _traits> {
  public:
    typedef _char_t char_type;
    typedef _traits traits_type;

    basic_httpbuf();
    basic_httpbuf* open(char const*, std::ios_base::openmode);
  };

  template<typename _char_t, typename _traits = std::char_traits<_char_t> >
  class basic_isockstream
    : public virtual std::basic_istream<_char_t, _traits> {
  private:
    typedef std::basic_iostream<_char_t, _traits> _parent_type;
    typedef std::basic_string<_char_t, _traits> _string_type;
    typedef basic_sockbuf<_char_t, _traits> _buffer_type;

  public:
    typedef _char_t char_type;
    typedef _traits traits_type;

    basic_isockstream(_buffer_type*);
  };

  template<typename _char_t, typename _traits = std::char_traits<_char_t> >
  class basic_osockstream
    : public virtual std::basic_ostream<_char_t, _traits> {
  private:
    typedef std::basic_ostream<_char_t, _traits> _parent_type;
    typedef basic_sockbuf<_char_t, _traits> _buffer_type;
    typedef std::basic_string<_char_t, _traits> _string_type;

  public:
    typedef _char_t char_type;
    typedef _traits traits_type;

    basic_osockstream(_buffer_type*);
  };

  template<typename _char_t, typename _traits = std::char_traits<char> >
  class basic_iosockstream
    : public basic_osockstream<_char_t, _traits>,
      public basic_isockstream<_char_t, _traits> {
  public:
    basic_iosockstream(basic_sockbuf<_char_t, _traits>*);
  };

  template<typename T, typename U>
  class basic_httpstream;

  template<typename _char_t, typename _traits = std::char_traits<_char_t> >
  class basic_httprequest {
  public:
    typedef _char_t char_type;
    typedef _traits traits_type;
    typedef std::basic_string<char_type, traits_type> string_type;
    typedef std::map<string_type, string_type> headers_type;

    basic_httprequest();
    basic_httprequest(char_type const*);
    basic_httprequest(char_type const*, char_type const*);
    void add_header(char_type const*, char_type const*);
    string_type str() const;

  private:
    headers_type _headers;
    string_type _method;
    string_type _url;
  };

  template<typename _char_t, typename _traits = std::char_traits<_char_t> >
  struct basic_httpresponse {
    typedef _char_t char_type;
    typedef _traits traits_type;

    std::basic_ostringstream<char_type, traits_type> body;

    virtual ~basic_httpresponse();
  };

  template<typename _char_t, typename _traits = std::char_traits<_char_t> >
  class basic_httpstream : public basic_iosockstream<_char_t, _traits> {
  public:
    typedef _char_t char_type;
    typedef _traits traits_type;
    typedef basic_httpbuf<char_type, traits_type> buffer_type;
    typedef std::basic_istream<char_type, traits_type> istream_type;
    typedef std::basic_ostream<char_type, traits_type> ostream_type;
    typedef basic_httprequest<char_type, traits_type> request_type;
    typedef basic_httpresponse<char_type, traits_type> response_type;

    basic_httpstream();
    basic_httpstream(char const*,
                     std::ios_base::openmode = std::ios_base::in | std::ios_base::out);
    buffer_type* rdbuf() const;
    void open(char const*,
              std::ios_base::openmode = std::ios_base::in | std::ios_base::out);

  private:
    buffer_type _buf;
  };

  template<typename T, typename U>
  basic_httpstream<T,U>&
  operator<<(basic_httpstream<T,U>&, typename basic_httpstream<T,U>::request_type const&);

  template<typename T, typename U>
  basic_httpstream<T,U>&
  operator>>(basic_httpstream<T,U>&, typename basic_httpstream<T,U>::response_type&);

  typedef basic_sockbuf<char> sockbuf;
  typedef basic_sockbuf<wchar_t> wsockbuf;
  typedef basic_osockstream<char> osockstream;
  typedef basic_osockstream<wchar_t> wosockstream;
  typedef basic_isockstream<char> isockstream;
  typedef basic_isockstream<wchar_t> wisockstream;
  typedef basic_iosockstream<char> iosockstream;
  typedef basic_iosockstream<wchar_t> wiosockstream;
  typedef basic_httprequest<char> httprequest;
  typedef basic_httprequest<wchar_t> whttprequest;
  typedef basic_httpresponse<char> httpresponse;
  typedef basic_httpresponse<wchar_t> whttpresponse;
  typedef basic_httpstream<char> httpstream;
  typedef basic_httpstream<wchar_t> whttpstream;

  template<>
  std::basic_ostream<char,std::char_traits<char> >&
  endl<char,std::char_traits<char> >
  (std::basic_ostream<char,std::char_traits<char> >& os) {
    os.put('\r');
    os.put('\n');
    os.flush();
    return os;
  }

  template<typename T, typename U>
  std::basic_ostream<T,U>& endl(std::basic_ostream<T,U>& os) {
    os.put(os.widen('\r'));
    os.put(os.widen('\n'));
    os.flush();
    return os;
  }

  template<typename T, typename U>
  basic_sockbuf<T,U>::basic_sockbuf() : _sockfd(0) {
    std::memset(_pbuf, 0, MAXBUF);
    std::memset(_gbuf, 0, MAXBUF);
    this->setp(&_pbuf[0], &_pbuf[MAXBUF-1]);
    this->setg(&_gbuf[0], &_gbuf[0], &_gbuf[MAXBUF-1]);
  }

  template<typename T, typename U>
  basic_sockbuf<T,U>::~basic_sockbuf() {
    ::closesocket(_sockfd);
  }

  template<typename T, typename U>
  typename basic_sockbuf<T,U>::_sock_t
  basic_sockbuf<T,U>::getsock() {
    return _sockfd;
  }

  template<typename T, typename U>
  void basic_sockbuf<T,U>::setsock(_sock_t sockfd) {
    _sockfd = sockfd;
  }

  template<typename T, typename U>
  typename basic_sockbuf<T,U>::int_type
  basic_sockbuf<T,U>::overflow(int_type c) {
    int bytes_sent = ::send(_sockfd, _pbuf, traits_type::length(_pbuf), 0);
    _pbuf[0] = c;
    std::memset(&_pbuf[1], 0, MAXBUF-1);
    this->setp(&_pbuf[0], &_pbuf[MAXBUF-1]);
  }

  template<typename T, typename U>
  typename basic_sockbuf<T,U>::int_type
  basic_sockbuf<T,U>::underflow() {
    int bytes_received = ::recv(_sockfd, _gbuf, MAXBUF-1, 0);
    _gbuf[bytes_received] = 0;
    if (bytes_received == 0) {
      return traits_type::eof();
    }
    this->setg(&_gbuf[0], &_gbuf[0], &_gbuf[bytes_received]);
    return traits_type::to_int_type(_gbuf[0]);
  }

  template<typename T, typename U>
  int basic_sockbuf<T,U>::sync() {
    int bytes_sent = ::send(_sockfd, _pbuf, traits_type::length(_pbuf), 0);
    std::memset(&_pbuf[0], 0, MAXBUF);
    this->setp(&_pbuf[0], &_pbuf[MAXBUF-1]);
    return 0;
  }

  template<typename T, typename U>
  basic_httpbuf<T,U>::basic_httpbuf() {}

  template<typename T, typename U>
  basic_httpbuf<T,U>* basic_httpbuf<T,U>::open(char const* url,
                                               std::ios_base::openmode mode) {
    struct addrinfo hints, *info, *p;
    int result, sockfd;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ((result = ::getaddrinfo(url, "http", &hints, &info)) != 0) {
      return NULL;
    }
    for (p = info; p != NULL; p = p->ai_next) {
      if ((sockfd = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol))
          == -1) {
        continue;
      }
      if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
        ::closesocket(sockfd);
        continue;
      }
      break;
    }
    if (p == NULL) {
      return NULL;
    }
    ::freeaddrinfo(info);
    this->setsock(sockfd);
    return this;
  }

  template<typename T, typename U>
  basic_isockstream<T,U>::basic_isockstream(basic_sockbuf<T,U>* buf) {
    this->init(buf);
  }

  template<typename T, typename U>
  basic_osockstream<T,U>::basic_osockstream(basic_sockbuf<T,U>* buf) {
    this->init(buf);
  }

  template<typename T, typename U>
  basic_iosockstream<T,U>::basic_iosockstream(basic_sockbuf<T,U>* buf)
    : basic_isockstream<T,U>(buf), basic_osockstream<T,U>(buf) {}

  template<typename T, typename U>
  basic_httprequest<T,U>::basic_httprequest() : _method("GET"), _url("/") {}

  template<typename T, typename U>
  basic_httprequest<T,U>::basic_httprequest(char_type const* url)
    : _method("GET"), _url(url) {}

  template<typename T, typename U>
  basic_httprequest<T,U>::basic_httprequest(char_type const* method,
                                            char_type const* url)
    : _method(method), _url(url) {}

  template<typename T, typename U>
  void basic_httprequest<T,U>::add_header(char_type const* name,
                                          char_type const* value) {
    _headers[name] = value;
  }

  template<typename T, typename U>
  typename basic_httprequest<T,U>::string_type
  basic_httprequest<T,U>::str() const {
    std::ostringstream oss;
    oss << _method << " " << _url << " HTTP/1.1" << endl;
    for (typename headers_type::const_iterator it = _headers.begin();
         it != _headers.end();
         ++it) {
      oss << it->first << ": " << it->second << endl;
    }
    oss << endl;
    return string_type(oss.str());
  }

  template<typename T, typename U>
  basic_httpresponse<T,U>::~basic_httpresponse() {}

  template<typename T, typename U>
  basic_httpstream<T,U>::basic_httpstream() : basic_iosockstream<T,U>(&_buf) {}

  template<typename T, typename U>
  basic_httpstream<T,U>::basic_httpstream(char const* url,
                                          std::ios_base::openmode mode)
    : basic_iosockstream<T,U>(&_buf) {
    if (!_buf.open(url, mode)) {
      this->setstate(std::ios_base::failbit);
    } else {
      this->clear();
    }
  }

  template<typename T, typename U>
  void basic_httpstream<T,U>::open(char const* url,
                                   std::ios_base::openmode mode) {
    _buf.open(url, mode);
  }

  template<typename T, typename U>
  basic_httpstream<T,U>&
  operator<<(basic_httpstream<T,U>& hs,
             typename basic_httpstream<T,U>::request_type const& req) {
    hs << req.str();
    hs.flush();
    return hs;
  }

  template<typename T, typename U>
  basic_httpstream<T,U>&
  operator>>(basic_httpstream<T,U>& hs,
             typename basic_httpstream<T,U>::response_type& res) {
    std::copy(std::istreambuf_iterator<T>(hs),
              std::istreambuf_iterator<T>(),
              std::ostream_iterator<T>(res.body));
    return hs;
  }
}

#endif
