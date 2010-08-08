import ConfigParser
import logging
import subprocess
import sys
import uuid
from twisted.internet import defer
from twisted.names import srvconnect
from twisted.python import log as txlog
from twisted.trial import unittest
from twisted.words.protocols.jabber import client
from twisted.words.protocols.jabber import error
from twisted.words.protocols.jabber import jid 
from twisted.words.protocols.jabber import xmlstream
from twisted.words.xish import domish

class Response(object):

    def __init__(self):
        self.id = None
        self.code = None
        self.type = None
        self.output = None
        self.error = None

class Execute(object):

    def __init__(self, command, stdin=''):
        self.code = '-1'
        self.error = ''
        self.output = ''

        reference = subprocess.Popen(
            command,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            shell=True,
            close_fds=True
        )

        self.stdout, self.stderr = reference.communicate(input=stdin)
        self.return_code = str(reference.returncode)

        self.parse()

    def parse(self):
        self.code = self.parse_code(self.stdout, self.code)
        self.code = self.parse_code(self.stderr, self.code)
        self.output = self.parse_text(self.stdout)
        self.error = self.parse_text(self.error)

    def parse_code(self, text, default):
        if int(default) < 0 and ' (' in text and '):' in text:
            return text.split(' (', 1)[1].split('):', 1)[0]
        else:
            return default

    def parse_text(self, text, default=''):
        if '):' in text:
            return text.split('): ', 1)[1]
        else:
            return default

class log(object):

    @staticmethod
    def debug(text):
        txlog.msg(text, logLevel=logging.DEBUG)

    @staticmethod
    def info(text):
        txlog.msg(text, logLevel=logging.INFO)

    @staticmethod
    def warning(text):
        txlog.msg(text, logLevel=logging.WARNING)

    @staticmethod
    def error(text):
        txlog.msg(text, logLevel=logging.ERROR)

    @staticmethod
    def critical(text):
        log.msg(text, logLevel=logging.CRITICAL)

class Base(object):

    def setup_basic(self):
        self.timeout = 60
        self.config = ConfigParser.RawConfigParser()
        self.config.read('helper.conf')
        if self.config.getboolean('general', 'logging'):
            txlog.startLogging(sys.stdout)

class BaseXMPP(Base):

    def setUp(self):
        defer_connected = defer.Deferred()
        self.defer_disconnected = defer.Deferred()
        self.client_connection = self.setup_client(
            defer_connected,
            self.defer_disconnected,
        )

        return defer.gatherResults([
            defer_connected,
        ])

    def tearDown(self):
        self.client_connection.disconnect()
        return defer.gatherResults([
            self.defer_disconnected,
        ])

    def setup_client(self, d1, d2):
        from twisted.internet import reactor
        factory = client.XMPPClientFactory(
            self.jid,
            self.password,
        )
        factory.addBootstrap(
            xmlstream.STREAM_AUTHD_EVENT,
            self.client_authenticated,
        )
        factory.addBootstrap(
            xmlstream.INIT_FAILED_EVENT,
            self.client_init_failed,
        )
        self.defer_authenticated = d1
        self.defer_closed = d2
        factory.clientConnectionLost = self.client_closed
        self.factory = factory
        return reactor.connectTCP('127.0.0.1', 5222, factory)

    def raw_data_in(self, buf):
        log.debug('CLIENT RECV: %s' % unicode(buf, 'utf-8').encode('ascii', 'replace'))

    def raw_data_out(self, buf):
        log.debug('CLIENT SEND: %s' % unicode(buf, 'utf-8').encode('ascii', 'replace'))

    def client_authenticated(self, xs):
        self.xmlStream = xs
        self.xmlStream.rawDataInFn = self.raw_data_in
        self.xmlStream.rawDataOutFn = self.raw_data_out
        xml = domish.Element(('jabber:client', 'stream:features'))
        xml.addElement('bind', self.config.get('general', 'namespace'))
        xs.send(xml)
        self.defer_authenticated.callback(None)

    def client_closed(self, connector, reason):
        self.defer_closed.callback(None)

    def client_init_failed(self, failure):
        log.error('Client init failed: %s' % failure)

    def create_service_iq(self, type=None, id=None, user=None, group=None,
                          timeout=None, input=None, service=None):
        iq = xmlstream.IQ(self.xmlStream)
        if service is not None:
            service = iq.addElement((self.config.get('general', 'namespace'),
                                    'service'))
            if type is not None:
                service.attributes['type'] = type
            if id is not None:
                service.attributes['id'] = id or uuid.uuid4().get_hex()
            if user is not None:
                service.attributes['user'] = user
            if group is not None:
                service.attributes['group'] = group
            if timeout is not None:
                service.attributes['timeout'] = unicode(timeout)
            if input and not input.endswith('\n'):
                input = input + '\n'
            if input is not None:
                service.addElement('input', content=input)
        return iq

    def create_service(self, type='', input='', handle_success=None,
                       handle_error=None, destination=None, user=None,
                       group=None, timeout=None, id='', service=True, iq=None):
        def default_success_func(xml):
            pass
        if handle_success is None:
            handle_success = default_success_func
        if destination is None:
            destination = self.config.get('general', 'destination')
        if iq is None:
            iq = self.create_service_iq(
                service=service,
                type=type,
                id=id,
                user=user,
                group=group,
                timeout=timeout,
                input=input,
            )
        # send iq
        d = iq.send(destination)
        d.addCallback(handle_success)
        if handle_error is not None:
            d.addErrback(handle_error)
        return d

    def parse_service(self, xml):
        response = Response()
        for e1 in xml.elements():
            if e1.name == 'service':
                response.code = e1.attributes.get('code')
                response.id = e1.attributes.get('id')
                response.type = e1.attributes.get('type')
                for e2 in e1.elements():
                    if e2.name == 'output':
                        response.output = unicode(e2).strip()
                    elif e2.name == 'error':
                        response.error = unicode(e2).strip()
        return response

    @property
    def handle_service_unavailable_success(self):
        def handle(xml):
            raise Exception('Should have got feature-not-implemented')
        return handle

    @property
    def handle_service_unavailable_error(self):
        def handle(failure):
            self.assertTrue(failure.check(error.StanzaError))
            self.assertTrue('feature-not-implemented' in failure.getErrorMessage())
        return handle
