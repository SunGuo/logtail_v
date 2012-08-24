#include "common.h"
#include "scribe_send.h"
#include <limits.h>
#include <queue>
/*
*	使用命名管道的时候维护的缓冲区大小
*/
#define BUFFER_SIZE PIPE_BUF

class ModelRead{
	public:
		ModelRead();
		bool getString(const std::string &stringName,std::string& _returnvalue);
		bool openLogFile();
		bool openFIFOFile();
		void readFIFO();
		bool parseModel();
		void set_modelconfig(const std::map<std::string,std::string>& model_config);
		void start();
		void setModeltime();
		void logtail_File();
		void readLogFile();
		void readLogFile_test();
		void make_prefix_category();
		long long  StringToInt(const std::string temp);
		void set_ipandan(std::string eth0ip,std::string an);
		void stillGood();
		void dip_logtail_alarm_interface(std::string message,std::string subject);
		void dip_logtail_alarm_libcurlinterface(std::string message,std::string subject);
		void setmodelname(const std::string t_modelname);
		void Log_Write(std::string messges);
		void remove_char(std::string &str);
		void read_backupfile();
		void setisreload(bool t_isreload);
		void reload(std::map<std::string,std::string> m_checkpoint);
		void create_dir(const std::string dir_name);
		bool isdir(std::string path);
		void write_checkpoint();
		~ModelRead();
	private:
		/*
		 *判断是否是进行重载配置文件工作
		 */
		bool isreload;
		/*
		*   checkpoint_time: 更新checkpoint的时间间隔，单位s（秒）默认300s ,用户可以根据自己机器的稳定性适当调整
		*   now :  现在的秒数 
		*   pass_time: 已经过的秒数
		*/

		long long checkpoint_time;
		time_t now;
        long long pass_time;
		
		/*
		 * 报警的超时时间 
		 */

		unsigned long  alarm_timeout;
		/*
		* 报警的URL地址
		*/
		std::string alarmURL;
		
		/*
		*报警使用的前缀
		*/
		std::string alarmPostPrefix;
		
		/*
		*做为报警使用的默认组电话和默认组邮箱
		*/
		std::string defaultphone;
		std::string defaultmail;
		/*
		* 定义一个modelname作为一个model的唯一标示
		*/
		 std::string modelname;
		
		/*
		*  通过对文件描述符的监控判断 用户日志的回滚以及日志文件移走的一些动作进行监控，来调整读取文件的一些操作；
		*/

		 struct stat n_stat;  
		 struct stat o_stat;
		
		/*
		* 发送日志的变量
		*/

		scribe_send scribe_inter;
		
		/*
		* 记录读取文件的位置
		*/

		long long position;
		
		/*
		* 用户的日志前缀：_accesskey={accesskey}&_ip={from}=&_port={log_server_port}&_an={an}&_{an}&_data=
		*/
		
		std::string log_prefix;
		
		/*
		* 本地ip，本地an(盘点号)；
		*/
		
		std::string local_an;
		std::string local_eth0_ip;
		
		/*
		* 每个model都会维护一个消息队列；
		*/
		
		std::queue<std::string> message_queue;
		
		/*
		* 命名管道维护的打开标志
		*/
		
		int fd;
		const char *FIFO;
		
		/*
		* 用c++描述一个文件描述符，ifStream
		*/

		std::ifstream logreadfile;
		const char *logfilename;
		/*
		*   连接scribe 用的host与 port
		*/
		std::string port;
		std::string host;
		
		/*
		*	用来拼成 用户的category与日志前缀
		*   accesskey 用户在我们系统申请
		*   secrekey  与secrekey对应暂时不启用，用来验证用户的日志
		*   type  用户的日志类型：www等
		*   log_server_port  用户日志的服务端口
		*   set   apache, 等
		*   from  日志来源ip
		*   我们通过 type_accesskey_set得出用户的category，
		*   日志前缀 _accesskey={accesskey}&_ip={from}=&_port={log_server_port}&_an={an}&_{an}&_data={data}
		*   其中an为机器盘点号，如果得不到机器的盘点号,将用eth0的ip代替
		*   
		*/
		
		std::string category;


		std::string accesskey;
        std::string secrekey;
        std::string type;
        std::string path;
		std::string backup_path;
		std::string log_server_port;
        std::string set;
        std::string from;
		
		/*
		*	mailto_alarm用户的报警邮箱列表，多个邮箱用","隔开
		*   phone_alarm 用户的报警电话接口多个报警电话用","隔开
		*/
		
		std::string mailto_alarm;
		std::string phone_alarm;
		
		/*
		* send_cycle用户用来推送用户日志时的发送周期（此参数用来调整占用用户cpu过高）默认值为50，用户不用修改
		* timeout用户设置的超时时间
		*/
		
		long max_send_msg;
		std::string timeout;
		
		/*
		* logtail_log_path   用户配置的logtail的日志目录 例如 配置为  logtail_log_path=/data0/log/  将在/data0/log/目录下生成logtail的sccl日志	
		*/

		//std::string logtail_log_path;
		
		std::map<std::string,std::string> modelconfig;
};
