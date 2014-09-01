#!/usr/bin/env python

# lazy pirate pattern
# http://zguide.zeromq.org/page:all#Client-Side-Reliability-Lazy-Pirate-Pattern
#
# bearing in mind that in our case:
# - the socket is always connected;
# - when no more retries are allowed, we can return 404.
#
# so, only one loop is needed, and not two like in the zmq docs

from flup.server.fcgi import WSGIServer
import zmq

context = zmq.Context()
ADDRESS = "ipc:////tmp/antifurto/live.ipc"
REQUEST_TIMEOUT = 2500
REQUEST_RETRIES = 3
REQUEST_MSG = True

def connect_socket():
    global socket, poll
    socket = context.socket(zmq.REQ)
    socket.connect(ADDRESS);
    poll = zmq.Poller()
    poll.register(socket, zmq.POLLIN)

def close_socket():
    global socket, poll
    socket.setsockopt(zmq.LINGER, 0)
    socket.close()
    poll.unregister(socket)

def app(environ, start_response):
    retries_left = REQUEST_RETRIES
    while retries_left:
        socket.send(REQUEST_MSG)

        socks = dict(poll.poll(REQUEST_TIMEOUT))
        if socks.get(socket) == zmq.POLL_IN:
            reply = client.recv()
            if not reply:
                # timeout: retry to send
                break

            # ok, got an answer
            start_response('200 OK', [('Content-Type', 'image/jpeg'), ('Cache-Control', 'no-cache')])
            return reply
        else:
            # socket gets confused. reconnect it
            close_socket()
            retries_left -= 1
            # create new connection
            connect_socket()

    # no more retries
    start_response('404 Not Found', [('Content-Type', 'text/html')])


connect_socket()
WSGIServer(app).run()

# cleanup on server exit
close_socket()
context.term()
