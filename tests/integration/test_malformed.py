from helper import *

class TestMalformed(BaseXMPP, unittest.TestCase):

    def __init__(self, *args, **kwargs):
        super(TestMalformed, self).__init__(*args, **kwargs)
        self.setup_basic()
        self.username = self.config.get('general', 'username')
        self.password = self.config.get('general', 'password')
        self.jid = jid.JID(self.username)
        self.timeout = 2

    def missing(self, **kwargs):
        kwargs['type'] = kwargs.get('type', 'org.tyrion.service.bash')
        kwargs['handle_success'] = kwargs.get(
            'handle_success',
            self.handle_service_unavailable_success,
        )
        kwargs['handle_error'] = kwargs.get(
            'handle_error',
            self.handle_service_unavailable_error,
        )
        return self.create_service(**kwargs)

    def test_missing_service(self):
        return self.missing(service=None)

    def test_missing_type(self):
        return self.missing(type=None)

    def test_missing_id(self):
        return self.missing(id=None)

    def test_missing_input(self):
        return self.missing(input=None)
