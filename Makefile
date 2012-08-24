CC=g++
LDFLAGS=/usr/local/dip/depsys/fb303-thrift-0.4.0/include/thrift/fb303/lib/*.a /usr/local/dip/depsys/thrift-0.4.0/include/thrift/lib/*.a 
LIBS=-L/usr/local/dip/depsys/fb303-thrift-0.4.0/lib -L/usr/local/dip/depsys/thrift-0.4.0/lib   -lthrift -lfb303 -lpthread -lcurl
CXXFLAGS=-O2 -D_FILE_OFFSET_BITS=64 
INCLUDE=-I/usr/local/dip/depsys/fb303-thrift-0.4.0/include/thrift/fb303 -I/usr/local/dip/depsys/thrift-0.4.0/include/thrift -I/usr/local/dip/depsys/libcurl/include                                                                                                                                                                   
SOURCE=conf.cpp ModelRead.cpp dip_logtail_util.cpp scribe.cpp scribe_types.cpp scribe_send.cpp thread.cpp 
dip_logtail: $(SOURCE) 
	$(CC) $(SOURCE) -o $@ $(INCLUDE) $(LIBS) $(CXXFLAGS)
clean:
	rm dip_logtail
