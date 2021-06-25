#pragma once
#include <string>
typedef enum {
	DPT_Int = 0,
	DPT_Float,
	DPT_Vector,
	DPT_VectorXY,
	DPT_String,
	DPT_Array,
	DPT_DataTable,
	DPT_Int64,
	DPT_NUMSendPropTypes
} Type;

struct recvProxyData { //-V802
	int pad;
	union {
		float _float;
		long _int;
		char* _string;
		void* data;
		float _x;
		float _y;
		float _z;
		int64_t int64;
	} value;
	Type m_Type;
};

using recvProxy = std::add_pointer_t<void(recvProxyData&, void*, void*)>;

struct RecvProp {
	char* name;
	int type;
	int flags;
	int stringBufferSize;
	int insideArray;
	const void* extraData;
	RecvProp* arrayProp;
	void* arrayLengthProxy;
	recvProxy proxy;
	void* dataTableProxy;
	struct RecvTable* dataTable;
	int offset;
	int elementStride;
	int elementCount;
	const char* parentArrayPropName;
};

struct RecvTable {
	RecvProp* props;
	int propCount;
	void* decoder;
	char* netTableName;
	bool isInitialized;
	bool isInMainList;
};
