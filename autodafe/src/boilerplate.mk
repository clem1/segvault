
all::
	@subdirs="$(SUBDIRS)"; for d in $$subdirs; do (cd $$d; $(MAKE) $@) || exit 1; done

clean::
	@subdirs="$(SUBDIRS)"; for d in $$subdirs; do (cd $$d; $(MAKE) $@) || exit 1; done

install::
	@subdirs="$(SUBDIRS)"; for d in $$subdirs; do (cd $$d; $(MAKE) $@) || exit 1; done

uninstall::
	@subdirs="$(SUBDIRS)"; for d in $$subdirs; do (cd $$d; $(MAKE) $@) || exit 1; done


SHELL = @SHELL@
top_srcdir = @top_srcdir@
