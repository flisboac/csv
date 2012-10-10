
WAF=waf -j2
OUT=./build

all: build

configure: 
	$(WAF) configure

build: configure
	$(WAF) build

clean:
	$(WAF) clean
