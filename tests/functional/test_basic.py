from helper import *

class TestBasic(Base, unittest.TestCase):

    def __init__(self, *args, **kwargs):
        super(TestBasic, self).__init__(*args, **kwargs)
        self.setup_basic()
        self.username = self.config.get('general', 'username')
        self.password = self.config.get('general', 'password')
        self.jid = jid.JID(self.username)

    def basic(self, text, type_):
        def handle(xml):
            response = self.parse_service(xml)
            self.assertEqual(response.type, type_)
            self.assertEqual(response.code, '0')
            self.assertEqual(response.output, '2')
            self.assertEqual(response.error, '')

        return self.create_service(type_, text, handle_success=handle)

    def test_bash(self):
        return self.basic('bc -q\n2\nquit()\n', 'org.tyrion.service.bash')

    def test_python(self):
        return self.basic('print 1+1\n', 'org.tyrion.service.python')

    def test_ruby(self):
        return self.basic('puts 1+1\n', 'org.tyrion.service.ruby')

    def test_timeout_error(self):
        text = 'sleep 10\n'
        type_ = 'org.tyrion.service.bash'

        def handle(xml):
            response = self.parse_service(xml)
            self.assertEqual(response.code, '15')
            self.assertEqual(response.type, type_)
            self.assertEqual(response.output, '')
            self.assertEqual(response.error, '')

        return self.create_service(type_, text, handle_success=handle, timeout=1)

    def test_service_unavailable_module_not_found(self):
        return self.create_service(
            'some-service-which-should-always-fail',
            handle_success=self.handle_service_unavailable_success,
            handle_error=self.handle_service_unavailable_error,
        )

class TestUnknownUser(Base, unittest.TestCase):

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
