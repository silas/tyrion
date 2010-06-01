library_source = [
    'src/config.cc',
    'src/logging.cc',
    'src/process.cc',
    'src/setting.cc',
    'src/third_party/inih/ini.c',
    'src/utils.cc',
    'src/xmpp.cc',
    'src/xmpp_service.cc',
    'src/xmpp_service_manager.cc',
]

client_source = [
    'src/client.cc',
    'src/client_xmpp.cc',
    'src/client_xmpp_service_manager.cc',
]

node_source = [
    'src/node.cc',
    'src/node_acl.cc',
    'src/node_main.cc',
    'src/node_service_handler.cc',
    'src/node_xmpp.cc',
    'src/node_xmpp_service_manager.cc',
]

library_list = ['gloox', 'pthread']

env = Environment()

library_list += [env.SharedLibrary('tyrion', library_source, LIBS=library_list)]

env.Program(target='tyrion-node', source=node_source, LIBS=library_list)
env.Program(target='tyrion', source=client_source, LIBS=library_list)
