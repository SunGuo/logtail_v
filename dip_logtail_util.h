#include "common.h"

class dip_logtail_util{
	public:
		dip_logtail_util();
		~dip_logtail_util();
		void get_local_ip();
		void get_local_an();
		std::string get_ip();
		std::string get_an();
		void use_util();
	private:
		static std::string trim_String(const std::string& str);
		std::string an;
		std::string eth0_ip;
};
