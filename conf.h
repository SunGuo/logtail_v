#include "common.h"


typedef std::map<std::string,std::string> string_map_t;   		//声明一个Model下的存储配置文件键值对的map
typedef std::map<std::string,string_map_t> string_map_model;    //声明一个config下所有的models的map

class conf{
	public:
		conf();
		bool getString(const std::string& stringName,string_map_t& _return);//返回一个选项（配置文件爱那个中相应的values的值）
		void parseConfig(const std::string &filename);
		string_map_model getConfmodels();
		bool getcheckpoint(const std::string t_modelname,std::map<std::string,std::string>& _return_map);
		virtual ~conf();
	private:
		string_map_model models;
		static std::string trimString(const std::string& str);
		static bool parseModel(/*in,out*/ std::queue<std::string>& raw_config,/*out*/ conf* parsed_config);
		bool readConfFile(const std::string& filename, std::queue<std::string>& _return);
};
