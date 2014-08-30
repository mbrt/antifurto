#!/usr/bin/env python

# lazy pirate pattern
# http://zguide.zeromq.org/page:all#Client-Side-Reliability-Lazy-Pirate-Pattern

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

def app(environ, start_response):
    retries_left = REQUEST_RETRIES
    while retries_left:
        socket.send(REQUEST_MSG)

        expect_reply = True
        while expect_reply:
            socks = dict(poll.poll(REQUEST_TIMEOUT))
            if socks.get(socket) == zmq.POLL_IN:
                reply = client.recv()
                if not reply:
                    # timeout: retry to send
                    break

                # ok, got an answer
                start_response('200 OK', [('Content-Type', 'image/jpeg'), ('Cache-Control', 'no-cache')])
                retries_left = REQUEST_RETRIES
                expect_reply = False
                return reply
            else:
                # socket gets confused. reconnect it
                socket.setsockopt(zmq.LINGER, 0)
                socket.close()
                poll.unregister(socket)
                retries_left -= 1
                # create new connection
                connect_socket()
                if retries_left:
                    socket_send(REQUEST_MSG)

    # no more retries
    start_response('404 Not Found', [('Content-Type', 'text/html')])


connect_socket()
WSGIServer(app).run()
context.term()
