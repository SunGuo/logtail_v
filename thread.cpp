#include "ModelRead.h"
#include <iostream>
#include "thread.h"
#include "conf.h"
#include "dip_logtail_util.h"
#include <map>

 using namespace std;


class MyThread:public Thread
{
	public:
		~MyThread();
        void run();
		void setmodel(ModelRead *t_model);
		ModelRead *model;
};
void MyThread::run()
{
	model->start();
}

void MyThread::setmodel(ModelRead *t_model){
	model=t_model;
}

MyThread::~MyThread(){
	//delete model;
}

/*
*
*  记录程序pid的，已让程序检查程序是否正常启动了
*/

void write_pid()
{
	std::ofstream f;
	std::stringstream ss;
	ss<<getpid();
	f.open("client_pid.pid",std::ios::out);
	std::string content_pid=ss.str();
	if(f.good())
	{
		f<<content_pid;
    }
	f.close();
}

int main(int argc,char **argv){

    int oc; 
    int argumentid=0;
    char *config_path;//存储配置文件的目录
    std::string filename;
    while((oc=getopt(argc,argv,"hvf:r:"))){
       switch(oc){
           case 'h':
               printf("Usage: scribeclient [OPTION]...  [CONFIG_FILE]\n");
			   printf("-f the path of config\n");
			   printf("-h display this help and exit\n");
			   printf("-v print product version and exit\n");
			   printf("-r reload the config\n");
               argumentid=0;
               break;
           case 'f':
               config_path=optarg;
               argumentid=1;
               break;
		   case  'v':
		   	   argumentid=2;
			   printf("dip_logtail version \"0.0.3_1_20120405\"\n");
			   printf("for tailing user's log \n");
			   break;
		   case  'r':
			   config_path=optarg;
			   argumentid=3;
			   break;
 		   case '?':
               printf("argument error\n");
               argumentid=4;
               break;
		   default:
            printf("Usage: scribeclient [OPTION]...[CONFIG_FILE]\n");
            printf("-f the path of config\n");
            printf("-h display this help and exit\n");
			printf("-v print product version and exit\n");
			printf("-r reload the config\n");
            argumentid=5;
            break;
       }
       break;
   }
   switch(argumentid){
        case 0:
        case 2:
		case 4:
		case 5:
            return 0;
        case 1:
		case 3:
            break;
    }
		filename=std::string(config_path);
		conf co;
		dip_logtail_util logtail_util;  //读取  本地的ip与本机的盘点号的工具类
		logtail_util.use_util();
		std::string ip=logtail_util.get_ip();
		std::string an=logtail_util.get_an();
		write_pid();
	if(argumentid==1){
		
		/*
		 * 参数为 f  正常启动
		 */

		co.parseConfig(filename);
		std::map<string,map<string,string> > mapvalues=co.getConfmodels();
		ModelRead models[mapvalues.size()];
		MyThread threads[mapvalues.size()];
		int n=0;
		bool flag_model=true;
		for (map<string,map<string,string> >::iterator iter=mapvalues.begin(); iter != mapvalues.end( ); ++iter)
		{

			std::map<std::string,std::string> values=(*iter).second;
			models[n].set_modelconfig(values);
			flag_model=models[n].parseModel();
			models[n].setmodelname((*iter).first);
			models[n].setisreload(false);
			models[n].set_ipandan(ip,an);
			models[n].make_prefix_category();
			n++;
		}

		ModelRead *p1;
		p1=&models[0];
		
		/*
		* 启动线程
		*/
		for(int d=0;d<mapvalues.size();d++){
			threads[d].setmodel(p1);
			threads[d].start();
			p1++;
		}
    
		/*
		* 守护线程
		*/
		for(int s=0;s<mapvalues.size();s++){
			threads[s].mythread_join();
		} 
	}else if(argumentid==3){
		/*
		 * 如果参数为 r  对比配置文件与相关checkpoint文件。
		 */
        co.parseConfig(filename);
        std::map<string,map<string,string> > mapvalues=co.getConfmodels();
        ModelRead models[mapvalues.size()];
        MyThread threads[mapvalues.size()];
        int n=0;
        bool flag_model=true;
        for (map<string,map<string,string> >::iterator iter=mapvalues.begin(); iter != mapvalues.end( ); ++iter)
        {   

            std::map<std::string,std::string> values=(*iter).second;
            models[n].set_modelconfig(values);
            flag_model=models[n].parseModel(); 
            models[n].setmodelname((*iter).first);
			models[n].setisreload(false);
			std::map<std::string,std::string> m_checkpoint;
			co.getcheckpoint((*iter).first,m_checkpoint);
			std::cout<<m_checkpoint.find("check_time")->second<<std::endl;
            models[n].reload(m_checkpoint);
            models[n].set_ipandan(ip,an);
            models[n].make_prefix_category();
            n++;
        }
		ModelRead *p1;
        p1=&models[0];
        /*  
        * 启动线程
        */
        for(int d=0;d<mapvalues.size();d++){                                                                                                                            
            threads[d].setmodel(p1);
            threads[d].start();
            p1++;
        }
    
        /*
        * 守护线程
        */
        for(int s=0;s<mapvalues.size();s++){
            threads[s].mythread_join();
        } 

	}
	return 0;
}
