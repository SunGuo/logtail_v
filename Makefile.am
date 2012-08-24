# Set libraries external to this component.
EXTERNAL_LIBS =-lthrift -lfb303 -lpthread -lcurl
bin_PROGRAMS = dip_logtail
dip_logtail_SOURCES =thread.cpp\
                     scribe_send.cpp\
                     scribe_types.cpp\
                     scribe.cpp\
                     conf.cpp\
                     ModelRead.cpp\
                     dip_logtail_util.cpp
dip_logtail_LDADD = $(EXTERNAL_LIBS) $(INTERNAL_LIBS)
