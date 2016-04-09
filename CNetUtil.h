#ifndef __NET_UTIL_H
#define __NET_UTIL_H

#include <stdint.h>
#include <string>

class CNetUtil
{
public:
	/**
     * 得到本机ip
     */
    static uint32_t getLocalAddr(const char *dev_name);
    /**
     * ip是本机ip地址, true - 是, false - 不是
     */
    static bool isLocalAddr(uint32_t ip, bool loopSkip = true);
    /**
     * 把字符串的ip转成int
     * 如 10.0.100.89 => 1499725834
     */
    static uint32_t getAddr(const char *ip);
    /**
     * 把uint64转成字符串
     */
    static std::string addrToString(uint64_t ipport);
    /**
     * 把ip,port转成uint64_t
     */
    static uint64_t strToAddr(const char *ip, int port);
    /**
     * 把ip,port转成uint64_t
     */
    static uint64_t ipToAddr(uint32_t ip, int port);
};

#endif //__NET_UTIL_H
