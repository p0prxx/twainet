#ifndef INET_HEADERS_H
#define INET_HEADERS_H

#include "connector_lib\handler\data_message.h"
#include "common\net_structs.h"
#include <map>

class EtherNetContainer : public DataMessage
{
public:
	EtherNetContainer();
	EtherNetContainer(const std::string& srcmac, const std::string& dstmac);
	virtual ~EtherNetContainer();
public:
	virtual bool serialize(char* data, int len);
	virtual bool deserialize(char* data, int& len);
protected:
	virtual bool SerializeData(const std::string& data);
	virtual bool DeserializeData(std::string& data);
public:
	ether_header m_ethHeader;
	EtherNetContainer* m_child;
};

class PPPoEContainer : public EtherNetContainer
{
public:
	PPPoEContainer();
	PPPoEContainer(const std::string& srcmac, const std::string& dstmac, unsigned char code);
	virtual ~PPPoEContainer();
protected:
	virtual bool SerializeData(const std::string& data);
	virtual bool DeserializeData(std::string& data);
public:
	pppoe_header m_pppoeHeader;
};

class PPPoEDContainer : public PPPoEContainer
{
public:
	PPPoEDContainer();
	PPPoEDContainer(const std::string& srcmac, const std::string& dstmac, unsigned char code);
	virtual ~PPPoEDContainer();
protected:
	virtual bool SerializeData(const std::string& data);
	virtual bool DeserializeData(std::string& data);
public:
	std::map<int, std::string> m_tags;
};

#endif/*INET_HEADERS_H*/