#!/usr/bin/env python3

from http.server import BaseHTTPRequestHandler, HTTPServer
from multiprocessing import Process, Queue, Barrier
import tempfile
import unittest
import subprocess
import time
import os


def run_http(*messages):
    def run(message, queue, bar):
        server = create_server(message)
        queue.put(server.server_port)
        bar.wait()
        server.serve_forever()

    queue = Queue()
    bar = Barrier(len(messages)+1, timeout=5)
    processes = [Process(target=run, args=(message, queue, bar)) for message in messages]

    for p in processes:
        p.start()

    bar.wait()

    return processes, ['localhost:'+str(queue.get()) for x in range(len(processes))]


def create_server(msg):
    class Handler(BaseHTTPRequestHandler):
        def do_GET(self):
            self.send_response(200)
            self.send_header('Content-type','text/plain')
            self.end_headers()
            self.wfile.write(bytes(self.server.message, 'utf8'))

    httpd = HTTPServer(('localhost', 0), Handler)
    httpd.message = msg
    return httpd


class TestCompleteNet(unittest.TestCase):
    def setUp(self):
        self.tmp_file = tempfile.NamedTemporaryFile(delete=False)
        self.processes = []

    def tearDown(self):
        for s in self.processes:
            s.terminate()

    def _run(self, executable, additional, n, level):
        args = [
            os.path.join(os.getcwd(), executable),
            '--file='+self.tmp_file.name,
            '--severity='+level,
            '--dimension='+str(n)
        ]

        args += additional

        print(str(args))

        p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        p.wait()
        p.stderr.close()

        with p.stdout as pipe:
            out = pipe.read().decode('utf-8')

        print(out)

        self.assertEqual(p.returncode, 0)

        return out

    def builder(self, urls, n=2):
        return self._run('builder', ['--urls=' + url for url in urls], n, 'debug')

    def colmplete(self, *words, n=2, count=None):
        args = ['--word=' + word for word in words]

        if count is not None:
            args.append('--count='+str(count))

        return self._run('complete', args, n, 'error')

    def test_should_complete_line_from_three_sources(self):
        self.processes, urls = run_http(
            'a b c',
            'a b c d e',
            'a b c d e f g h i j k'
        )

        self.builder(urls)
        self.assertEqual(self.colmplete('a').strip(), 'a b c d e f g h i j k')

    def test_should_complete_line_from_one_source_with_count(self):
        self.processes, urls = run_http(
            'a a'
        )

        self.builder(urls)
        self.assertEqual(self.colmplete('a', count=15).strip(), ('a '*16).strip())

    def test_should_complete_line_from_one_source_until_it_is_possible_with_default_count(self):
        self.processes, urls = run_http(
            'a b',
            'a b c'
        )

        self.builder(urls)
        self.assertEqual(self.colmplete('a').strip(), 'a b c')

    def test_should_complete_line_from_many_source(self):
        self.processes, urls = run_http(
            'a b c',
            'a b c',
            'a one two three four five',
            'a one two three four five',
            'a one two three four five',
            'a one b two three four five',
            'a one b two three four five',
            'a one b two three four five',
            'a one b two three four five',
        )

        self.builder(urls, n=3)
        self.assertEqual(self.colmplete('a', 'b', n=3).strip(), 'a b c')
        self.assertEqual(self.colmplete('a', 'one', n=3).strip(), 'a one b two three four five')
        self.assertEqual(self.colmplete('one', 'two', n=3).strip(), 'one two three four five')

    def test_should_complete_line_from_very_long_source(self):
        self.processes, urls = run_http(
            'a b '*70,
            'b a '*70
        )
        self.builder(urls)
        self.assertEqual(self.colmplete('a', count=80).strip(), ('a b '*40)+'a')


if __name__ == '__main__':
    unittest.main()
