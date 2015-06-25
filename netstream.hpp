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

#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <iterator>
#include <limits>

#define __BEGIN_NETSTREAM_NAMESPACE namespace net {
#define __END_NETSTREAM_NAMESPACE }

__BEGIN_NETSTREAM_NAMESPACE

// ============= //
// basic_sockbuf //
// ============= //

template<typename _CharT, typename _Traits = std::char_traits<_CharT> >
class basic_sockbuf : public std::basic_streambuf<_CharT, _Traits>
{
public:
  enum { MAXBUF = 1024 };

  typedef _CharT char_type;
  typedef _Traits traits_type;
  typedef typename traits_type::int_type int_type;
  typedef SOCKET __socket_type;

  virtual ~basic_sockbuf();
  basic_sockbuf* close();

protected:
  basic_sockbuf();
  basic_sockbuf(basic_sockbuf const&);
  basic_sockbuf& operator=(basic_sockbuf const&);

  __socket_type sock();
  void sock(__socket_type);
  virtual int_type overflow(int_type = traits_type::eof());
  virtual int_type underflow();
  virtual int sync();

private:
  __socket_type __sockfd;
  char_type __pbuf[MAXBUF];
  char_type __gbuf[MAXBUF];
};

// ============= //
// basic_httpbuf //
// ============= //

template<typename _CharT, typename _Traits = std::char_traits<_CharT> >
class basic_httpbuf : public basic_sockbuf<_CharT, _Traits>
{
public:
  typedef _CharT char_type;
  typedef _Traits traits_type;
  typedef std::ios_base::openmode __openmode_type;

  basic_httpbuf();
  virtual ~basic_httpbuf();
  basic_httpbuf* open(char const*, __openmode_type);
  basic_httpbuf* open(std::string const&, __openmode_type);

private:
  basic_httpbuf* __open(char const*);
};

// ================= //
// basic_isockstream //
// ================= //

template<typename _CharT, typename _Traits = std::char_traits<_CharT> >
class basic_isockstream : public virtual std::basic_istream<_CharT, _Traits>
{
public:
  typedef _CharT char_type;
  typedef _Traits traits_type;
  typedef basic_sockbuf<char_type, traits_type> __buffer_type;

  explicit basic_isockstream(__buffer_type*);
  virtual ~basic_isockstream();

protected:
  basic_isockstream(basic_isockstream const&);
  basic_isockstream& operator=(basic_isockstream const&);
};

// ================= //
// basic_osockstream //
// ================= //

template<typename _CharT, typename _Traits = std::char_traits<_CharT> >
class basic_osockstream : public virtual std::basic_ostream<_CharT, _Traits>
{
public:
  typedef _CharT char_type;
  typedef _Traits traits_type;
  typedef basic_sockbuf<char_type, traits_type> __buffer_type;

  explicit basic_osockstream(__buffer_type*);
  virtual ~basic_osockstream();

protected:
  basic_osockstream(basic_osockstream const&);
  basic_osockstream& operator=(basic_osockstream const&);
};

// ================== //
// basic_iosockstream //
// ================== //

template<typename _CharT, typename _Traits = std::char_traits<_CharT> >
class basic_iosockstream : public basic_osockstream<_CharT, _Traits>,
                           public basic_isockstream<_CharT, _Traits>
{
public:
  typedef _CharT char_type;
  typedef _Traits traits_type;
  typedef basic_sockbuf<char_type, traits_type> __buffer_type;

  basic_iosockstream(__buffer_type*);
};

// ================= //
// basic_httprequest //
// ================= //

template<typename _CharT, typename _Traits = std::char_traits<_CharT> >
class basic_httprequest
{
public:
  typedef _CharT char_type;
  typedef _Traits traits_type;
  typedef std::basic_ostream<char_type, traits_type> __ostream_type;
  typedef std::basic_string<char_type, traits_type> __string_type;
  typedef std::map<__string_type, __string_type> __headers_type;

  basic_httprequest();
  basic_httprequest(char_type const*);
  basic_httprequest(char_type const*, char_type const*);
  void add_header(char_type const*, char_type const*);
  void write(__ostream_type&) const;

private:
  __string_type __method;
  __string_type __url;
  __headers_type __headers;
};

// ================== //
// basic_httpresponse //
// ================== //

template<typename _CharT, typename _Traits = std::char_traits<_CharT> >
class basic_httpresponse
{
public:
  typedef _CharT char_type;
  typedef _Traits traits_type;
  typedef std::basic_istream<char_type, traits_type> __istream_type;
  typedef std::basic_string<char_type, traits_type> __string_type;
  typedef std::pair<int, __string_type> __status_type;
  typedef std::map<__string_type, __string_type> __headers_type;
  typedef std::basic_stringstream<char_type, traits_type> __body_type;

  virtual ~basic_httpresponse();
  void read(__istream_type&);
  __status_type status();
  __headers_type headers();
  __body_type body();

private:
  __status_type __status;
  __headers_type __headers;
  __body_type __body;
};

// ================ //
// basic_httpstream //
// ================ //

template<typename _CharT, typename _Traits = std::char_traits<_CharT> >
class basic_httpstream : public basic_iosockstream<_CharT, _Traits>
{
public:
  typedef _CharT char_type;
  typedef _Traits traits_type;
  typedef basic_httpbuf<char_type, traits_type> __buffer_type;
  typedef std::ios_base::openmode __openmode_type;
  using std::ios_base::in;
  using std::ios_base::out;

  basic_httpstream();
  explicit basic_httpstream(char const*, __openmode_type = in | out);
  explicit basic_httpstream(std::string const&, __openmode_type = in | out);
  void open(char const*, __openmode_type = in | out);
  void close();
  __buffer_type* rdbuf() const;

private:
  __buffer_type __buf;
};

template<typename _CharT, typename _Traits>
std::basic_ostream<_CharT, _Traits>&
endl(std::basic_ostream<_CharT, _Traits>&);

template<typename _CharT, typename _Traits>
std::basic_istream<_CharT, _Traits>&
getline(std::basic_istream<_CharT, _Traits>&,
        std::basic_string<_CharT, _Traits>&);

template<typename _CharT, typename _Traits>
basic_httpstream<_CharT, _Traits>&
operator<<(basic_httpstream<_CharT, _Traits>&,
           basic_httprequest<_CharT, _Traits> const&);

template<typename _CharT, typename _Traits>
basic_httpstream<_CharT, _Traits>&
operator>>(basic_httpstream<_CharT, _Traits>&,
           basic_httpresponse<_CharT, _Traits>&);

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

template<typename _CharT, typename _Traits>
std::basic_ostream<_CharT, _Traits>&
endl(std::basic_ostream<_CharT, _Traits>& __os)
{
  __os.put(__os.widen('\r'));
  __os.put(__os.widen('\n'));
  __os.flush();
  return __os;
}

template<>
std::basic_ostream<char>&
endl<char>(std::basic_ostream<char>& __os)
{
  __os.put('\r');
  __os.put('\n');
  __os.flush();
  return __os;
}

template<typename _CharT, typename _Traits>
basic_sockbuf<_CharT, _Traits>::basic_sockbuf() : __sockfd(0)
{
  std::memset(__pbuf, 0, MAXBUF);
  std::memset(__gbuf, 0, MAXBUF);
  this->setp(&__pbuf[0], &__pbuf[MAXBUF-1]);
  this->setg(&__gbuf[0], &__gbuf[0], &__gbuf[MAXBUF-1]);
}

template<typename _CharT, typename _Traits>
basic_sockbuf<_CharT, _Traits>::~basic_sockbuf()
{
  close();
}

template<typename _CharT, typename _Traits>
basic_sockbuf<_CharT, _Traits>*
basic_sockbuf<_CharT, _Traits>::close()
{
  basic_sockbuf* __ret = NULL;
  if (::closesocket(__sockfd) == 0)
    __ret = this;
  return __ret;
}

template<typename _CharT, typename _Traits>
typename basic_sockbuf<_CharT, _Traits>::__socket_type
basic_sockbuf<_CharT, _Traits>::sock()
{
  return __sockfd;
}

template<typename _CharT, typename _Traits>
void
basic_sockbuf<_CharT, _Traits>::sock(__socket_type __sockfd)
{
  this->__sockfd = __sockfd;
}

template<typename _CharT, typename _Traits>
typename basic_sockbuf<_CharT, _Traits>::int_type
basic_sockbuf<_CharT, _Traits>::overflow(int_type __c)
{
  ::send(__sockfd, __pbuf, traits_type::length(__pbuf), 0);
  __pbuf[0] = __c;
  std::memset(&__pbuf[1], 0, MAXBUF-1);
  this->setp(&__pbuf[0], &__pbuf[MAXBUF-1]);
  return __c;
}

template<typename _CharT, typename _Traits>
typename basic_sockbuf<_CharT, _Traits>::int_type
basic_sockbuf<_CharT, _Traits>::underflow()
{
  int bytes_received = ::recv(__sockfd, __gbuf, MAXBUF-1, 0);
  __gbuf[bytes_received] = 0;
  if (bytes_received == 0) {
    return traits_type::eof();
  }
  this->setg(&__gbuf[0], &__gbuf[0], &__gbuf[bytes_received]);
  return traits_type::to_int_type(__gbuf[0]);
}

template<typename _CharT, typename _Traits>
int
basic_sockbuf<_CharT, _Traits>::sync()
{
  ::send(__sockfd, __pbuf, traits_type::length(__pbuf), 0);
  std::memset(&__pbuf[0], 0, MAXBUF);
  this->setp(&__pbuf[0], &__pbuf[MAXBUF-1]);
  return 0;
}

template<typename _CharT, typename _Traits>
basic_httpbuf<_CharT, _Traits>::basic_httpbuf()
  : basic_sockbuf<_CharT, _Traits>()
{}

template<typename _CharT, typename _Traits>
basic_httpbuf<_CharT, _Traits>::~basic_httpbuf()
{}

template<typename _CharT, typename _Traits>
basic_httpbuf<_CharT, _Traits>*
basic_httpbuf<_CharT, _Traits>::open(char const* __u, __openmode_type)
{
  return __open(__u);
}

template<typename _CharT, typename _Traits>
basic_httpbuf<_CharT, _Traits>*
basic_httpbuf<_CharT, _Traits>::open(std::string const& __u, __openmode_type)
{
  return __open(__u.c_str());
}

template<typename _CharT, typename _Traits>
basic_httpbuf<_CharT, _Traits>*
basic_httpbuf<_CharT, _Traits>::__open(char const* __url)
{
  struct addrinfo hints, *info, *p;
  int result, fd;

  std::memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((result = ::getaddrinfo(__url, "http", &hints, &info)) != 0)
    return NULL;

  for (p = info; p != NULL; p = p->ai_next) {
    if ((fd = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
      continue;

    if (connect(fd, p->ai_addr, p->ai_addrlen) == -1) {
      ::closesocket(fd);
      continue;
    }

    break;
  }

  if (p == NULL)
    return NULL;

  ::freeaddrinfo(info);
  this->sock(fd);

  return this;
}

template<typename _CharT, typename _Traits>
basic_isockstream<_CharT, _Traits>::basic_isockstream(__buffer_type* __buf)
  : std::basic_istream<_CharT, _Traits>(__buf)
{}

template<typename _CharT, typename _Traits>
basic_isockstream<_CharT, _Traits>::~basic_isockstream()
{}

template<typename _CharT, typename _Traits>
basic_osockstream<_CharT, _Traits>::basic_osockstream(__buffer_type* __buf)
  : std::basic_ostream<_CharT, _Traits>(__buf)
{}

template<typename _CharT, typename _Traits>
basic_osockstream<_CharT, _Traits>::~basic_osockstream()
{}

template<typename _CharT, typename _Traits>
basic_iosockstream<_CharT, _Traits>::basic_iosockstream(__buffer_type* __buf)
  : basic_osockstream<_CharT, _Traits>(__buf),
    basic_isockstream<_CharT, _Traits>(__buf)
{}

template<typename _CharT, typename _Traits>
basic_httprequest<_CharT, _Traits>::basic_httprequest()
  : __method("GET"),
    __url("/")
{}

template<typename _CharT, typename _Traits>
basic_httprequest<_CharT, _Traits>::basic_httprequest(char_type const* __u)
  : __method("GET"),
    __url(__u)
{}

template<typename _CharT, typename _Traits>
basic_httprequest<_CharT, _Traits>::basic_httprequest(char_type const* __m,
                                                      char_type const* __u)
  : __method(__m),
    __url(__u)
{}

template<typename _CharT, typename _Traits>
void
basic_httprequest<_CharT, _Traits>::add_header(char_type const* __name,
                                               char_type const* __value)
{
  __headers[__name] = __value;
}

template<typename _CharT, typename _Traits>
void
basic_httprequest<_CharT, _Traits>::write(__ostream_type& __os) const
{
  __os << __method << " " << __url << " HTTP/1.1" << endl;
  for (typename __headers_type::const_iterator it = __headers.begin();
       it != __headers.end(); ++it) {
    __os << it->first << ": " << it->second << endl;
  }
  __os << endl;
}

template<typename _CharT, typename _Traits>
basic_httpresponse<_CharT, _Traits>::~basic_httpresponse()
{}

template<typename _CharT, typename _Traits>
void
basic_httpresponse<_CharT, _Traits>::read(__istream_type& __is)
{
  __is.ignore(std::numeric_limits<std::streamsize>::max(),
              traits_type::to_int_type(' '));
  __is >> __status.first;
  __is.get();
  net::getline(__is, __status.second);
  __string_type s;
  net::getline(__is, s);
  while (!s.empty()) {
    typename __string_type::size_type pos1(s.find_first_of(':'));
    typename __string_type::size_type pos2(s.find_first_not_of(' ', pos1+1));
    __string_type name(s.substr(0, pos1));
    __string_type value(s.substr(pos2, __string_type::npos));
    __headers.insert(std::make_pair(name, value));
    net::getline(__is, s);
  }
  std::copy(std::istreambuf_iterator<char_type>(__is),
            std::istreambuf_iterator<char_type>(),
            std::ostream_iterator<char_type>(body));
}

template<typename _CharT, typename _Traits>
typename basic_httpresponse<_CharT, _Traits>::__status_type
basic_httpresponse<_CharT, _Traits>::status()
{
  return __status;
}

template<typename _CharT, typename _Traits>
typename basic_httpresponse<_CharT, _Traits>::__headers_type
basic_httpresponse<_CharT, _Traits>::headers()
{
  return __headers;
}

template<typename _CharT, typename _Traits>
typename basic_httpresponse<_CharT, _Traits>::__body_type
basic_httpresponse<_CharT, _Traits>::body()
{
  return __body;
}

template<typename _CharT, typename _Traits>
basic_httpstream<_CharT, _Traits>::basic_httpstream()
  : basic_iosockstream<_CharT, _Traits>(&__buf)
{}

template<typename _CharT, typename _Traits>
basic_httpstream<_CharT, _Traits>::basic_httpstream(char const* __url,
                                                    __openmode_type __mode)
  : basic_iosockstream<_CharT, _Traits>(&__buf)
{
  if (!__buf.open(__url, __mode))
    this->setstate(std::ios_base::failbit);
}

template<typename _CharT, typename _Traits>
basic_httpstream<_CharT, _Traits>::basic_httpstream(std::string const& __url,
                                                    __openmode_type __mode)
{
  if (!__buf.open(__url, __mode))
    this->setstate(std::ios_base::failbit);
}

template<typename _CharT, typename _Traits>
void
basic_httpstream<_CharT, _Traits>::open(char const* __url,
                                        __openmode_type __mode)
{
  if (!__buf.open(__url, __mode))
    this->setstate(std::ios_base::failbit);
}

template<typename _CharT, typename _Traits>
typename basic_httpstream<_CharT, _Traits>::__buffer_type*
basic_httpstream<_CharT, _Traits>::rdbuf() const
{
  return const_cast<__buffer_type*>(&__buf);
}

template<typename _CharT, typename _Traits>
void
basic_httpstream<_CharT, _Traits>::close()
{
  if (!__buf.close())
    this->setstate(std::ios_base::failbit);
}

template<typename _CharT, typename _Traits>
std::basic_istream<_CharT, _Traits>&
getline(std::basic_istream<_CharT, _Traits>& __is,
        std::basic_string<_CharT, _Traits>& __s)
{
  typedef std::basic_streambuf<_CharT, _Traits> streambuf_t;
  typedef std::basic_istream<_CharT, _Traits> istream_t;
  typedef typename istream_t::traits_type traits_t;
  typedef typename istream_t::sentry sentry_t;

  sentry_t sentry(__is, true);
  streambuf_t* sb = __is.rdbuf();

  __s.clear();

  while (1) {
    int ch = sb->sbumpc();

    if (traits_t::eq_int_type(ch, traits_t::to_int_type('\n'))) {
      return __is;
    } else if (traits_t::eq_int_type(ch, traits_t::to_int_type('\r'))) {
      if (traits_t::eq_int_type(sb->sgetc(), traits_t::to_int_type('\n'))) {
        sb->sbumpc();
      }
      return __is;
    } else if (traits_t::eq_int_type(ch, traits_t::eof())) {
      if (__s.empty()) {
        __is.setstate(std::ios_base::eofbit);
      }
      return __is;
    } else {
      __s += traits_t::to_char_type(ch);
    }
  }
}

template<typename _CharT, typename _Traits>
basic_httpstream<_CharT, _Traits>&
operator<<(basic_httpstream<_CharT, _Traits>& __hs,
           basic_httprequest<_CharT, _Traits> const& __req)
{
  __req.write(__hs);
  return __hs;
}

template<typename _CharT, typename _Traits>
basic_httpstream<_CharT, _Traits>&
operator>>(basic_httpstream<_CharT, _Traits>& __hs,
           basic_httpresponse<_CharT, _Traits>& __res)
{
  __res.read(__hs);
  return __hs;
}

__END_NETSTREAM_NAMESPACE

#endif
