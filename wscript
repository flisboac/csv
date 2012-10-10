#!/usr/bin/env python

APPNAME = 'csv'
MAJORVERSION = 0
MINORVERSION = 1
PATCHVERSION = 0
RELEASENAME = 'beta'

VERSION = '%d.%d.%d-%s' % (MAJORVERSION, MINORVERSION, PATCHVERSION, RELEASENAME)
APPPREFIX = APPNAME + '_'

top = '.'
out = 'build'

sources = 'src/**.c'
includes = 'include/**.h'

def options(ctx):

    # Waf chore
	ctx.load("compiler_c")

	# Options
	ctx.add_option(
		'-C', '--c-compiler'
		, dest = 'c_compiler'
		, default = ''
		, action = 'store'
		, help = "Chooses the exact compiler to use in the build process."
	)


def configure(ctx):
    # Load compiler
	if ctx.options.c_compiler:
		ctx.load(ctx.options.c_compiler)
	else:
		ctx.load("compiler_c")


def build(ctx):
    ctx.stlib(
          source = ctx.path.ant_glob(sources)
        , target = APPNAME
        , defines = [APPPREFIX + 'STATIC', APPPREFIX + 'BUILDING']
        , includes = ['include']
        #, cflags = ['-E']
    )
    ctx.shlib(
          source = ctx.path.ant_glob(sources)
        , target = APPNAME
        , includes = ['include']
        , defines = [APPPREFIX + 'BUILDING']
    )
    
