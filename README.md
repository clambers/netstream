NetStream
=========

NetStream is a C++ header library that provides a set of classes for working
with network sockets and connections. It acts as a drop-in extension for the
standard IOStream classes, and tries to emulate their architecture and
functionality. As such, it shares IOStream's strengths and weaknesses.
NetStream supports Windows, OSX, and GNU/Linux.

Here's some sample code that spits out my favorite website:

```C++
net::httpstream conn("zombo.com");

net::httprequest req;
req.add_header("Host", "zombo.com");
req.add_header("Accept", "text/html");

conn << req;

std::cout << conn.rdbuf() << std::endl;
```

Using NetStream is easy: just add the single header to your project and you're
done. NetStream is [LGPL][1], so even proprietary software can use it, just
make sure to submit any meaningful changes you make to the header itself
upstream.

NetStream is currently under heavy initial development, and as such is
extremely unstable. It should not be used for production software.

---------------------------------

© 2015 Chris Lamberson  
Licensed under the [LGPLv3][1].

[1]: https://www.gnu.org/licenses/lgpl-3.0-standalone.html
