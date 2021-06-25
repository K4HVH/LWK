#pragma once

#include "Recv.hpp"
#include "Enums.hpp"

class ClientClass;
class IClientNetworkable;

typedef IClientNetworkable* (*CreateClientClassFn)(int entnum, int serialNum);
typedef IClientNetworkable* (*CreateEventFn)();

class ClientClass {
public:
	CreateClientClassFn m_pCreateFn;
	CreateEventFn m_pCreateEventFn;
	char *m_pNetworkName;
	RecvTable *m_pRecvTable;
	ClientClass *m_pNext;
	ClassID m_ClassID;
	std::add_pointer_t<IClientEntity* __cdecl(int, int)> createFunction;// ты уверен что этой строчкой ты не ломаешь всё что ниже? xD
};