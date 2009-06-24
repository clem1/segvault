SUBDIRS = src

all::

install::
	cd ./etc/generator; ./generator.sh /usr/local/etc

clean::
	rm -rf ./etc/generator/autodafe/*

#followed by boilerplate.mk

all::
	@subdirs="$(SUBDIRS)"; for d in $$subdirs; do (cd $$d; $(MAKE) $@) || exit 1; done

clean::
	@subdirs="$(SUBDIRS)"; for d in $$subdirs; do (cd $$d; $(MAKE) $@) || exit 1; done

install::
	@subdirs="$(SUBDIRS)"; for d in $$subdirs; do (cd $$d; $(MAKE) $@) || exit 1; done

uninstall::
	@subdirs="$(SUBDIRS)"; for d in $$subdirs; do (cd $$d; $(MAKE) $@) || exit 1; done


SHELL = /bin/sh
top_srcdir = .
