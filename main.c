//	Author: Zoo
//	QQ:276793422
//
#include "ListLayer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memleak.h"

#define ZooLogV(...) {{printf("[ZooDriver] " __VA_ARGS__);}}

#if 0
#include "TrieTree.h"

typedef char CHAR;

//	创建一个TRIE树，这个是基础，必须提供
PVOID AllocTrieMemory(ULONG uLen)
{
	printf("[%s] \n", __FUNCTION__);
	return malloc(uLen);
}

//	销毁一个TRIE树，这个是基础，必须提供
PVOID DestroyTrieMemory(PVOID p)
{
	printf("[%s] \n", __FUNCTION__);
	free(p);
	return NULL;
}

ULONG GetSpecifyRuleCmpFunc(PVOID pTrieTree, WCHAR *wsKey, PVOID pParam, PVOID pData)
{
	printf("GetSpecifyRuleCmpFunc , pParam = %d, wsKey = %S , p = %s \n", pParam, wsKey, pData);
	return 1;
}

PVOID AllocDataPool(PVOID pTrieTree)
{
	printf("[%s] \n", __FUNCTION__);
	return NULL;
}

PVOID AllocData(PVOID pTrieTree, PVOID pool, ULONG uLen, PVOID data)
{
	printf("[%s] \n", __FUNCTION__);
	return data;
}

PVOID FreeData(PVOID pTrieTree, PVOID pool, PVOID data)
{
	printf("[%s] \n", __FUNCTION__);
	return NULL;
}

PVOID DestroyData(PVOID pTrieTree)
{
	printf("[%s] \n", __FUNCTION__);
	return NULL;
}



PVOID AllocNodePool(PVOID pTrieTree)
{
	printf("[%s] \n", __FUNCTION__);
	return NULL;
}

PVOID AllocNode(PVOID pTrieTree, PVOID pool, ULONG uLen)
{
	PVOID p = NULL;
	printf("[%s] \n", __FUNCTION__);
	p = malloc(uLen);
	if (p)
	{
		memset(p, 0, uLen);
	}
	return p;
}

PVOID FreeNode(PVOID pTrieTree, PVOID pool, PVOID data)
{
	printf("[%s] \n", __FUNCTION__);
	return NULL;
}

PVOID DestroyNode(PVOID pTrieTree)
{
	printf("[%s] \n", __FUNCTION__);
	return NULL;
}

PVOID MemorySet(PVOID pTrieTree, PVOID _Dst, ULONG _Val, ULONG _Size)
{
	return memset(_Dst, _Val, _Size);
}

int main()
{
	CHAR *str;
	PVOID pTrieTree = NULL;
	TRIE_TREE_MEMORY_FUNCTION Funa;
	WCHAR lwArray[10] = { L'\\', L'/' };
	Funa.AllocTrieMemory	= AllocTrieMemory;
	Funa.DestroyTrieMemory	= DestroyTrieMemory;
	Funa.AllocData			= AllocData;
	Funa.AllocDataPool		= AllocDataPool;
	Funa.AllocNode			= AllocNode;
	Funa.AllocNodePool		= AllocNodePool;
	Funa.DestroyData		= DestroyData;
	Funa.DestroyNode		= DestroyNode;
	Funa.FreeData			= FreeData;
	Funa.FreeNode			= FreeNode;
	Funa.MemSet				= MemorySet;
	Funa.GetSpecifyRuleCmp	= GetSpecifyRuleCmpFunc;

	TrieTree_Init(&pTrieTree, 4, &Funa, lwArray, 2);

	TrieTree_Insert(pTrieTree, L"D:\\1\\3\\main.c", "D:\\1\\3\\main.c");
	if (TrieTree_GetRule(pTrieTree, L"D:\\1\\3\\main.c", &str))
	{
		printf("del get str = %s \n", str);
	}
	else
	{
		printf("del get find error \n");
	}
	TrieTree_Insert(pTrieTree, L"D:\\1\\3\\", "D:\\1\\3\\");
	TrieTree_Insert(pTrieTree, L"D:\\1\\", "D:\\1\\");
	TrieTree_Insert(pTrieTree, L"D:\\DLL.c", "D:\\DLL.c");


	if (TrieTree_GetSpecifyRuleCmp(pTrieTree, L"D:\\1\\3\\main.c", (PVOID)1))
	{
		printf("str = %s \n", str);
	}

	if (TrieTree_Search(pTrieTree, L"D:\\1\\3\\main.c", &str))
	{
		printf("str = %s \n", str);
	}
	if (TrieTree_Search(pTrieTree, L"D:\\1\\3\\", &str))
	{
		printf("str = %s \n", str);
	}
	if (TrieTree_Search(pTrieTree, L"D:\\DLL.c", &str))
	{
		printf("str = %s \n", str);
	}
	if (TrieTree_Search(pTrieTree, L"D:\\1\\3\\main.c", &str))
	{
		printf("str = %s \n", str);
	}

	if (TrieTree_Delete(pTrieTree, L"D:\\1\\3\\main.c"))
	{
		if (TrieTree_Search(pTrieTree, L"D:\\1\\3\\main.c", &str))
		{
			printf("del search str = %s \n", str);
		}
		else
		{
			printf("del search find error \n");
		}
		if (TrieTree_GetRule(pTrieTree, L"D:\\1\\3\\main.c", &str))
		{
			printf("del get str = %s \n", str);
		}
		else
		{
			printf("del get find error \n");
		}
		if (TrieTree_GetRule_Right(pTrieTree, L"D:\\1\\3\\main.c", &str))
		{
			printf("del get str = %s \n", str);
		}
		else
		{
			printf("del get find error \n");
		}
	}

	if (TrieTree_Delete(pTrieTree, L"D:\\1\\"))
	{
		if (TrieTree_GetRule(pTrieTree, L"D:\\1\\3\\main.c", &str))
		{
			printf("del get str = %s \n", str);
		}
		else
		{
			printf("del get find error \n");
		}
	}

	printf("TrieTree_GetNodeOnUseCount() = %d \n", TrieTree_GetNodeOnUseCount(pTrieTree));
	printf("TrieTree_GetAllocNodeCount() = %d \n", TrieTree_GetAllocNodeCount(pTrieTree));
	printf("TrieTree_GetDataOnUseCount() = %d \n", TrieTree_GetDataOnUseCount(pTrieTree));
	printf("TrieTree_GetAllocDataCount() = %d \n", TrieTree_GetAllocDataCount(pTrieTree));
	printf("TrieTree_GetNodeSize() = %d \n", TrieTree_GetNodeSize(pTrieTree));
	printf("TrieTree_GetDataSize() = %d \n", TrieTree_GetDataSize(pTrieTree));

	TrieTree_Destroy(&pTrieTree);
	return 0;
}

#endif

//	根据uLen长度，创建一个List Layer，这个是基础，必须提供
PVOID ZooTypeFunc_ListLayer_AllocTrieMemory_Fun(ULONG uLen)
{
	ZooLogV("		%s() , uLen = %d \n", __FUNCTION__, uLen);
	return malloc(uLen);
}

//	销毁一个List Layer，这个是基础，必须提供，p
PVOID ZooTypeFunc_ListLayer_DestroyTrieMemory_Fun(PVOID p)
{
	ZooLogV("		%s() , p = 0x%08X \n", __FUNCTION__, p);
	free(p);
	return NULL;
}




//	创建一个RULE池，并且返回，uLen 就是池里面每个元素的长度，这里只是报备，后面每次申请也会提供长度的
PVOID ZooTypeFunc_ListLayer_InitListInfoPool_Fun(PVOID pThis, ULONG uLen)
{
	ZooLogV("		%s() , pThis = 0x%08X , uLen = %d \n", __FUNCTION__, pThis, uLen);
	return malloc(uLen);
}

//	销毁RULE pool
PVOID ZooTypeFunc_ListLayer_DestroyListInfoPool_Fun(PVOID pThis, PVOID pool)
{
	ZooLogV("		%s() , pThis = 0x%08X , pool = 0x%08X \n", __FUNCTION__, pThis, pool);
	free(pool);
	return NULL;
}

//	DATA数据申请函数，根据uLen长度，从pool 中申请一块内存
PVOID ZooTypeFunc_ListLayer_AllocListInfoFromPool_Fun(PVOID pThis, PVOID pool, ULONG uLen)
{
	ZooLogV("		%s() , pThis = 0x%08X , pool = 0x%08X , uLen = %d \n", __FUNCTION__, pThis, pool, uLen);
	return malloc(uLen);
}

//	DATA数据释放函数，从pool 中，释放data元素
PVOID ZooTypeFunc_ListLayer_FreeListInfoToPool_Fun(PVOID pThis, PVOID pool, PVOID data)
{
	ZooLogV("		%s() , pThis = 0x%08X , pool = 0x%08X , data = 0x%08X \n", __FUNCTION__, pThis, pool, data);
	free(data);
	return NULL;
}


//	初始化一个链表操作锁，并且返回
PVOID ZooTypeFunc_ListLayer_InitListLayerLock_Fun(PVOID pThis)
{
	ZooLogV("		%s() , pThis = 0x%08X \n", __FUNCTION__, pThis);
	return (PVOID)0x200;
}

//	销毁链表操作锁，也就是pLock
PVOID ZooTypeFunc_ListLayer_DestroyListLayerLock_Fun(PVOID pThis, PVOID pLock)
{
	ZooLogV("		%s() , pThis = 0x%08X , pLock = 0x%08X \n", __FUNCTION__, pThis, pLock);
	return NULL;
}

//	获取链表操作锁
PVOID ZooTypeFunc_ListLayer_GetListLayerLock_Fun(PVOID pThis, PVOID pLock)
{
	ZooLogV("		%s() , pThis = 0x%08X , pLock = 0x%08X \n", __FUNCTION__, pThis, pLock);
	return NULL;
}

//	释放链表操作锁
PVOID ZooTypeFunc_ListLayer_ReleaseListLayerLock_Fun(PVOID pThis, PVOID pLock)
{
	ZooLogV("		%s() , pThis = 0x%08X , pLock = 0x%08X \n", __FUNCTION__, pThis, pLock);
	return NULL;
}


//	初始化池操作读写锁
PVOID ZooTypeFunc_ListLayer_InitRuleInfoPoolLock_Fun(PVOID pThis)
{
	ZooLogV("		%s() , pThis = 0x%08X \n", __FUNCTION__, pThis);
	return (PVOID)0x201;
}

PVOID ZooTypeFunc_ListLayer_DestroyRuleInfoPoolLock_Fun(PVOID pThis, PVOID pLock)
{
	ZooLogV("		%s() , pThis = 0x%08X , pLock = 0x%08X \n", __FUNCTION__, pThis, pLock);
	return NULL;
}

PVOID ZooTypeFunc_ListLayer_GetRuleInfoPoolLockForRead_Fun(PVOID pThis, PVOID pLock)
{
	ZooLogV("		%s() , pThis = 0x%08X , pLock = 0x%08X \n", __FUNCTION__, pThis, pLock);
	return NULL;
}

PVOID ZooTypeFunc_ListLayer_ReleaseRuleInfoPoolLockForRead_Fun(PVOID pThis, PVOID pLock)
{
	ZooLogV("		%s() , pThis = 0x%08X , pLock = 0x%08X \n", __FUNCTION__, pThis, pLock);
	return NULL;
}

PVOID ZooTypeFunc_ListLayer_GetRuleInfoPoolLockForWrite_Fun(PVOID pThis, PVOID pLock)
{
	ZooLogV("		%s() , pThis = 0x%08X , pLock = 0x%08X \n", __FUNCTION__, pThis, pLock);
	return NULL;
}

PVOID ZooTypeFunc_ListLayer_ReleaseRuleInfoPoolLockForWrite_Fun(PVOID pThis, PVOID pLock)
{
	ZooLogV("		%s() , pThis = 0x%08X , pLock = 0x%08X \n", __FUNCTION__, pThis, pLock);
	return NULL;
}



//	内存设置函数，memset
PVOID ZooTypeFunc_ListLayer_MemorySet_Fun(PVOID pThis, PVOID _Dst, ULONG _Val, ULONG _Size)
{
	ZooLogV("		%s() , pThis = 0x%08X , _Dst = 0x%08X , _Val = %d , _Size = %d \n", __FUNCTION__, pThis, _Dst, _Val, _Size);
	return memset(_Dst, _Val, _Size);
}

//	内存拷贝 memcpy
ULONG ZooTypeFunc_ListLayer_MemoryCopy_Fun(PVOID pThis, PVOID _Dst, PVOID _Src, ULONG _Size)
{
	ZooLogV("		%s() , pThis = 0x%08X , _Dst = 0x%08X , _Src = 0x%08X , _Size = %d \n", __FUNCTION__, pThis, _Dst, _Src, _Size);
	memcpy(_Dst, _Src, _Size);
	return _Size;
}

//	内存拷贝 memcpy
ULONG ZooTypeFunc_ListLayer_MemoryCmp_Fun(PVOID pThis, PVOID _Dst, PVOID _Src, ULONG _Size)
{
	ZooLogV("		%s() , pThis = 0x%08X , _Dst = 0x%08X , _Src = 0x%08X , _Size = %d \n", __FUNCTION__, pThis, _Dst, _Src, _Size);
	return memcmp(_Dst, _Src, _Size);
}

LONG ZooTypeFunc_ListInfo_CompareRule_Fun(PVOID pRule1, PVOID pRule2, PVOID pOut)
{
	ZooLogV("		%s() , pRule1 = 0x%08X , pRule2 = 0x%08X , pOut = 0x%08X \n", __FUNCTION__, pRule1, pRule2, pOut);

	return 0;
}

PVOID ZooTypeFunc_RuleTable_InitRuleTable_Fun(PVOID pLayerInfo, PVOID pool)
{
	ZooLogV("		%s() , pLayerInfo = 0x%08X , pool = 0x%08X \n", __FUNCTION__, pLayerInfo, pool);
	return NULL;
}

PVOID ZooTypeFunc_RuleTable_DestoryRuleTable_Fun(PVOID pLayerInfo, PVOID pRuleTable, PVOID pool, PVOID pPoolLock)
{
	ZooLogV("		%s() , pLayerInfo = 0x%08X , pRuleTable = 0x%08X, pool = 0x%08X, pPoolLock = 0x%08X \n", __FUNCTION__, pLayerInfo, pRuleTable, pool, pPoolLock);
	return NULL;
}

BOOLEAN ZooTypeFunc_RuleTable_SearchRuleInTable_Fun(PVOID pLayerInfo, PVOID pRuleTable, PVOID pool, PVOID pPoolLock, PVOID pRule, PVOID pOut)
{
	ZooLogV("		%s() , pLayerInfo = 0x%08X , pRuleTable = 0x%08X, pool = 0x%08X, pPoolLock = 0x%08X, pRule = 0x%08X, pOut = 0x%08X \n", __FUNCTION__, pLayerInfo, pRuleTable, pool, pPoolLock, pRule, pOut);
	return FALSE;
}

BOOLEAN ZooTypeFunc_RuleTable_AddRuleToTable_Fun(PVOID pLayerInfo, PVOID pRuleTable, PVOID pool, PVOID pPoolLock, PVOID pRule)
{
	ZooLogV("		%s() , pLayerInfo = 0x%08X , pRuleTable = 0x%08X, pool = 0x%08X, pPoolLock = 0x%08X, pRule = 0x%08X \n", __FUNCTION__, pLayerInfo, pRuleTable, pool, pPoolLock, pRule);
	return FALSE;
}

BOOLEAN ZooTypeFunc_RuleTable_RemoveRuleFromTable_Fun(PVOID pLayerInfo, PVOID pRuleTable, PVOID pool, PVOID pPoolLock, PVOID pRule)
{
	ZooLogV("		%s() , pLayerInfo = 0x%08X , pRuleTable = 0x%08X, pool = 0x%08X, pPoolLock = 0x%08X, pRule = 0x%08X \n", __FUNCTION__, pLayerInfo, pRuleTable, pool, pPoolLock, pRule);
	return FALSE;
}

int main()
{
	MODE_INFO mode1 = {1, "Zoo1"};
	MODE_INFO mode2 = {2, "Zoo2"};
	MODE_INFO mode100 = {100, "Zoo100"};
	MODE_INFO mode51 = {51, "Zoo51"};
	MODE_INFO mode31 = {31, "Zoo31"};
	MODE_INFO mode51_2 = {51, "Zoo51_2"};
	MODE_INFO mode;
	PVOID ListLayer;
	PVOID pLayerInfoHead;
	LIST_FUNCTION_LIST lfl = { 0 };
	LIST_LAYER_MEMORY_FUNCTION lmf = { 0 };
	LIST_INFO_MEMORY_FUNCTION imf = { 0 };
	LIST_RULE_MEMORY_FUNCTION rmf = { 0 };
	ULONG i = 0;

	
	lmf.AllocListMemory = ZooTypeFunc_ListLayer_AllocTrieMemory_Fun;
	lmf.DestroyListMemory = ZooTypeFunc_ListLayer_DestroyTrieMemory_Fun;

	lmf.InitListLayerLock = ZooTypeFunc_ListLayer_InitListLayerLock_Fun;
	lmf.DestroyListLayerLock = ZooTypeFunc_ListLayer_DestroyListLayerLock_Fun;
	lmf.GetListLayerLock = ZooTypeFunc_ListLayer_GetListLayerLock_Fun;
	lmf.ReleaseListLayerLock = ZooTypeFunc_ListLayer_ReleaseListLayerLock_Fun;

	lmf.InitRuleInfoPoolLock = ZooTypeFunc_ListLayer_InitRuleInfoPoolLock_Fun;
	lmf.DestroyRuleInfoPoolLock = ZooTypeFunc_ListLayer_DestroyRuleInfoPoolLock_Fun;
	lmf.GetRuleInfoPoolLockForRead = ZooTypeFunc_ListLayer_GetRuleInfoPoolLockForRead_Fun;
	lmf.ReleaseRuleInfoPoolLockForRead = ZooTypeFunc_ListLayer_ReleaseRuleInfoPoolLockForRead_Fun;
	lmf.GetRuleInfoPoolLockForWrite = ZooTypeFunc_ListLayer_GetRuleInfoPoolLockForWrite_Fun;
	lmf.ReleaseRuleInfoPoolLockForWrite = ZooTypeFunc_ListLayer_ReleaseRuleInfoPoolLockForWrite_Fun;

	lmf.InitListInfoPool = ZooTypeFunc_ListLayer_InitListInfoPool_Fun;
	lmf.DestroyListInfoPool = ZooTypeFunc_ListLayer_DestroyListInfoPool_Fun;
	lmf.AllocListInfoFromPool = ZooTypeFunc_ListLayer_AllocListInfoFromPool_Fun;
	lmf.FreeListInfoToPool = ZooTypeFunc_ListLayer_FreeListInfoToPool_Fun;

	lmf.MemSet = ZooTypeFunc_ListLayer_MemorySet_Fun;
	lmf.MemCpy = ZooTypeFunc_ListLayer_MemoryCopy_Fun;
	lmf.MemCmp = ZooTypeFunc_ListLayer_MemoryCmp_Fun;


	imf.InitRulePool = ZooTypeFunc_ListLayer_InitListInfoPool_Fun;
	imf.DestroyRulePool = ZooTypeFunc_ListLayer_DestroyListInfoPool_Fun;
	imf.AllocRuleFromPool = ZooTypeFunc_ListLayer_AllocListInfoFromPool_Fun;
	imf.FreeRuleToPool = ZooTypeFunc_ListLayer_FreeListInfoToPool_Fun;

	imf.InitRuleTablePoolLock = ZooTypeFunc_ListLayer_InitRuleInfoPoolLock_Fun;
	imf.DestroyRuleTablePoolLock = ZooTypeFunc_ListLayer_DestroyRuleInfoPoolLock_Fun;
	imf.GetRuleTablePoolLockForRead = ZooTypeFunc_ListLayer_GetRuleInfoPoolLockForRead_Fun;
	imf.ReleaseRuleTablePoolLockForRead = ZooTypeFunc_ListLayer_ReleaseRuleInfoPoolLockForRead_Fun;
	imf.GetRuleTablePoolLockForWrite = ZooTypeFunc_ListLayer_GetRuleInfoPoolLockForWrite_Fun;
	imf.ReleaseRuleTablePoolLockForWrite = ZooTypeFunc_ListLayer_ReleaseRuleInfoPoolLockForWrite_Fun;

	imf.MemSet = ZooTypeFunc_ListLayer_MemorySet_Fun;
	imf.MemCpy = ZooTypeFunc_ListLayer_MemoryCopy_Fun;


	rmf.InitRuleTable = ZooTypeFunc_RuleTable_InitRuleTable_Fun;
	rmf.DestoryRuleTable = ZooTypeFunc_RuleTable_DestoryRuleTable_Fun;
	rmf.AddRuleToTable = ZooTypeFunc_RuleTable_AddRuleToTable_Fun;
	rmf.SearchRuleInTable = ZooTypeFunc_RuleTable_SearchRuleInTable_Fun;
	rmf.RemoveRuleFromTable = ZooTypeFunc_RuleTable_RemoveRuleFromTable_Fun;

	lfl.lmf = lmf;
	lfl.imf = imf;
	lfl.rmf = rmf;

	if (!ZooListLayer_InitListLayer(&ListLayer, 4, &lfl))
	{
		ZooLogV(" InitListLayer Error \n");
		return -1;
	}
	else
	{
		ZooLogV(" InitListLayer OK \n");
	}

	if (!ZooListLayer_GetLayerInfoFromListByIDAndCreate(ListLayer, &mode1, &pLayerInfoHead))
	{
		ZooLogV(" GetLayerInfoFromListByIDWithCreate mode1 Error \n");
	}
	else
	{
		ZooLogV(" GetLayerInfoFromListByIDWithCreate mode1 OK \n");
	}

	if (!ZooListLayer_CreateLayerInfoToList(ListLayer, &mode1, &pLayerInfoHead))
	{
		ZooLogV(" CreateLayerInfoToList mode1 Error \n");
	}
	else
	{
		ZooLogV(" CreateLayerInfoToList mode1 OK \n");
	}

	if (!ZooListLayer_GetLayerInfoFromListByIDAndCreate(ListLayer, &mode1, &pLayerInfoHead))
	{
		ZooLogV(" GetLayerInfoFromListByIDWithCreate mode1 Error \n");
	}
	else
	{
		ZooLogV(" GetLayerInfoFromListByIDWithCreate mode1 OK \n");
	}

	if (!ZooListLayer_GetLayerInfoFromListByIndex(ListLayer, 0, &pLayerInfoHead))
	{
		ZooLogV(" GetLayerInfoFromListByIndex index 0 Error \n");
	}
	else
	{
		ZooLogV(" GetLayerInfoFromListByIndex index 0 OK \n");
	}

	if (!ZooListLayer_GetLayerInfoFromListByIndex(ListLayer, 1, &pLayerInfoHead))
	{
		ZooLogV(" GetLayerInfoFromListByIndex index 1 Error \n");
	}
	else
	{
		ZooLogV(" GetLayerInfoFromListByIndex index 1 OK \n");
	}

	if (!ZooListLayer_GetLayerInfoFromListByIDAndCreate(ListLayer, &mode100, &pLayerInfoHead))
	{
		ZooLogV(" GetLayerInfoFromListByIDWithCreate mode100 Error \n");
	}
	else
	{
		ZooLogV(" GetLayerInfoFromListByIDWithCreate mode100 OK \n");
	}

	if (!ZooListLayer_GetLayerInfoFromListByIndex(ListLayer, 1, &pLayerInfoHead))
	{
		ZooLogV(" GetLayerInfoFromListByIndex index 1 Error \n");
	}
	else
	{
		ZooLogV(" GetLayerInfoFromListByIndex index 1 OK \n");
	}

	if (!ZooListLayer_GetLayerInfoFromListByIDAndCreate(ListLayer, &mode51, &pLayerInfoHead))
	{
		ZooLogV(" GetLayerInfoFromListByIDWithCreate mode51 Error \n");
	}
	else
	{
		ZooLogV(" GetLayerInfoFromListByIDWithCreate mode51 OK \n");
	}

	if (!ZooListLayer_GetLayerInfoFromListByIDAndCreate(ListLayer, &mode2, &pLayerInfoHead))
	{
		ZooLogV(" GetLayerInfoFromListByIDWithCreate mode2 Error \n");
	}
	else
	{
		ZooLogV(" GetLayerInfoFromListByIDWithCreate mode2 OK \n");
	}

	if (!ZooListLayer_GetLayerInfoFromListByIDAndCreate(ListLayer, &mode51_2, &pLayerInfoHead))
	{
		ZooLogV(" GetLayerInfoFromListByIDWithCreate mode51_2 Error \n");
	}
	else
	{
		ZooLogV(" GetLayerInfoFromListByIDWithCreate mode51_2 OK \n");
	}

	if (!ZooListLayer_GetLayerInfoFromListByIDAndCreate(ListLayer, &mode31, &pLayerInfoHead))
	{
		ZooLogV(" GetLayerInfoFromListByIDWithCreate mode31 Error \n");
	}
	else
	{
		ZooLogV(" GetLayerInfoFromListByIDWithCreate mode31 OK \n");
	}

	if (!ZooListLayer_GetLayerInfoFromListByIndex(ListLayer, 2, &pLayerInfoHead))
	{
		ZooLogV(" GetLayerInfoFromListByIndex index 2 Error \n");
	}
	else
	{
		ZooLogV(" GetLayerInfoFromListByIndex index 2 OK \n");
	}

	ZooLogV(" 开始枚举 \n");

	for (i = 0; i < ZooListLayer_GetLayerInfoCount(ListLayer) ; i++)
	{
		if (!ZooListLayer_GetLayerInfoFromListByIndex(ListLayer, i, &pLayerInfoHead))
		{
			ZooLogV(" GetLayerInfoFromListByIndex index %d Error \n", i);
		}
		else
		{
			ZooListLayer_GetLayerInfoMode(ListLayer, pLayerInfoHead, &mode);
			ZooLogV(" GetLayerInfoFromListByIndex index %d OK , dwLayer = %d, strFrom = %s \n", i, mode.dwLayer, mode.strFrom);
		}
	}

	ZooLogV(" 枚举结束 \n");

	if (!ZooListLayer_DestoryListLayer(&ListLayer))
	{
		ZooLogV(" DestoryListLayer Error \n");
		return -1;
	}
	ZooLogV(" DestoryListLayer OK \n");


	DEBUG_printf();
	return 0;
}
