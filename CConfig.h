#ifndef __CONFIG_H
#define __CONFIG_H

#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <vector>

#ifdef MAP
#include <map>
using std::map;

typedef std::map<std::string, std::string> STR_STR_MAP;
typedef std::map<std::string, STR_STR_MAP*> STR_MAP;
typedef STR_STR_MAP::iterator STR_STR_MAP_ITER;
typedef STR_MAP::iterator STR_MAP_ITER;
#else
//#include "CHash.h"
#include <ext/hash_map>
using __gnu_cxx::hash_map;
struct str_hash{
    unsigned int operator()(const std::string& str)const
	{
		return __gnu_cxx::__stl_hash_string(str.c_str());
	}
};
// struct str_hash{
// 	unsigned int operator()(const std::string& str) const
// 	{
// 		return CHash::HashA(const_cast<char*>(str.c_str()), str.size());
// 	}
// };
typedef __gnu_cxx::hash_map<std::string, std::string, str_hash> STR_STR_MAP;
typedef __gnu_cxx::hash_map<std::string, STR_STR_MAP*, str_hash> STR_MAP;
typedef STR_STR_MAP::iterator STR_STR_MAP_ITER;
typedef STR_MAP::iterator STR_MAP_ITER;

#endif

struct char_equal{
    bool operator()(const char* s1, const char* s2) const
	{
		return (strcmp(s1, s2) == 0);
	}
};

struct string_equal{
	bool operator()(const std::string& s1, const std::string& s2) const
	{
		return (s1 == s2);
	}
};


class CConfig
{
public:
    CConfig();
    virtual ~CConfig();

	bool Load(const std::string& configFile);
	std::string getValueString(std::string section, std::string key, std::string value);
	int getValueInt(std::string section, std::string key, int value);
	bool getValueBool(std::string section, std::string key, bool value);
	void getStringList(std::string section, std::string key, std::vector<std::string>& vec);
	void getIntList(std::string section, std::string key, std::vector<int>& vec);
	
	void toString();
	static CConfig* getCConfig();
private:
	std::string Trim(std::string& str)
	{
		str.erase(0, str.find_first_not_of(" "));
		str.erase(str.find_last_not_of(" ")+1);
		return str;
	}
	bool isSection(std::string& str)
	{
		int start = str.find_first_of('[');
		int end = str.find_first_of(']');
		if(start !=std::string::npos && end > start)
		{
			str = str.substr(start+1, end-start-1);
			Trim(str);
			if(!str.empty())
				return true;
		}
		return false;
	}
	bool parseValue(std::string& str, std::string& key, std::string& value)
	{
		int pos = str.find("=");
		if(pos == std::string::npos)
			return false;
		key = str.substr(0, pos);
		Trim(key);
		value = str.substr(pos+1, str.size()-pos -1);
		Trim(value);
		if(!key.empty() && !value.empty())
		{
			return true;
		}
		return false;
	}
    
private:
	STR_MAP m_config;
	static CConfig* config;
};

#define Config CConfig::getCConfig()
					   
#endif //__CONFIG_H
