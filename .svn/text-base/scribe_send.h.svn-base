#include "common.h"
#include "scribe.h"
#include <protocol/TBinaryProtocol.h>
#include <transport/TSocket.h>
#include <transport/TTransportUtils.h>
#include <FacebookService.h>
#include <vector>

using namespace boost;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace scribe::thrift;
using namespace facebook::fb303;



class scribe_send{
	
	public:
		bool open();
        bool isOpen();
		int send();
        void close();
		void set_host_port_timeout(std::string t_host,int t_port,int t_timeout);
		std::vector<LogEntry> messages;
	private:
		std::string host;
		int  port;
		int  timeout;
		boost::shared_ptr<apache::thrift::transport::TSocket> socket;
        boost::shared_ptr<apache::thrift::transport::TFramedTransport> transport;
        boost::shared_ptr<apache::thrift::protocol::TBinaryProtocol> protocol;
        boost::shared_ptr<scribe::thrift::scribeClient> sendClient;
};
