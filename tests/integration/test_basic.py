from helper import *
import shared

class TestBasic(BaseXMPP, shared.ServiceTests, unittest.TestCase):

    def __init__(self, *args, **kwargs):
        super(TestBasic, self).__init__(*args, **kwargs)
        self.setup_basic()
        self.username = self.config.get('general', 'username')
        self.password = self.config.get('general', 'password')
        self.jid = jid.JID(self.username)

    def check(self, input, type, output=None, error=None, code=None, callback=None, timeout=30, id=None):
        def handle(xml):
            response = self.parse_service(xml)
            if id is not None:
                self.assertEqual(response.id, id)
            if type is not None:
                self.assertEqual(response.type, type)
            if code is not None:
                self.assertEqual(response.code, code)
            if output is not None:
                self.assertEqual(response.output, output)
            if error is not None:
                self.assertEqual(response.error, error)
            if callable(callback):
                callback(code=response.code, output=response.output, error=response.error)
        return self.create_service(type, input, handle_success=handle, timeout=timeout)

    def test_service_unavailable_module_not_found(self):
        return self.create_service(
            'some-service-which-should-always-fail',
            handle_success=self.handle_service_unavailable_success,
            handle_error=self.handle_service_unavailable_error,
        )

class TestUnknownUser(BaseXMPP, unittest.TestCase):

    def __init__(self, *args, **kwargs):
        super(TestUnknownUser, self).__init__(*args, **kwargs)
        self.setup_basic()
        self.username = self.config.get('general', 'username_unknown_user')
        self.password = self.config.get('general', 'password_unknown_user')
        self.jid = jid.JID(self.username)

    def test_service_unavailable_acl(self):
        return self.create_service(
            'org.tyrion.service.bash',
            handle_success=self.handle_service_unavailable_success,
            handle_error=self.handle_service_unavailable_error,
        )
