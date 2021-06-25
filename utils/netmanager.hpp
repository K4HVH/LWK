#pragma once

#include "..\sdk\misc\Recv.hpp"
#include <unordered_map>

class netvars
{
public:
	std::unordered_map<std::string, RecvTable*> tables;

	int get_offset(const char *tableName, const char *propName); 
	int get_prop(const char *tableName, const char *propName, RecvProp **prop = 0);
	int get_prop(RecvTable *recvTable, const char *propName, RecvProp **prop = 0);

	static netvars& get()
	{
		static netvars instance;
		return instance;
	}

private:
	RecvTable *get_table(const char *tableName);
};