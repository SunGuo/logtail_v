#include "conf.h"
#include "common.h"



conf::conf(){
}

conf::~conf(){
}
void conf::parseConfig(const std::string &filename){
	std::queue<std::string> config_strings;

	if(readConfFile(filename, config_strings)) {
  	}else{
		std::string msg="Failed to open config file <";
    	msg =msg.append(filename).append(">");
		std::cout<<msg<<std::endl;
  	}
  
  	parseModel(config_strings, this);
}
//去掉前后空格的函数
std::string conf::trimString(const std::string& str){
  	std::string whitespace = " \t";
  	size_t start	  =str.find_first_not_of(whitespace);
  	size_t end        =str.find_last_not_of(whitespace);

  	if(start != std::string::npos) {
    	return str.substr(start, end - start + 1); 
  	}else{
    	return ""; 
  	}
}
string_map_model conf::getConfmodels(){
	return models;
}
bool conf::getcheckpoint(const std::string t_modelname,std::map<std::string,std::string>& _return_map){
	std::ifstream checkpoint_file;
	std::string prefix_f="checkpoint/";
	std::string t_filename=t_modelname;
	std::string t_content;
	t_filename=prefix_f.append(t_filename).append(".checkpoint");
	checkpoint_file.open(t_filename.c_str(),std::ios::in);
	if(checkpoint_file.fail()){
		LOG_OPER("ERROR :modelname :the checkpointfile [%s] failed",t_filename.c_str());
		return false;
	}else{
		getline(checkpoint_file,t_content);
		std::string check_time=t_content.substr(0,t_content.find_first_of(" "));
		_return_map["check_time"]=check_time;
		t_content=t_content.substr(t_content.find_first_of(" ")+1);
		std::string check_position=t_content.substr(0,t_content.find_first_of(" "));
		_return_map["check_position"]=check_position;
		t_content=t_content.substr(t_content.find_first_of(" ")+1);
		std::string check_path=t_content.substr(0,t_content.length());
		_return_map["check_path"]=check_path;
		return true;
	}
}
bool conf::getString(const std::string& stringName,string_map_t& _return){
   	bool found=false;
   	string_map_model::const_iterator iter = models.find(stringName);
   	if (iter != models.end()) {
    	_return = iter->second;
     	found=true;
   	}
   	return found;
 }

bool conf::parseModel(/*in,out*/ std::queue<std::string>& raw_config,/*out*/ conf* parsed_config){

  std::string line;
  std::string model_name;
  string_map_t values; 
  int dd=0;
  while (!raw_config.empty()) {
    line = raw_config.front();
    raw_config.pop();

    // remove leading and trailing whitespace
    line = trimString(line);

    // remove comment
    size_t comment = line.find_first_of('#');
    if (comment != std::string::npos) {
      line.erase(comment);
    }   

    int length = line.size();
    if (0 >= length) {
      continue;
    }
   
   	if(line[0]=='['){
		std::string::size_type pos = line.find(']');
		if (pos == std::string::npos) {
      		LOG_OPER("Bad config - line %s has a [  but not a  ]", line.c_str());
			return false;  
      	}
		
		if(model_name.size()>0){
			parsed_config->models[model_name]=values;
		}
	  	model_name = line.substr(1, pos - 1);
		if(0<values.size()){
			values.clear();
		}
      	if (parsed_config->models.find(model_name) != parsed_config->models.end()) {
			LOG_OPER("Bad config - duplicate model name [ %s ]", model_name.c_str());
			return false;
		}   
        //parsed_config->models[model_name] = new_store;
	}else {

	  		std::string spilt="=";
      		std::string::size_type eq = line.find(spilt);
      		if (eq == std::string::npos) {
        		LOG_OPER("Bad config - line is missing an =  [ %s ]",line.c_str());
      		} else {
        		std::string arg = line.substr(0, eq);
        		std::string val = line.substr(eq + spilt.length(), std::string::npos);

        	// remove leading and trailing whitespace
        	arg = trimString(arg);
        	val = trimString(val);

        	if (values.find(arg) != values.end()) {
          		LOG_OPER("Bad config - duplicate key [ %s ]",arg.c_str());
        	}
        	values[arg] = val;
      	}
	}

    }
	if(model_name.size()>0){
		parsed_config->models[model_name]=values;
	}
  return true;
}

bool conf::readConfFile(const std::string& filename, std::queue<std::string>& _return){
	std::string line;
	std::ifstream config_file;
	
	config_file.open(filename.c_str());
	if (!config_file.good()) {
		LOG_OPER("Error the file [ %s ] is not exist",filename.c_str());
		return false;
	}
	
	while (getline(config_file, line)) {
    	_return.push(line);
	}

	config_file.close();
	return true;
}

