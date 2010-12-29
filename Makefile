SCONS?=scons
TESTS?=./tests
OPTFLAGS?=
PREFIX?=/usr/local

build:
	$(SCONS) --flags="${OPTFLAGS}"

devel:
	$(SCONS) --flags="${OPTFLAGS}" --with-devel

install:
	$(SCONS) --flags="${OPTFLAGS}" --prefix="${PREFIX}"

test:
	cd ${TEST_DIR} && ./run

clean:
	$(SCONS) -c
