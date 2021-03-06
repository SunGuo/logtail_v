#include "ModelRead.h"

ModelRead::ModelRead(){

}

ModelRead::~ModelRead(){
	//delete FIFO;
	//delete logfilename;
}

void ModelRead::set_modelconfig(const std::map<std::string,std::string>& model_config){
	modelconfig=model_config;	// 获取一个modelconfig的值；
}

bool ModelRead::getString(const std::string &stringName,std::string& _returnvalue){
   bool found=false;
   std::map<std::string,std::string>::const_iterator iter = modelconfig.find(stringName);
   if (iter != modelconfig.end()) {
        _returnvalue = iter->second;
        found=true;
   }
   return found;
}

bool ModelRead::openFIFOFile(){
	FIFO=path.c_str();
	fd=open(FIFO,O_RDONLY|O_NONBLOCK,0);
	if(fd==-1){
		std::cout<<"以非阻塞模式打开命名管道的时候出错"<<std::endl;
		return false;
	}else{
		std::cout<<"打开管道成功"<<std::endl;
		return true;
	}
}

void ModelRead::start(){
	//TODO 对管道读取的支持
	
	/*if(openFIFOFile()){
		std::cout<<"打开成功  "<<std::endl;
		std::cout<<"path  :"<<path<<std::endl;
		readFIFO();
	}*/
	//readLogFile();
	//readLogFile_test();
	if(openLogFile()){
		scribe_inter.set_host_port_timeout(host,StringToInt(port),StringToInt(timeout));
		scribe_inter.open();
		logtail_File();
		LOG_OPER("OK	:modelname :[%s] category [%s]  Started",modelname.c_str(),category.c_str());
	}
}

void ModelRead::make_prefix_category(){
		std::string temp_prefix="_accesskey=";
		std::string temp_category=type;
		log_prefix=temp_prefix.append(accesskey).append("&_ip=").append(from).append("&_port=").append(log_server_port).append("&_an=").append(local_an).append("&_data=");
		category=temp_category.append("_").append(accesskey).append("_").append(set);
		
}

void ModelRead::set_ipandan(std::string eth0ip,std::string an){
	local_an=an;
	local_eth0_ip=eth0ip;
}

/*
* TODO：对管道读取的支持，暂时发现在读取管道的时候可能有bug，不支持
*/

void ModelRead::readFIFO(){
    
	char buf_r[BUFFER_SIZE+1];
    int fd;
    int nread;
    //FIFO=path.c_str();
    printf("开始从管道中读取内容 ...\n");
    memset(buf_r,0,sizeof(buf_r));
    std::string suffix="test_suffix&_data=";
    std::string temp;
    std::string dddd;
    //fd=open(FIFO,O_RDONLY|O_NONBLOCK,0);
    if(fd==-1)
    {
        perror("open");
        exit(1);
    }       
    while(1)    
    {   
        memset(buf_r,0,sizeof(buf_r));
		std::cout<<"path model"<<path<<std::endl;
        if((nread=read(fd,buf_r,BUFFER_SIZE+1))==-1){
            if(errno==EAGAIN)
                printf("no data yet\n");
        }   
        else{   
            if(temp.length()==0){
                temp=std::string(buf_r);
            }else{
                temp=temp.append(std::string(buf_r));
            }   
            while(temp.find_first_of("\n")!=std::string::npos){
            
                dddd=suffix;
                message_queue.push(dddd.append(temp.substr(0,temp.find_first_of("\n"))));
                temp=temp.substr(temp.find_first_of("\n")+1);
            }    	
             while (!message_queue.empty()){
                std::string temp_d=message_queue.front();
                size_t dd =temp_d.find_first_of("\n");
                message_queue.pop();
            }


        }
        sleep(1);
    }
    pause();
    unlink(FIFO);
    
}

long long ModelRead::StringToInt(const std::string temp){
	long long t_return;
	std::stringstream oss;
	
	oss<<temp;
	oss>>t_return;

	return t_return;
}

void ModelRead::setisreload(bool t_isreload){
	isreload=t_isreload;
}

void ModelRead::reload(std::map<std::string,std::string> m_checkpoint){
	time_t m_now;
	m_now=time(NULL);
	std::string check_time;
	std::string check_position;
	std::string check_path;
    check_time=m_checkpoint.find("check_time")->second;
	check_position=m_checkpoint.find("check_position")->second;
	check_path=m_checkpoint.find("check_path")->second;
	long long t_position=(long long )StringToInt(check_position);
	if(path.compare(check_path)==0){
		position=t_position;
		isreload=true;
	}
	else{
		isreload=false;
	}	
}

bool ModelRead::openLogFile(){
		logfilename=path.c_str();
		stat(logfilename, &n_stat);
		o_stat=n_stat;
		logreadfile.open(logfilename,std::ios::in);
		if(logreadfile.fail()){//文件打开失败:返回0
			LOG_OPER("ERROR	:modelname :[%s] open the log file [%s]",modelname.c_str(),path.c_str());
			return false;
		}else{
			
			if(!isreload){
				logreadfile.seekg(0,std::ios::end);
				position=logreadfile.tellg();//得到刚开始去tail日志的位置
			}
			if(isreload){
				LOG_OPER("OK    :modelname :[%s] reload log file [%s]  position rolled",modelname.c_str(),path.c_str());
				logreadfile.seekg(0,std::ios::end);
				long long end_position=logreadfile.tellg();
				/*
				 *	在进行reload的情况下，但是当前文件的偏移量小于checkpoint中记录的position时
				 *		1 文件已经回滚，当前打开的文件已经是一个回滚后的文件
				 *		为了防止读不到文件现在的操作是直接定位到文件末尾
				 *		TODO：通过backup_file来控制文件的回滚现象（通过把回滚后的文件全部发送过去）
				 */
				if(end_position<position){
					position=end_position;
				}
			}
			//logreadfile.close();
			LOG_OPER("OK	:modelname :[%s] open the log file [%s]",modelname.c_str(),path.c_str());
			setModeltime();
			logreadfile.close();
			logreadfile.clear();
			return true;
		}
}

/*
* logtail的报警接口使用libcurl  对logtail的报警的重新封装
*/

void ModelRead::dip_logtail_alarm_libcurlinterface(std::string message,std::string subject){
	//报警使用的URL
	
	std::string t_alarmURL = alarmURL; 
	std::string t_alarmPostPrefix = alarmPostPrefix;
	
	std::string temp_an=local_an;
	std::string default_phone=defaultphone;
	std::string default_mail=defaultmail;
	
	if(!phone_alarm.empty()&&phone_alarm!="null"){
		default_phone.append(",").append(phone_alarm);
	} else {
        default_phone.append("");
    }
	
    if(!mailto_alarm.empty()&&mailto_alarm!="null"){
        default_mail.append(",").append(mailto_alarm);
	}
		
	CURL *curl;
	curl_global_init(CURL_GLOBAL_ALL);
    curl=curl_easy_init(); //init curl
    curl_easy_setopt(curl, CURLOPT_URL, t_alarmURL.c_str());
		
	//设置libcurl的超时时间
	curl_easy_setopt(curl,CURLOPT_TIMEOUT,alarm_timeout);
	//设置post的参数
	
	t_alarmPostPrefix.append(temp_an).append("_").append(message).append(default_mail).append(default_phone);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS,t_alarmPostPrefix.c_str()); 
    curl_easy_setopt(curl, CURLOPT_POST, 1);
	int res = curl_easy_perform(curl); //执行发送
	
    curl_easy_cleanup(curl);//清除curl操作
}

/*
*	logtail的报警接口，对logtail的报警进行封装
*/

void ModelRead::dip_logtail_alarm_interface(std::string message,std::string subject){
    std::string temp_an=local_an;
	std::string default_phone="&gmsg_to=dip_logtail";
    std::string default_mail="&gmail_to=dip_logtail";
    if(!phone_alarm.empty()&&phone_alarm!="null"){
        default_phone.append(",").append(phone_alarm).append("\"");
    }   
    else{
        default_phone.append("\"");
    }   
    if(!mailto_alarm.empty()&&mailto_alarm!="null")
        default_mail.append(",").append(mailto_alarm);
    std::string net_site=" http://monitor.sina.com.cn/cgi-bin/alertmail.cgi";
    std::string alarm_message="curl -d \"group_name=dpool&grade=-1&service_name=dip-logtail&object=scribe&subject=";
    alarm_message.append(temp_an).append("_").append(message).append(default_mail).append(default_phone).append(net_site);
    char const *temp=alarm_message.c_str();
    system(temp);	
}


void ModelRead::read_backupfile(){
	long long length;
	char * buffer;
	long long temp;
	std::ifstream backupread;
	backupread.open(backup_path.c_str(),std::ios::binary);
  	// 得到文件中还没有进行读的size:
	if(backupread.fail()){
		LOG_OPER("ERROR : modelname :[%s] ,[%s] backup_path not exist ",modelname.c_str(),backup_path.c_str());
		//***************报警接口 对backup_path进行报警//
		std::string alarm_backup_mess="backup_path_not_exist";
		std::string alarm_backup_sub="&subject=dip_logtail_error";
		dip_logtail_alarm_libcurlinterface(alarm_backup_mess,alarm_backup_sub);
	}else if(!backupread.fail()){
		backupread.seekg (0, std::ios::end);
  		length=backupread.tellg();
		backupread.seekg (position, std::ios::beg);
  		length=length-position;

	//分配内存空间:
		if(length>0){
  			buffer = new char [length];

  			// 把整个数据读取到内存中:
  			backupread.read (buffer,length);
  			backupread.close();
			backupread.clear();
  			std::string lastmsg=std::string(buffer);
			std::string t_prefix;
			long long t_m=5;
    		long long all_num;
			long long t_num=2;
			while(lastmsg.find_first_of("\n")!=std::string::npos){
				t_prefix=log_prefix;
        		t_prefix=t_prefix.append(lastmsg.substr(0,lastmsg.find_first_of("\n")));
				lastmsg=lastmsg.substr(lastmsg.find_first_of("\n")+1);
        		LogEntry le;
        		le.message=t_prefix;
				le.category=category;
        		scribe_inter.messages.push_back(le);
				if(scribe_inter.messages.size()>max_send_msg){
					int result_code=scribe_inter.send();//得到进行发送的发送状态
            		if(result_code==CONN_FATAL||result_code==CONN_TRANSIENT){
                		LOG_OPER("ERROR  : modelname :[%s] CONN_FAILED {%d}: connect failed",modelname.c_str(),result_code);
                		scribe_inter.close();
                		scribe_inter.open();
                		while(!scribe_inter.isOpen()){
                    	//***********///报警接口   对连接失败的接口
							std::string alarm_backupconn_mess="can_not_link";
							std::string alarm_backupconn_sub="&subject=dip_logtail_error";
							dip_logtail_alarm_libcurlinterface(alarm_backupconn_mess,alarm_backupconn_sub);
                    		scribe_inter.open();
                    		sleep(t_m);
                    		all_num+=t_m;
                    		LOG_OPER("ERROR  : modelname :[%s]  ,[%ds] can not connect scribe server",modelname.c_str(),all_num);
							if(t_m>300){
								continue;
							}
                    		t_m=t_num*t_m;
                		}
            		}else{
                		LOG_OPER("OK  : modelname :[%s]  ,send <host: %s port :%s> messages [%d]",modelname.c_str(),host.c_str(),port.c_str(),scribe_inter.messages.size());
                		scribe_inter.messages.clear();
            		}	
				}
        		t_prefix="";
			}    
  			delete[] buffer;
   
  		}
	}
	
}


/*
* 在每次读取之前都需要判断文件日否能正常读取的函数
*/

void ModelRead::stillGood(){
	long sleep_m=5; 
	long t_num=2;
	long all_time; 
	
	logreadfile.open(logfilename,std::ios::in);

	while(logreadfile.fail()){ //文件打开失败，以退避算法等待去重复打开文件
		sleep(sleep_m);
		logreadfile.close();
		logreadfile.clear();
		LOG_OPER("WARNING: modelname :[ %s ],[ %s ] can not open ,wait time[%d]",modelname.c_str(),path.c_str(),sleep_m);
		 //****************///报警策略  对日志文件不能打开的报警
		std::string alarm_file_mess="can_not_open_file";
		std::string alarm_file_sub="&subject=dip_logtail_error";
		dip_logtail_alarm_libcurlinterface(alarm_file_mess,alarm_file_sub);
		logreadfile.open(logfilename,std::ios::in);
		
		if(sleep_m>=300){
			continue;
		}
		
		all_time+=sleep_m;
		sleep_m=t_num*sleep_m;
	}
	

	/*
	* 如果文件打开成功，则定位文件应该读取的位置
	*/
	
	stat(logfilename, &n_stat);
	if(n_stat.st_ino!=o_stat.st_ino){        //文件的描述符已经变化，文件已经被移走
		logreadfile.seekg(0,std::ios::beg);
		read_backupfile();				
		position=logreadfile.tellg();
		LOG_OPER("WARNING : modelname :[%s] ,[%s] log file moved",modelname.c_str(),logfilename);
	
	}else if(n_stat.st_size<o_stat.st_size){ //发现文件已经回滚
		logreadfile.seekg(0,std::ios::beg);
		read_backupfile();
		position=logreadfile.tellg();
		LOG_OPER("WARNING : modelname :[%s] ,[%s] log file rolled",modelname.c_str(),logfilename);
	}
	/*
	 *更新文件描述符
	 */
	o_stat=n_stat;
}

void ModelRead::readLogFile(){
	 stillGood();
	 std::string t_prefix;
	 std::string t_category;
	 std::string temp;
	 logreadfile.seekg(position,std::ios::beg);
	 while(getline(logreadfile,temp)){
		
		t_prefix=log_prefix;
		t_prefix=t_prefix.append(temp).append("\n");
		LogEntry le;
		le.message=t_prefix;
		le.category=category;
		scribe_inter.messages.push_back(le);
		t_prefix="";
		temp="";
	 	position=logreadfile.tellg();//记录最新的读取位置；
		
		if(scribe_inter.messages.size()>=max_send_msg){
			break;
		} 
	 }
	 write_checkpoint(); 
	 logreadfile.close();
	 logreadfile.clear();
}


void ModelRead::logtail_File(){
	long t_m;
	long t_num;
	long all_num;
	long send_size;
	while(1){
		readLogFile();
		t_m=5;
		t_num=2;
		all_num=0;
		send_size=scribe_inter.messages.size();
		if(send_size>0){
			int result_code=scribe_inter.send();//得到进行发送的发送状态
			if(result_code==CONN_FATAL||result_code==CONN_TRANSIENT){
				LOG_OPER("ERROR  : modelname :[%s]  CONN_FAILED [%d] : connect failed",modelname.c_str(),result_code);
				scribe_inter.close();
				scribe_inter.open();
				while(!scribe_inter.isOpen()){
					//***********///报警接口   对连接失败的接口
					scribe_inter.close();
					std::string alarm_send_mess="can_not_link";
					std::string alarm_send_sub="&subject=dip_logtail_error";
					dip_logtail_alarm_libcurlinterface(alarm_send_mess,alarm_send_sub);
					scribe_inter.open();
					sleep(t_m);
					all_num+=t_m;
					LOG_OPER("ERROR  : modelname :[%s]  ,[%ds] can not connect scribe server",modelname.c_str(),all_num);
					if(t_m>=300){
						/****///如果t_m的数据大于300s则不在进行向上添加
						continue;
					}
					t_m=t_num*t_m;
				}
			}else{
				LOG_OPER("OK  : modelname :[%s]  ,send <host: %s port :%s> messages [%d]",modelname.c_str(),host.c_str(),port.c_str(),send_size);
				scribe_inter.messages.clear();
			}
		}else{
	  		sleep(1);
		}
	}
}

void ModelRead::create_dir(const std::string dir_name){
	std::string dirname=dir_name; 
	mkdir(dirname.c_str(),07777); 
}

bool ModelRead::isdir(std::string path){
	struct stat dirstat;
	if(stat(path.c_str(),&dirstat)<0)
		return false;
	if(S_ISDIR(dirstat.st_mode))
		return false;

	return true;
}

void ModelRead::write_checkpoint(){
	time_t t_time;
	std::stringstream str;
	std::stringstream str_position;
	std::string t_position;
	std::string str_time ;
	t_time=time(NULL);
    		
	std::string checkpointpath="checkpoint";
	create_dir(checkpointpath);
	checkpointpath.append("/").append(modelname).append(".checkpoint");
	pass_time=t_time-now;
	str<<t_time;
	str_position<<position;
	str_position>>t_position;
	str>>str_time;
	
	str_time.append(" ").append(t_position).append(" ").append(path);
	
	std::ofstream f; //累加向文件添加内容
	if((pass_time%checkpoint_time)==0){
		f.open(checkpointpath.c_str(),std::ios::trunc);
		std::string write_info=str_time;
    	if(f.good())
    	{  
        	f<<write_info;
    	}   
   		f.close();	
		f.clear();
	}
}

void ModelRead::setmodelname(const std::string t_modelname){
	modelname=t_modelname;	
}

/*
 *  初始化模块时间的函数
 */
void ModelRead::setModeltime(){
	time_t t_now;
	t_now=time(NULL);
	now=t_now;
	pass_time=0;
}
bool ModelRead::parseModel(){
	  	
		std::string temp_port;
		std::string temp_host;
		std::string temp_accesskey;
		std::string temp_secrekey;
		std::string temp_type;
		std::string temp_path;
		std::string temp_set;
		std::string temp_from;
		std::string temp_max_send_msg;  		//发送周期 单位（条）默认为50，经过测试除非特别要求不用修改（日志量特别多或者特别少）
		std::string temp_mailto_alarm;
		std::string temp_phone_alarm;
		std::string temp_timeout; 			//连接超时时间 单位（毫秒） 默认5000，可根据用户自己的情况修改
		//std::string temp_logtail_log_path;
		std::string temp_log_server_port;
		std::string temp_backup_path;
		std::string temp_checkpoint_time;
		std::string temp_alarmURL;
		std::string temp_alarmPostPrefix;
		std::string temp_defaultphone;
		std::string temp_defaultmail;
		std::string temp_alarm_timeout;
		
		/*
		*获得这个模块的参数，如果某个参数不是必须的则可以用默认值代替，如果某个参数是必须的则解析配置文件出错返回值为false
		*/		
		if(getString("port",temp_port)){
			port=temp_port;
		}
		else{
			LOG_OPER("ERROR  : modelname :[%s] ,config  [  port ] is necessary  ",modelname.c_str());
			return false;	
		}
		
		if(getString("host",temp_host)){
			host=temp_host;
		}else{
			LOG_OPER("ERROR  : modelname :[%s] ,config  [  host ] is necessary  ",modelname.c_str());
			return false;
		}
		
		if(getString("accesskey",temp_accesskey)){
			accesskey=temp_accesskey;
		}else{
			LOG_OPER("ERROR  : modelname :[%s] ,config  [  accesskey ] is necessary  ",modelname.c_str());
			return false;
		}
		
		
		if(getString("defaultphone",temp_defaultphone)){
			defaultphone = temp_defaultphone;
		} else {
			defaultphone = "&gmsg_to=dip_logtail";
		}
		
		if(getString("defaultmail",temp_defaultmail)){
			defaultmail = temp_defaultmail;
		} else {
			defaultmail = "&gmail_to=dip_logtail";
		}
		
		if(getString("alarmURL",temp_alarmURL)){
			alarmURL = temp_alarmURL;
		} else {
			alarmURL = "http://monitor.sina.com.cn/cgi-bin/alertmail.cgi";
		}
		
		if(getString("alarmPostPrefix",temp_alarmPostPrefix)){
			alarmPostPrefix = temp_alarmPostPrefix;
		} else {
			alarmPostPrefix = "group_name=dpool&grade=-1&service_name=dip-logtail&object=scribe&subject="; 
		}
		
		
		if(getString("secrekey",temp_secrekey)){
			secrekey=temp_secrekey;	
		}else{
			secrekey="null";
		}
		
		if(getString("type",temp_type)){
			type=temp_type;
		}else{
			LOG_OPER("ERROR  : modelname :[%s] ,config  [  type ] is necessary  ",modelname.c_str());
			return false;
		}
		
		if(getString("from",temp_from)){
			from=temp_from;
		}else{
			LOG_OPER("ERROR  : modelname :[%s] ,config  [  from ] is necessary  ",modelname.c_str());
			return false;
		}

		if(getString("path",temp_path)){
            path=temp_path;
        }else{
			LOG_OPER("ERROR  : modelname :[%s] ,config  [  path ] is necessary  ",modelname.c_str());
            return false;
        }

		if(getString("set",temp_set)){
            set=temp_set;
        }else{
			LOG_OPER("ERROR  : modelname :[%s] ,config  [  set ] is necessary  ",modelname.c_str());
            return false;
        }   		
		
		if(getString("max_send_msg",temp_max_send_msg)){
            long t_max_send_msg=StringToInt(temp_max_send_msg);
			if(t_max_send_msg<=0){
				max_send_msg=5000;
			}else{
				max_send_msg=t_max_send_msg;	
			}
        }else{
            max_send_msg=5000;
        }
		
		if(getString("alarm_timeout",temp_alarm_timeout)){
			unsigned long t_alarm_timeout = StringToInt(temp_alarm_timeout);
			if(t_alarm_timeout<=0){
				alarm_timeout= 10;
			}else{
				alarm_timeout= t_alarm_timeout;
			}
		}else{
			alarm_timeout = 10;
		}
		if(getString("checkpoint_time",temp_checkpoint_time)){
			long long t_checkpoint_time=StringToInt(temp_checkpoint_time);
			if(t_checkpoint_time<=0){
				checkpoint_time=5;
			}else{
				checkpoint_time=t_checkpoint_time;
			}
		}else{
			checkpoint_time=5;
		}

		if(getString("backup_path",temp_backup_path)){
			backup_path=temp_backup_path;
		}else{
			backup_path="null";
		}
		
		if(getString("mailto_alarm",temp_mailto_alarm)){
            mailto_alarm=temp_mailto_alarm;
        }else{
            mailto_alarm="null";
        }  

		if(getString("phone_alarm",temp_phone_alarm)){
            phone_alarm=temp_phone_alarm;
        }else{
            phone_alarm="null";
        }
		
        if(getString("log_server_port",temp_log_server_port)){
            log_server_port=temp_log_server_port;
        }else{
            log_server_port="null";
        }
        
		if(getString("timeout",temp_timeout)){
			timeout=temp_timeout;
		}else{
			timeout="5000";
		}
		
		return true;
}	
