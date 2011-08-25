var amqp = require('amqp')
  , iniparser = require('iniparser')
  , nomnom = require('nomnom')

var defaults = {
  config_file: {
	abbr: 'c',
    full: 'config-file',
    help: 'Configuration file',
  },
  amqp_host: {
    _default: 'localhost',
    full: 'amqp-host',
    help: 'AMQP host',
  },
  amqp_port: {
    _default: 5672,
    full: 'amqp-port',
    type: 'int',
    help: 'AMQP port',
  },
  amqp_exchange: {
    _default: 'tyrion',
    full: 'amqp-exchange',
    help: 'AMQP exchange',
  },
}

var options = nomnom.scriptName('tyrion').opts(defaults).parseArgs()

if (options.config_file) {
	var config = iniparser.parseSync(options.config_file)

	for (var key in defaults) {
		var v = defaults[key].full.split('-', 2)
		var k1 = v[0]
		  , k2 = v[1]
		if (!options[key]) {
			if (config[k1] && config[k1][k2]) {
				if ((defaults[key].type || 'string') != 'string') {
					try {
						options[key] = JSON.parse(config[k1][k2])
					} catch(e) {
					}
				} else {
					options[key] = config[k1][k2]
				}
			} else {
				options[key] = defaults[key]._default
			}
		}
	}
} else {
	for (var key in defaults) {
		options[key] = options[key] || defaults[key]._default
	}
}

var connection = amqp.createConnection({ host: options.amqp_host, port: options.amqp_port })

connection.on('ready', function () {
  connection.exchange(options.amqp_exchange, {type: 'topic'}, function(exchange) {
    connection.queue('', function(q) {
      q.bind(exchange, "*")

      q.on('queueBindOk', function() {
        q.subscribe(function (message) {
          console.log('Got message: ' + message.data.toString())
        })
      })
    })
  })
})
