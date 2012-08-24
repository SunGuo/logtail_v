/*
*     Copyright (c) 2012_01_04 SINA Inc. All rights reserved.
*     Maintainer:   guoxian1@staff.sina.com
*     Descriptor:   scribe client 
*/
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <queue>
#include <vector>
#include <pthread.h>
#include <semaphore.h>
#include <map>
#include <set>
#include <stdexcept>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <climits>
#include <stdlib.h>
#include <ctime>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <curl/curl.h>

#define LOG_OPER(format_string,...)                                     \
  {                                                                     \
    time_t now;                                                         \
    char dbgtime[26] ;                                                  \
    time(&now);                                                         \
    ctime_r(&now, dbgtime);                                             \
    dbgtime[24] = '\0';                                                 \
    fprintf(stderr,"[%s %s:%d] " #format_string " \n", dbgtime, __FILE__, __LINE__, ##__VA_ARGS__); \
  }

#define CONN_FATAL (-1)
#define CONN_OK (0)
#define CONN_TRANSIENT (1)
