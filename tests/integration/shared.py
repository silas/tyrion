class ServiceTests(object):

    def test_bash(self):
        return self.check(
            input='bc -q\n1+1\nquit()',
            type='org.tyrion.service.bash',
            output='2',
            error='',
            code='0',
        )

    def test_python(self):
        return self.check(
            input='print 1+1',
            type='org.tyrion.service.python',
            output='2',
            error='',
            code='0',
        )

    def test_ruby(self):
        return self.check(
            input='puts 1+1',
            type='org.tyrion.service.ruby',
            output='2',
            error='',
            code='0',
        )

    def test_timeout_error(self):
        return self.check(
            input='echo test\nsleep 2',
            type='org.tyrion.service.bash',
            output='test',
            error=None,
            code='15',
            timeout=1,
        )
