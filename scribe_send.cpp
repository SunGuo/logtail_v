#include "scribe_send.h"


bool scribe_send::open(){
	try{
      	socket = shared_ptr<TSocket>(new TSocket(host, port));
        if(!socket) {
            throw std::runtime_error("Error creating socket");
		}   
		socket->setConnTimeout(timeout);
        socket->setRecvTimeout(timeout);
        socket->setSendTimeout(timeout);
        socket->setLinger(0, 0); 

        transport = shared_ptr<TFramedTransport>(new TFramedTransport(socket));
        if(!transport) {
            throw std::runtime_error("Error creating transport");
        }   

        protocol = shared_ptr<TBinaryProtocol>(new TBinaryProtocol(transport));
        if(!transport) {
            throw std::runtime_error("Error creating protocol");
        }   

        sendClient = shared_ptr<scribe::thrift::scribeClient>(new scribeClient(protocol));
        protocol->setStrict(false, false);
        if(!sendClient) {
            throw std::runtime_error("Error creating client");
        }   

        transport->open();    
    } catch(const TTransportException& ttx) {
        LOG_OPER("dip_logtail Initialization Failed due to TransportException :[%s]",ttx.what());
        return false;
    } catch(const std::exception& stx) {
        LOG_OPER("dip_logtail Initialization Failed due to Exception : [%s]",stx.what());
        return false;
    }   
   		LOG_OPER("dip_logtail Initialization Successful, listening for messages");
    return true;	
}

void scribe_send::set_host_port_timeout(std::string t_host,int t_port,int t_timeout){
	host=t_host;
	port=t_port;
	timeout=t_timeout;
}

bool scribe_send::isOpen(){
	return transport->isOpen();
}

int scribe_send::send(){
	if(!isOpen()) {
        if(!open()){
            return (CONN_FATAL);
        }   
    } 
    ResultCode result =TRY_LATER;
    try 
    {    
        result=sendClient->Log(messages);
        if(result == OK) {
			return (CONN_OK);
		}else if(result == TRY_LATER) {
            return (CONN_TRANSIENT);
        }     
    }   
	catch(const TTransportException& ttx) {
        LOG_OPER("< host:%s, port:%d > Message sending failed due to TransportException : [%s]",host.c_str(),port,ttx.what());
    }catch(std::exception& stx) {
        LOG_OPER("[ host:%s, port:%d ]Message sending failed due to Exception: [%s]",host.c_str(),port,stx.what());
    }
	return (CONN_FATAL);      
}

void scribe_send::close(){
	  try{
        	transport->close();
    	}catch(const TTransportException& ttx) {
        	LOG_OPER("<host:%s,port:%d> closing Exception",host.c_str(),port,ttx.what());
    	}
   		LOG_OPER("[ host:%s, port:%d ] Closing Scribe Client...",host.c_str(),port);	
}
