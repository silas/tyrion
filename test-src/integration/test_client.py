import os
from helper import *
import shared

class TestClient(Base, shared.ServiceTests, unittest.TestCase):

    def __init__(self, *args, **kwargs):
        super(TestClient, self).__init__(*args, **kwargs)
        self.setup_basic()
        self.path = os.path.realpath(os.path.join(os.path.dirname(__file__), '..', '..'))
        os.environ['LD_LIBRARY_PATH'] = self.path
        os.environ['DYLD_LIBRARY_PATH'] = self.path

    def create_command(self, service='org.tyrion.service.bash', destination=None, timeout=30):
        if destination is None:
            destination = self.config.get('general', 'destination')
        command = os.path.join(self.path, 'tyrion')
        command += ' -c %s' % os.path.join(self.path, 'config', 'client.conf')
        command += ' -j %s' % destination
        command += ' -s %s' % service
        command += ' -t %s' % timeout
        return command

    def check(self, input, type, output=None, error=None, code=None, callback=None, timeout=30):
        command = Execute(
            self.create_command(
                service=type,
                timeout=timeout,
            ),
            stdin=input,
        )
        if error is not None:
            self.assertEqual(command.error.rstrip(), error)
        if output is not None:
            self.assertEqual(command.output.rstrip(), output)
        if code is not None:
            self.assertEqual(command.code, code)
        if callable(callback):
            callback(code=command.code, output=command.stdout, error=command.stderr)
