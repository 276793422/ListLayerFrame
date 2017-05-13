// Author: Zoo
//	QQ：276793422
//
//	首先，感谢Linux 内核源码吧，有几个宏是从里面拿出来的
//	然后，感谢Windows 的部分开源的代码，LIST_HEAD 就是从里面拿出来的
//
//	用途：
//		当前模块的存在，是为了实现一个类似MiniFilter的分层过滤接口
//
//	接口：
//		接口部分需要提供3个主要的函数指针列表
//
//		这两个函数列表的主要功能是为了实现跨平台，一旦平台确定，这两个函数列表实现之后可以万年不改
//		为了跨平台，支持多环境，这里做了太多的事情
//			LIST_LAYER_MEMORY_FUNCTION		主模块的初始化函数支持
//			LIST_INFO_MEMORY_FUNCTION		分层信息的初始化函数支持
//
//		这个函数列表的唯一用途就是
//			LIST_RULE_MEMORY_FUNCTION		每一层的规则支持函数
//
//	优点：
//		分层框架和规则列表分开了，
//		分层框架需要外部提供两套函数来做内存相关、锁相关的支持
//		规则列表部分需要外部自己实现规则列表的搭建、规则增、删、查
//		分层框架和规则列表分开了，所以替换规则列表比较方便，比如存某些数据用hash表好，或者存其他数据用红黑树好，
//			可以给外部很大很灵活的空间
//
//	缺点：
//		由于我考虑到需要适配多平台，所以我把内存相关、锁相关、平台相关可能的接口都导出了
//			这导致出现一个问题，就是外部需要做的工作很多，初始化起来很可能有点麻烦
//			但是如果确定选择某一平台的话，只需要初始化一次就好了，
//		目前接口部分仍然不是很精练，仍然有优化空间
//		目前内部函数部分仍然有些不易读，内部函数接口部分仍然需要修改一下
//		内部某些锁的使用方法及位置有可能不是很合理，
//		这个代码，实际上代码部分，完成度不算低，主要的问题还是UT比较少
//
//	最后：
//		最终的实现结果就是类似于这样的一个分层匹配模型
//
//	GLOBAL
//	HEAD—┐
//	      ｜
//	      ｜
//	  ┌—┴—┐
//	  ｜  1   ｜
//	  ├———┼———┬———┐
//	  ｜  15  ｜ NAME ｜ HEAD ├—┬  右边就是串连着的整个规则列表，
//	  ├———┼———┴———┘  ｜  这个列表是一个外部提供的列表
//	  ｜  23  ｜                  ├
//	  ├———┤                  ｜
//	  ｜  50  ｜                  ├
//	  ├———┤                  ｜
//	  ｜  70  ｜                  └
//	  └———┘
//
//
//		我已经尽量把代码写得好看了，但是有些地方还是不太好看
//
#pragma once

//	这里是需要支持的全部变量类型
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



//	模块基本信息
//	模块的基本信息，需要向我们的安全中心备忘的
//	原则上是不许冲突的，
typedef struct _MODE_INFO
{
	//	当前层ID，这个层ID是用来决定当前模块的规则被匹配的顺序的，ID越小，被匹配得越早
	//	这个ID不许重复，如果发现它了，那么就会去匹配模块名字，如果模块名字也相同，
	//	则认为是同一模块的规则，只是不在同一文件中声明（或者没有在同一批次下发），
	//	具体分层信息ID区域有待商榷
	ULONG dwLayer;
	//	此成员记录了当前规则所在的模块名字，这个名字是必须的，而且是唯一的，
	//	如果一个IO请求被一条规则拦截了，用它来区分具体是被哪个模块的规则拦截了，
	//	不会出现一个层下面有两个名字，的情况，如果出现了，则第二个名字所对应的规则无效
	char strFrom[8];
}MODE_INFO;




//	根据uLen长度，创建一个List Layer，这个是基础，必须提供
typedef PVOID (*ZooTypeFunc_ListLayer_AllocTrieMemory)(ULONG uLen);

//	销毁一个List Layer，这个是基础，必须提供，p
typedef PVOID (*ZooTypeFunc_ListLayer_DestroyTrieMemory)(PVOID p);


//	RULE Info池相关的函数初始化
typedef PVOID (*ZooTypeFunc_ListLayer_InitListInfoPool)(PVOID pThis, ULONG uLen);

typedef PVOID (*ZooTypeFunc_ListLayer_DestroyListInfoPool)(PVOID pThis, PVOID pool);

typedef PVOID (*ZooTypeFunc_ListLayer_AllocListInfoFromPool)(PVOID pThis, PVOID pool, ULONG uLen);

typedef PVOID (*ZooTypeFunc_ListLayer_FreeListInfoToPool)(PVOID pThis, PVOID pool, PVOID data);


//	初始化Info池操作读写锁
typedef PVOID (*ZooTypeFunc_ListLayer_InitRuleInfoPoolLock)(PVOID pThis);

typedef PVOID (*ZooTypeFunc_ListLayer_DestroyRuleInfoPoolLock)(PVOID pThis, PVOID pLock);

typedef PVOID (*ZooTypeFunc_ListLayer_GetRuleInfoPoolLockForRead)(PVOID pThis, PVOID pLock);

typedef PVOID (*ZooTypeFunc_ListLayer_ReleaseRuleInfoPoolLockForRead)(PVOID pThis, PVOID pLock);

typedef PVOID (*ZooTypeFunc_ListLayer_GetRuleInfoPoolLockForWrite)(PVOID pThis, PVOID pLock);

typedef PVOID (*ZooTypeFunc_ListLayer_ReleaseRuleInfoPoolLockForWrite)(PVOID pThis, PVOID pLock);


//	链表操作锁相关
typedef PVOID (*ZooTypeFunc_ListLayer_InitListLayerLock)(PVOID pThis);

typedef PVOID (*ZooTypeFunc_ListLayer_DestroyListLayerLock)(PVOID pThis, PVOID pLock);

typedef PVOID (*ZooTypeFunc_ListLayer_GetListLayerLock)(PVOID pThis, PVOID pLock);

typedef PVOID (*ZooTypeFunc_ListLayer_ReleaseListLayerLock)(PVOID pThis, PVOID pLock);


//	内存操作函数相关
typedef PVOID (*ZooTypeFunc_ListLayer_MemorySet)(PVOID pThis, PVOID _Dst, ULONG _Val, ULONG _Size);

typedef ULONG (*ZooTypeFunc_ListLayer_MemoryCopy)(PVOID pThis, PVOID _Dst, PVOID _Src, ULONG _Size);

typedef ULONG (*ZooTypeFunc_ListLayer_MemoryCmp)(PVOID pThis, PVOID _Dst, PVOID _Src, ULONG _Size);


//	框架管理接口，内部管理每一层的内存等信息
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



//	初始化Rule Table 池相关的函数
typedef PVOID (*ZooTypeFunc_ListLayer_InitRuleTablePool)(PVOID pThis, ULONG uLen);

typedef PVOID (*ZooTypeFunc_ListLayer_DestroyRuleTablePool)(PVOID pThis, PVOID pool);

typedef PVOID (*ZooTypeFunc_ListLayer_AllocRuleTableFromPool)(PVOID pThis, PVOID pool, ULONG uLen);

typedef PVOID (*ZooTypeFunc_ListLayer_FreeRuleTableToPool)(PVOID pThis, PVOID pool, PVOID data);


//	初始化Rule Table 池锁相关的函数
typedef PVOID (*ZooTypeFunc_ListLayer_InitRuleTablePoolLock)(PVOID pThis);

typedef PVOID (*ZooTypeFunc_ListLayer_DestroyRuleTablePoolLock)(PVOID pThis, PVOID pLock);

typedef PVOID (*ZooTypeFunc_ListLayer_GetRuleTablePoolLockForRead)(PVOID pThis, PVOID pLock);

typedef PVOID (*ZooTypeFunc_ListLayer_ReleaseRuleTablePoolLockForRead)(PVOID pThis, PVOID pLock);

typedef PVOID (*ZooTypeFunc_ListLayer_GetRuleTablePoolLockForWrite)(PVOID pThis, PVOID pLock);

typedef PVOID (*ZooTypeFunc_ListLayer_ReleaseRuleTablePoolLockForWrite)(PVOID pThis, PVOID pLock);

//	内存设置函数，memset
typedef PVOID (*ZooTypeFunc_ListInfo_MemorySet)(PVOID pThis, PVOID _Dst, ULONG _Val, ULONG _Size);

typedef ULONG (*ZooTypeFunc_ListInfo_MemoryCopy)(PVOID pThis, PVOID _Dst, PVOID _Src, ULONG _Size);




//	层管理接口，内部管理当前层的数据
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


//	申请规则结构，树、HASH表，等等
typedef PVOID (* ZooTypeFunc_RuleTable_InitRuleTable)(PVOID pLayerInfo, PVOID pool);

//	销毁规则结构
typedef PVOID (* ZooTypeFunc_RuleTable_DestoryRuleTable)(PVOID pLayerInfo, PVOID pRuleTable, PVOID pool, PVOID pPoolLock);

//	查询一条规则是否存在
typedef BOOLEAN (* ZooTypeFunc_RuleTable_SearchRuleInTable)(PVOID pLayerInfo, PVOID pRuleTable, PVOID pool, PVOID pPoolLock, PVOID pRule, PVOID pOut);

//	添加一条规则
typedef BOOLEAN (* ZooTypeFunc_RuleTable_AddRuleToTable)(PVOID pLayerInfo, PVOID pRuleTable, PVOID pool, PVOID pPoolLock, PVOID pRule);

//	删除一条规则
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
}LIST_FUNCTION_LIST ,*PLIST_FUNCTION_LIST;


//	外部得到这些导出接口就可以正常工作了

//	初始化整个规则层链
BOOLEAN ZooListLayer_InitListLayer(PVOID *ppThis, ULONG dwRuleLen, PLIST_FUNCTION_LIST pLfl);

//	销毁规则层链
BOOLEAN ZooListLayer_DestoryListLayer(PVOID *pThis);

//	获取规则总层数
ULONG ZooListLayer_GetLayerInfoCount(PVOID pThis);

//	根据层索引，获取层mode 信息
BOOLEAN ZooListLayer_GetLayerInfoModeByIndex(PVOID pThis, ULONG ulIndex, MODE_INFO* pMode);

//	针对指定mode ，插入一条规则
BOOLEAN ZooListLayer_AddRuleToSpecifyRuleTable(PVOID pThis, MODE_INFO *pMod, PVOID pRule);

//	针对指定mode ，删除一条规则
BOOLEAN ZooListLayer_RemoveRuleFromSpecifyRuleTable(PVOID pThis, MODE_INFO *pMod, PVOID pRule);

//	在整个规则层链中，看命中哪条规则
BOOLEAN ZooListLayer_SearchRuleFromAllRuleTable(PVOID pThis, PVOID pRule, PVOID pOut);
