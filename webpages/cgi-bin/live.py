#!/usr/bin/env python

from flup.server.fcgi import WSGIServer
import os, stat

def app(environ, start_response):
    url = environ['DOCUMENT_ROOT'] + environ['PATH_INFO']
    if not stat.S_ISFIFO(os.stat(url).st_mode):
        start_response('404 Not Found', [('Content-Type', 'text/html')])
    else:
        start_response('200 OK', [('Content-Type', 'image/jpeg'), ('Cache-Control', 'no-cache')])
        with open(url, 'r') as fifo_in:
            while True:
                read = fifo_in.read(4096)
                if not read:
                    break
                yield read

WSGIServer(app).run()
