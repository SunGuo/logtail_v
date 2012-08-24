#include "dip_logtail_util.h"
dip_logtail_util::dip_logtail_util(){
	
}

dip_logtail_util::~dip_logtail_util(){
	
}

void  dip_logtail_util::get_local_ip(){
	int sock_get_ip;
    char ipaddr[50];

    struct   sockaddr_in *sin;
    struct   ifreq ifr_ip;

    if ((sock_get_ip=socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
         printf("不能取得本机的ip!\n");
         eth0_ip=""; 	     
    }

    memset(&ifr_ip, 0, sizeof(ifr_ip));
    strncpy(ifr_ip.ifr_name, "eth0", sizeof(ifr_ip.ifr_name) - 1);

    if( ioctl( sock_get_ip, SIOCGIFADDR, &ifr_ip) < 0 )
    {
         eth0_ip="";
    }
    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;
    strcpy(ipaddr,inet_ntoa(sin->sin_addr));

    close( sock_get_ip );

    eth0_ip=trim_String(std::string(ipaddr));	
}

void  dip_logtail_util::get_local_an(){
	std::ifstream GET_an;
    std::string temp_an;
    GET_an.open("/etc/sinainstall.conf",std::ios::in);
    if(GET_an.fail())
    {
		/*
		*如果取得本机的盘点号用本机的ip的eth0的ip代替
		*/
        temp_an=eth0_ip;
    }
    else
    {
        getline(GET_an,temp_an);
        int mm=temp_an.find('=');
        temp_an=temp_an.substr(mm+1,temp_an.size());
    }
    an=trim_String(temp_an);
}

void dip_logtail_util::use_util(){
	get_local_ip();
	get_local_an();
}
std::string dip_logtail_util::get_ip(){
	return eth0_ip;
}

std::string dip_logtail_util::get_an(){
	return an;
}


std::string dip_logtail_util::trim_String(const std::string& str){
    std::string whitespace = " \t";
    size_t start      =str.find_first_not_of(whitespace);
    size_t end        =str.find_last_not_of(whitespace);
    
	if(start != std::string::npos) {
        return str.substr(start, end - start + 1); 
    }else{
        return ""; 
    }   
}

