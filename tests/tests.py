# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import unittest

import subprocess


class Server:
    def __init__(self):
        self.p = subprocess.Popen("../Debug/coap_server", stdout=subprocess.PIPE)
        print "coap_server started"

    def __del__(self):
        print "killing coap_server"
        self.p.send_signal(9)


class Client:
    def run(self, requestType, server, uri, payload=""):
        p = subprocess.Popen(["../Debug/coap_client", requestType, "coap://" + server + uri, payload], stdout=subprocess.PIPE)
        output = []
        for x in p.stdout:
            output.append(x)
        return output

    def get(self, server, uri):
        return self.run("get", server, uri)

    def put(self, server, uri, payload):
        return self.run("put", server, uri, payload)

    def post(self, server, uri, payload):
        return self.run("post", server, uri, payload)

    def delete(self, server, uri):
        return self.run("delete", server, uri)


class IntegrationTests(unittest.TestCase):
    def setUp(self):
        self.server = Server()
        self.client = Client()

    def tearDown(self):
        self.client = 0
        self.server = 0

    @unittest.skip("Takes some time")
    def test_get_invalid_server(self):
        response = self.client.get("1.2.3.4", "/unknown-uri")
        self.assertGreater(len(response), 0)
        self.assertEqual(response[0], "503-ServiceUnavailable\n")

    def test_get_invalid_uri(self):
        response = self.client.get("localhost", "/unknown-uri")
        self.assertGreater(len(response), 0)
        self.assertEqual(response[0], "404-NotFound\n")

    def test_get(self):
        response = self.client.get("localhost", "/name")
        self.assertGreater(len(response), 1)
        self.assertEqual(response[0], "205-Content\n")
        self.assertEqual(response[1], "coap_server\n")

    def test_put(self):
        response = self.client.put("localhost", "/name", "CoaPP")
        self.assertGreater(len(response), 0)
        self.assertEqual(response[0], "204-Changed\n")
        # read it back to verify that it was changed
        response = self.client.get("localhost", "/name")
        self.assertGreater(len(response), 1)
        self.assertEqual(response[0], "205-Content\n")
        self.assertEqual(response[1], "coap_server\n")

    def test_post(self):
        # create a ressource
        response = self.client.post("localhost", "/dynamic", "something")
        self.assertGreater(len(response), 1)
        self.assertEqual(response[0], "201-Created\n")
        self.assertEqual(response[1], "1\n");
        # read it back to verify that it was created
        response = self.client.get("localhost", "/dynamic/1")
        self.assertGreater(len(response), 1)
        self.assertEqual(response[0], "205-Content\n")
        self.assertEqual(response[1], "something\n")

    def test_delete_not_allowed(self):
        response = self.client.delete("localhost", "/name")
        self.assertGreater(len(response), 0)
        self.assertEqual(response[0], "405-MethodNotAllowed\n")

    def test_delete(self):
        # create a ressource to delete
        response = self.client.post("localhost", "/dynamic", "something")
        self.assertGreater(len(response), 1)
        self.assertEqual(response[0], "201-Created\n")
        self.assertEqual(response[1], "1\n");
        # read it back to verify that it was created
        response = self.client.get("localhost", "/dynamic/1")
        self.assertGreater(len(response), 1)
        self.assertEqual(response[0], "205-Content\n")
        self.assertEqual(response[1], "something\n")
        # now delete it
        response = self.client.delete("localhost", "/dynamic/1")
        self.assertGreater(len(response), 1)
        self.assertEqual(response[0], "202-Deleted\n")
        # read it back to verify that it was deleted
        response = self.client.get("localhost", "/dynamic/1")
        self.assertGreater(len(response), 0)
        self.assertEqual(response[0], "404-NotFound\n")
