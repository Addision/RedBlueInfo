#include "CConfig.h"
#include <fstream>
#include <stdio.h>

CConfig* CConfig::config = NULL;
CConfig* CConfig::getCConfig()
{
	if(config == NULL)
		config = new CConfig();
	return config;
}

CConfig::CConfig()
{}

CConfig::~CConfig()
{
	for(STR_MAP_ITER it = m_config.begin(); it != m_config.end(); ++it)
	{
		delete it->second;			
	}
}

bool CConfig::Load(const std::string& configFile)
{
	if(configFile.empty())
	{
		return false;
	}
	std::fstream fin(configFile.c_str(), std::ios::in);
	if(!fin)
	{
		std::cout << "open file err\n";
		return false;
	}

	char buf[256];
	STR_STR_MAP *m = NULL;
	std::string key, value;
	std::string strLine;
	while(!fin.eof())
	{
		fin.getline(buf, 256);
		if(buf == NULL || buf[0]=='#' || buf[0]==';' || buf[0]=='\0')
			continue;
		strLine = buf;
		Trim(strLine);
		if(isSection(strLine))
		{
		    STR_MAP_ITER it = m_config.find(strLine);
			if(it == m_config.end())
			{
				m = new STR_STR_MAP();
				m_config.insert(std::make_pair<std::string, STR_STR_MAP*>(strLine, m));
			}
			else
			{
			    m = it->second;
			}
			continue;
		}
		if(!parseValue(strLine, key, value))
			continue;
		if(m == NULL)
		{
			std::cout << "don't config the section\n";
			fin.close();
			return false;
		}
		STR_STR_MAP_ITER it2 = m->find(key);
		if(it2 == m->end())
		{
			value += '\0';
			m->insert(std::make_pair<std::string, std::string>(key, value));
		}
		else
		{
			it2->second += value;
			it2->second += '\0';
		}
		
	}
	fin.close();
	return true;
}

std::string CConfig::getValueString(std::string section, std::string key, std::string value)
{
	std::string realValue = "";
	STR_MAP_ITER it = m_config.find(section);
	if(it == m_config.end())
		return value;
	STR_STR_MAP* m = it->second;
	if(m == NULL)
		return value;
	STR_STR_MAP_ITER it2 = m->find(key);
	if(it2 == m->end())
		return value;
	
	realValue = it2->second;
	if(realValue.size() == 0)
		realValue = value;
	return realValue;
}

int CConfig::getValueInt(std::string section, std::string key, int value)
{
	char buf[256];
	sprintf(buf, "%d", value);
	return atoi(getValueString(section, key, buf).c_str());
}

bool CConfig::getValueBool(std::string section, std::string key, bool value)
{
	std::string res;
	res = getValueString(section, key, value ? "true":"false");
	// if(res.compare("true") == 0)
	// 	return true;
    // else
	// 	return false;
	if(res.find("true") != std::string::npos)
		return true;
	else
		return false;
}

void CConfig::getStringList(std::string section, std::string key, std::vector<std::string>& vec)
{
	STR_MAP_ITER it = m_config.find(section);
	if(it == m_config.end())
		return;
	STR_STR_MAP *m = it->second;
	if(m == NULL)
		return;
	STR_STR_MAP_ITER it2 = m->find(key);
	if(it2 == m->end())
		return;
	else
	{
		std::string str = it2->second;
		int pos = str.find('\0');
		while( pos != std::string::npos)
		{
			vec.push_back(str.substr(0, pos));
			str = str.substr(pos+1);
			pos = str.find('\0');
		}
	}
	
}
void CConfig::getIntList(std::string section, std::string key, std::vector<int>& vec)
{
	STR_MAP_ITER it = m_config.find(section);
	if(it == m_config.end())
		return;
	STR_STR_MAP *m = it->second;
	if(m == NULL)
		return;
	STR_STR_MAP_ITER it2 = m->find(key);
	if(it2 == m->end())
		return;
	else
	{
		std::string str = it2->second;
		int pos = str.find('\0');
		while( pos != std::string::npos)
		{
			vec.push_back(atoi((str.substr(0, pos).c_str())));
			str = str.substr(pos+1);
			pos = str.find('\0');
		}
	}
}

void CConfig::toString()
{
	STR_MAP_ITER it = m_config.begin();
	for(;it != m_config.end(); ++it)
	{
		std::cout <<"["<< it->first << "]"<<"\n";
		if(it->second != NULL)
		{
			STR_STR_MAP_ITER it2 = it->second->begin();
			for(; it2 != it->second->end(); ++it2)
			{
				std::cout << it2->first << " = " << it2->second << "\n";
			}
		}
	}	
}



