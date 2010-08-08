import os
from twisted.internet import defer
from helper import *
import test_basic

class TestLoad(BaseXMPP, test_basic.CheckBase, unittest.TestCase):

    def __init__(self, *args, **kwargs):
        super(TestLoad, self).__init__(*args, **kwargs)
        self.setup_basic()
        self.username = self.config.get('general', 'username')
        self.password = self.config.get('general', 'password')
        self.jid = jid.JID(self.username)

    def simple(self):
        return self.check(
            type='org.tyrion.service.bash',
            input='echo hello world',
        )

    def test_basic(self):
        defer_list = []
        for _ in xrange(int(os.environ.get('TYRION_LOAD_NUM', 50))):
            defer_list.append(self.simple())
        return defer.gatherResults(defer_list)
