bin_PROGRAMS = hello_world

AM_CFLAGS = -g -Wall $(PACKAGES_CFLAGS)
AM_CXXFLAGS = $(AM_CFLAGS)

hello_world_SOURCES = app.cc gl.cc
hello_world_LDADD = $(PACKAGES_LIBS)

if SYSTEM_PEPPER
noinst_DATA = hello_world.pexe

hello_world.pexe: hello_world
	$(PNACL_FINALIZE) $< $(OUTPUT_OPTION)

serve: hello_world.pexe
	$(PYTHON) $(HTTPD_PY) --no-dir-check -C $(CURDIR)

AM_CPPFLAGS = -DTARGET_PPAPI
hello_world_SOURCES += main_ppapi.cc
endif

if SYSTEM_SDL
AM_CPPFLAGS = -DTARGET_SDL
hello_world_SOURCES += main_sdl.cc
endif
