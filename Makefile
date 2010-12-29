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

devel:
	$(SCONS) --flags="$(OPTFLAGS)" --with-devel

install: $(BUILD)
	cp -f $(BUILD) $(INSTALL)
	echo -n " --install" >> $(INSTALL)
	sh $(INSTALL)

test:
	cd $(TESTS) && ./run

clean:
	$(SCONS) -c
	rm -f config.*
