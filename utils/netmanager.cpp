// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "netmanager.hpp"
#include <cctype>
#include "..\includes.hpp"

int netvars::get_offset(const char *tableName, const char *propName) 
{
	int offs = get_prop(tableName, propName);

	if (!offs)
		return 0;

	return offs;
}

int netvars::get_prop(const char *tableName, const char *propName, RecvProp **prop) 
{
	RecvTable *recvTable = this->get_table(tableName);

	if (!recvTable)
		return 0;

	int offs = get_prop(recvTable, propName, prop);

	if (!offs)
		return 0;

	return offs;
}

int netvars::get_prop(RecvTable *recvTable, const char *propName, RecvProp **prop) 
{
	int extrOffs = 0;

	for (int i = 0; i < recvTable->propCount; i++) 
	{
		auto *recvProp = &recvTable->props[i];
		auto recvChild = recvProp->dataTable;

		if (recvChild && (recvChild->propCount > 0)) 
		{
			int tmp = get_prop(recvChild, propName, prop);

			if (tmp)
				extrOffs += (recvProp->offset + tmp);
		}

		if (strcmp(recvProp->name, propName)) //-V526
			continue;

		if (prop)
			*prop = recvProp;

		return (recvProp->offset + extrOffs);
	}

	return extrOffs;
}

RecvTable *netvars::get_table(const char *tableName) 
{
	if (tables.empty())
		return 0;

	for (auto table : tables) 
		if (!strcmp(table.first.c_str(), tableName))
			return table.second;

	return 0;
}