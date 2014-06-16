OpenGL Hello World for PNaCl/Pepper
===================================

To build this package for PNaCl, and serve it using the simple HTTP server
distributed with the NaCl SDK, run:

    autoreconf -f -i
    ./configure --host=pnacl-unknown-pepper
    make serve

Warning: The web server listens by default on all interfaces, so other
(hostile) devices may be able to connect to it.
