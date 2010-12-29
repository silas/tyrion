BUILD?=config.build
INSTALL?=config.install
SCONS?=scons
TESTS?=./tests
OPTFLAGS?=
PREFIX?=/usr/local

$(BUILD):
	echo -n "$(SCONS) --flags=\"$(OPTFLAGS)\" --prefix=\"$(PREFIX)\"" > $(BUILD)
	sh $(BUILD)

build: $(BUILD)

install: $(BUILD)
	cp -f $(BUILD) $(INSTALL)
	echo -n " --install" >> $(INSTALL)
	sh $(INSTALL)

devel:
	$(SCONS) --flags="$(OPTFLAGS)" --with-devel

test:
	$(TESTS)/run-unit

test-integration: tyrion
	$(TESTS)/run-integration

test-all: test test-integration

clean:
	$(SCONS) -c
	rm -f config.*
	find . -name \*.o -type f -delete
	find . -name \*.pyc -type f -delete
