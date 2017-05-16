// Author: Zoo
//	QQ��276793422
//
//	���ȣ���лLinux �ں�Դ��ɣ��м������Ǵ������ó�����
//	Ȼ�󣬸�лWindows �Ĳ��ֿ�Դ�Ĵ��룬LIST_HEAD ���Ǵ������ó�����
//
//	��;��
//		��ǰģ��Ĵ��ڣ���Ϊ��ʵ��һ������MiniFilter�ķֲ���˽ӿ�
//
//	�ӿڣ�
//		�ӿڲ�����Ҫ�ṩ3����Ҫ�ĺ���ָ���б�
//
//		�����������б����Ҫ������Ϊ��ʵ�ֿ�ƽ̨��һ��ƽ̨ȷ���������������б�ʵ��֮��������겻��
//		Ϊ�˿�ƽ̨��֧�ֶ໷������������̫�������
//			LIST_LAYER_MEMORY_FUNCTION		��ģ��ĳ�ʼ������֧��
//			LIST_INFO_MEMORY_FUNCTION		�ֲ���Ϣ�ĳ�ʼ������֧��
//
//		��������б��Ψһ��;����
//			LIST_RULE_MEMORY_FUNCTION		ÿһ��Ĺ���֧�ֺ���
//
//	�ŵ㣺
//		�ֲ��ܺ͹����б�ֿ��ˣ�
//		�ֲ�����Ҫ�ⲿ�ṩ���׺��������ڴ���ء�����ص�֧��
//		�����б�����Ҫ�ⲿ�Լ�ʵ�ֹ����б�Ĵ����������ɾ����
//		�ֲ��ܺ͹����б�ֿ��ˣ������滻�����б�ȽϷ��㣬�����ĳЩ������hash��ã����ߴ����������ú�����ã�
//			���Ը��ⲿ�ܴ�����Ŀռ�
//
//	ȱ�㣺
//		�����ҿ��ǵ���Ҫ�����ƽ̨�������Ұ��ڴ���ء�����ء�ƽ̨��ؿ��ܵĽӿڶ�������
//			�⵼�³���һ�����⣬�����ⲿ��Ҫ���Ĺ����ܶ࣬��ʼ�������ܿ����е��鷳
//			�������ȷ��ѡ��ĳһƽ̨�Ļ���ֻ��Ҫ��ʼ��һ�ξͺ��ˣ�
//		Ŀǰ�ӿڲ�����Ȼ���Ǻܾ�������Ȼ���Ż��ռ�
//		Ŀǰ�ڲ�����������Ȼ��Щ���׶����ڲ������ӿڲ�����Ȼ��Ҫ�޸�һ��
//		�ڲ�ĳЩ����ʹ�÷�����λ���п��ܲ��Ǻܺ���
//		������룬ʵ���ϴ��벿�֣���ɶȲ���ͣ���Ҫ�����⻹��UT�Ƚ���
//
//	���
//		���յ�ʵ�ֽ������������������һ���ֲ�ƥ��ģ��
//
//	GLOBAL
//	HEAD����
//	      ��
//	      ��
//	  �����ء���
//	  ��  1   ��
//	  ���������ࡪ�����С�������
//	  ��  15  �� NAME �� HEAD ������  �ұ߾��Ǵ����ŵ����������б�
//	  ���������ࡪ�����ء�������  ��  ����б���һ���ⲿ�ṩ���б�
//	  ��  23  ��                  ��
//	  ����������                  ��
//	  ��  50  ��                  ��
//	  ����������                  ��
//	  ��  70  ��                  ��
//	  ����������
//
//
//		���Ѿ������Ѵ���д�úÿ��ˣ�������Щ�ط����ǲ�̫�ÿ�
//
#pragma once

//	��������Ҫ֧�ֵ�ȫ����������
#ifndef _WINDOWS_
typedef long BOOLEAN;
typedef long LONG;
typedef unsigned long ULONG;
typedef void * PVOID;
typedef struct _LIST_ENTRY {
	struct _LIST_ENTRY *Flink;
	struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY;
typedef LONG NTSTATUS;
typedef char * PCHAR;
typedef char * ULONG_PTR;
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L) 
#define TRUE 1
#define FALSE 0
#ifndef NULL
#define NULL 0
#endif
#endif



//	ģ�������Ϣ
//	ģ��Ļ�����Ϣ����Ҫ�����ǵİ�ȫ���ı�����
//	ԭ�����ǲ����ͻ�ģ�
typedef struct _MODE_INFO
{
	//	��ǰ��ID�������ID������������ǰģ��Ĺ���ƥ���˳��ģ�IDԽС����ƥ���Խ��
	//	���ID�����ظ�������������ˣ���ô�ͻ�ȥƥ��ģ�����֣����ģ������Ҳ��ͬ��
	//	����Ϊ��ͬһģ��Ĺ���ֻ�ǲ���ͬһ�ļ�������������û����ͬһ�����·�����
	//	����ֲ���ϢID�����д���ȶ
	ULONG dwLayer;
	//	�˳�Ա��¼�˵�ǰ�������ڵ�ģ�����֣���������Ǳ���ģ�������Ψһ�ģ�
	//	���һ��IO����һ�����������ˣ����������־����Ǳ��ĸ�ģ��Ĺ��������ˣ�
	//	�������һ�����������������֣����������������ˣ���ڶ�����������Ӧ�Ĺ�����Ч
	char strFrom[8];
}MODE_INFO, *PMODE_INFO;

//	ԭ�Ӳ����ӷ�
typedef PVOID (*ZooTypeFunc_Standard_StdInterlockedAdd)(PVOID pThis, ULONG *ulNumber, ULONG ulAdd);

//	ԭ�Ӳ�������
typedef PVOID (*ZooTypeFunc_Standard_StdInterlockedSub)(PVOID pThis, ULONG *ulNumber, ULONG ulAdd);


typedef struct _LIST_STANDARD_LIB_FUNCTION 
{
	ZooTypeFunc_Standard_StdInterlockedAdd StdInterlockedAdd;
	ZooTypeFunc_Standard_StdInterlockedSub StdInterlockedSub;
}LIST_STANDARD_LIB_FUNCTION ,*PLIST_STANDARD_LIB_FUNCTION;



//	����uLen���ȣ�����һ��List Layer������ǻ����������ṩ
typedef PVOID (*ZooTypeFunc_ListLayer_AllocTrieMemory)(ULONG uLen);

//	����һ��List Layer������ǻ����������ṩ��p
typedef PVOID (*ZooTypeFunc_ListLayer_DestroyTrieMemory)(PVOID p);


//	RULE Info����صĺ�����ʼ��
typedef PVOID (*ZooTypeFunc_ListLayer_InitListInfoPool)(PVOID pThis, ULONG uLen);

typedef PVOID (*ZooTypeFunc_ListLayer_DestroyListInfoPool)(PVOID pThis, PVOID pool);

typedef PVOID (*ZooTypeFunc_ListLayer_AllocListInfoFromPool)(PVOID pThis, PVOID pool, ULONG uLen);

typedef PVOID (*ZooTypeFunc_ListLayer_FreeListInfoToPool)(PVOID pThis, PVOID pool, PVOID data);


//	��ʼ��Info�ز�����д��
typedef PVOID (*ZooTypeFunc_ListLayer_InitRuleInfoPoolLock)(PVOID pThis);

typedef PVOID (*ZooTypeFunc_ListLayer_DestroyRuleInfoPoolLock)(PVOID pThis, PVOID pLock);

typedef PVOID (*ZooTypeFunc_ListLayer_GetRuleInfoPoolLockForRead)(PVOID pThis, PVOID pLock);

typedef PVOID (*ZooTypeFunc_ListLayer_ReleaseRuleInfoPoolLockForRead)(PVOID pThis, PVOID pLock);

typedef PVOID (*ZooTypeFunc_ListLayer_GetRuleInfoPoolLockForWrite)(PVOID pThis, PVOID pLock);

typedef PVOID (*ZooTypeFunc_ListLayer_ReleaseRuleInfoPoolLockForWrite)(PVOID pThis, PVOID pLock);


//	������������
typedef PVOID (*ZooTypeFunc_ListLayer_InitListLayerLock)(PVOID pThis);

typedef PVOID (*ZooTypeFunc_ListLayer_DestroyListLayerLock)(PVOID pThis, PVOID pLock);

typedef PVOID (*ZooTypeFunc_ListLayer_GetListLayerLock)(PVOID pThis, PVOID pLock);

typedef PVOID (*ZooTypeFunc_ListLayer_ReleaseListLayerLock)(PVOID pThis, PVOID pLock);


//	�ڴ�����������
typedef PVOID (*ZooTypeFunc_ListLayer_MemorySet)(PVOID pThis, PVOID _Dst, ULONG _Val, ULONG _Size);

typedef ULONG (*ZooTypeFunc_ListLayer_MemoryCopy)(PVOID pThis, PVOID _Dst, PVOID _Src, ULONG _Size);

typedef ULONG (*ZooTypeFunc_ListLayer_MemoryCmp)(PVOID pThis, PVOID _Dst, PVOID _Src, ULONG _Size);


//	��ܹ���ӿڣ��ڲ�����ÿһ����ڴ����Ϣ
typedef struct _LIST_LAYER_MEMORY_FUNCTION
{
	ZooTypeFunc_ListLayer_AllocTrieMemory					AllocListMemory;
	ZooTypeFunc_ListLayer_DestroyTrieMemory					DestroyListMemory;

	ZooTypeFunc_ListLayer_InitListLayerLock					InitListLayerLock;
	ZooTypeFunc_ListLayer_DestroyListLayerLock				DestroyListLayerLock;
	ZooTypeFunc_ListLayer_GetListLayerLock					GetListLayerLock;
	ZooTypeFunc_ListLayer_ReleaseListLayerLock				ReleaseListLayerLock;

	ZooTypeFunc_ListLayer_InitRuleInfoPoolLock				InitRuleInfoPoolLock;
	ZooTypeFunc_ListLayer_DestroyRuleInfoPoolLock			DestroyRuleInfoPoolLock;
	ZooTypeFunc_ListLayer_GetRuleInfoPoolLockForRead		GetRuleInfoPoolLockForRead;
	ZooTypeFunc_ListLayer_ReleaseRuleInfoPoolLockForRead	ReleaseRuleInfoPoolLockForRead;
	ZooTypeFunc_ListLayer_GetRuleInfoPoolLockForWrite		GetRuleInfoPoolLockForWrite;
	ZooTypeFunc_ListLayer_ReleaseRuleInfoPoolLockForWrite	ReleaseRuleInfoPoolLockForWrite;


	ZooTypeFunc_ListLayer_InitListInfoPool					InitListInfoPool;
	ZooTypeFunc_ListLayer_DestroyListInfoPool				DestroyListInfoPool;
	ZooTypeFunc_ListLayer_AllocListInfoFromPool				AllocListInfoFromPool;
	ZooTypeFunc_ListLayer_FreeListInfoToPool				FreeListInfoToPool;

	ZooTypeFunc_ListLayer_MemorySet							MemSet;
	ZooTypeFunc_ListLayer_MemoryCopy						MemCpy;
	ZooTypeFunc_ListLayer_MemoryCmp							MemCmp;

}LIST_LAYER_MEMORY_FUNCTION, *PLIST_LAYER_MEMORY_FUNCTION;



//	��ʼ��Rule Table ����صĺ���
typedef PVOID (*ZooTypeFunc_ListLayer_InitRuleTablePool)(PVOID pThis, ULONG uLen);

typedef PVOID (*ZooTypeFunc_ListLayer_DestroyRuleTablePool)(PVOID pThis, PVOID pool);

typedef PVOID (*ZooTypeFunc_ListLayer_AllocRuleTableFromPool)(PVOID pThis, PVOID pool, ULONG uLen);

typedef PVOID (*ZooTypeFunc_ListLayer_FreeRuleTableToPool)(PVOID pThis, PVOID pool, PVOID data);


//	��ʼ��Rule Table ������صĺ���
typedef PVOID (*ZooTypeFunc_ListLayer_InitRuleTablePoolLock)(PVOID pThis);

typedef PVOID (*ZooTypeFunc_ListLayer_DestroyRuleTablePoolLock)(PVOID pThis, PVOID pLock);

typedef PVOID (*ZooTypeFunc_ListLayer_GetRuleTablePoolLockForRead)(PVOID pThis, PVOID pLock);

typedef PVOID (*ZooTypeFunc_ListLayer_ReleaseRuleTablePoolLockForRead)(PVOID pThis, PVOID pLock);

typedef PVOID (*ZooTypeFunc_ListLayer_GetRuleTablePoolLockForWrite)(PVOID pThis, PVOID pLock);

typedef PVOID (*ZooTypeFunc_ListLayer_ReleaseRuleTablePoolLockForWrite)(PVOID pThis, PVOID pLock);

//	�ڴ����ú�����memset
typedef PVOID (*ZooTypeFunc_ListInfo_MemorySet)(PVOID pThis, PVOID _Dst, ULONG _Val, ULONG _Size);

typedef ULONG (*ZooTypeFunc_ListInfo_MemoryCopy)(PVOID pThis, PVOID _Dst, PVOID _Src, ULONG _Size);




//	�����ӿڣ��ڲ�����ǰ�������
typedef struct _LIST_INFO_MEMORY_FUNCTION
{
	ZooTypeFunc_ListLayer_InitRuleTablePool					InitRulePool;
	ZooTypeFunc_ListLayer_DestroyRuleTablePool				DestroyRulePool;
	ZooTypeFunc_ListLayer_AllocRuleTableFromPool			AllocRuleFromPool;
	ZooTypeFunc_ListLayer_FreeRuleTableToPool				FreeRuleToPool;

	ZooTypeFunc_ListLayer_InitRuleTablePoolLock				InitRuleTablePoolLock;
	ZooTypeFunc_ListLayer_DestroyRuleTablePoolLock			DestroyRuleTablePoolLock;
	ZooTypeFunc_ListLayer_GetRuleTablePoolLockForRead		GetRuleTablePoolLockForRead;
	ZooTypeFunc_ListLayer_ReleaseRuleTablePoolLockForRead	ReleaseRuleTablePoolLockForRead;
	ZooTypeFunc_ListLayer_GetRuleTablePoolLockForWrite		GetRuleTablePoolLockForWrite;
	ZooTypeFunc_ListLayer_ReleaseRuleTablePoolLockForWrite	ReleaseRuleTablePoolLockForWrite;
	
	ZooTypeFunc_ListInfo_MemorySet							MemSet;
	ZooTypeFunc_ListInfo_MemoryCopy							MemCpy;

}LIST_INFO_MEMORY_FUNCTION, *PLIST_INFO_MEMORY_FUNCTION;


//	�������ṹ������HASH���ȵ�
typedef PVOID (* ZooTypeFunc_RuleTable_InitRuleTable)(PVOID pLayerInfo, PVOID pool);

//	���ٹ���ṹ
typedef PVOID (* ZooTypeFunc_RuleTable_DestoryRuleTable)(PVOID pLayerInfo, PVOID pRuleTable, PVOID pool, PVOID pPoolLock);

//	��ѯһ�������Ƿ����
typedef BOOLEAN (* ZooTypeFunc_RuleTable_SearchRuleInTable)(PVOID pLayerInfo, PVOID pRuleTable, PVOID pool, PVOID pPoolLock, PVOID pRule, PVOID pOut);

//	���һ������
typedef BOOLEAN (* ZooTypeFunc_RuleTable_AddRuleToTable)(PVOID pLayerInfo, PVOID pRuleTable, PVOID pool, PVOID pPoolLock, PVOID pRule);

//	ɾ��һ������
typedef BOOLEAN (* ZooTypeFunc_RuleTable_RemoveRuleFromTable)(PVOID pLayerInfo, PVOID pRuleTable, PVOID pool, PVOID pPoolLock, PVOID pRule);

typedef struct _LIST_RULE_MEMORY_FUNCTION 
{
	ZooTypeFunc_RuleTable_InitRuleTable			InitRuleTable;
	ZooTypeFunc_RuleTable_DestoryRuleTable		DestoryRuleTable;
	ZooTypeFunc_RuleTable_SearchRuleInTable		SearchRuleInTable;
	ZooTypeFunc_RuleTable_AddRuleToTable		AddRuleToTable;
	ZooTypeFunc_RuleTable_RemoveRuleFromTable	RemoveRuleFromTable;
}LIST_RULE_MEMORY_FUNCTION, *PLIST_RULE_MEMORY_FUNCTION;


typedef struct _LIST_FUNCTION_LIST 
{
	LIST_LAYER_MEMORY_FUNCTION lmf;
	LIST_INFO_MEMORY_FUNCTION imf;
	LIST_RULE_MEMORY_FUNCTION rmf;
	LIST_STANDARD_LIB_FUNCTION lsf;
}LIST_FUNCTION_LIST ,*PLIST_FUNCTION_LIST;


//	�ⲿ�õ���Щ�����ӿھͿ�������������

//	��ʼ�������������
BOOLEAN ZooListLayer_InitListLayer(PVOID *ppThis, ULONG dwRuleLen, PLIST_FUNCTION_LIST pLfl);

//	���ٹ������
BOOLEAN ZooListLayer_DestoryListLayer(PVOID *pThis);

//	��ȡ�����ܲ���
ULONG ZooListLayer_GetLayerInfoCount(PVOID pThis);

//	���ݲ���������ȡ��mode ��Ϣ
BOOLEAN ZooListLayer_GetLayerInfoModeByIndex(PVOID pThis, ULONG ulIndex, MODE_INFO* pMode);

//	���ָ��mode ������һ������
BOOLEAN ZooListLayer_AddRuleToSpecifyRuleTable(PVOID pThis, MODE_INFO *pMod, PVOID pRule);

//	���ָ��mode ��ɾ��һ������
BOOLEAN ZooListLayer_RemoveRuleFromSpecifyRuleTable(PVOID pThis, MODE_INFO *pMod, PVOID pRule);

//	��������������У���������������
BOOLEAN ZooListLayer_SearchRuleFromAllRuleTable(PVOID pThis, PVOID pRule, PVOID pOut);
