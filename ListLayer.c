//#include "stdafx.h"
#include "ListLayer.h"

//	内部预留了一套规则表初始化的函数，但是就没有用过
//#define __ZOO_RULE_TABLE_INSIDE_INIT__

#define __IsInvalidPoint(_POINT_)		(((_POINT_) == NULL) || ((_POINT_) == 0))

#define __InitializeListHead(_NODE_)	(((_NODE_)->Flink = (_NODE_)->Blink = (_NODE_)), 1)

#define __RemoveEntryList(_Entry_)	\
	1;do							\
	{								\
		PLIST_ENTRY Blink;			\
		PLIST_ENTRY Flink;			\
		Flink = (_Entry_)->Flink;	\
		Blink = (_Entry_)->Blink;	\
		Blink->Flink = Flink;		\
		Flink->Blink = Blink;		\
	} while (FALSE)

#define __IsListEmpty(_ListHead_)		((_ListHead_)->Flink == (_ListHead_))

#define __InsertMidListNode(__CURRENT__, __BLINK__, __FLINK__)	\
	1;do														\
	{															\
		(__BLINK__)->Flink = (__CURRENT__);						\
		(__CURRENT__)->Blink = (__BLINK__);						\
		(__CURRENT__)->Flink = (__FLINK__);						\
		(__FLINK__)->Blink = (__CURRENT__);						\
	} while (FALSE)

#define __InsertHeadList(_ListHead_, _Entry_)	\
	1;do										\
	{											\
		PLIST_ENTRY Flink;						\
		Flink = (_ListHead_)->Flink;			\
		(_Entry_)->Flink = Flink;				\
		(_Entry_)->Blink = (_ListHead_);		\
		Flink->Blink = (_Entry_);				\
		(_ListHead_)->Flink = (_Entry_);		\
	} while (FALSE)

#define __InsertTailList(_ListHead_, _Entry_)	\
	1;do										\
	{											\
		PLIST_ENTRY Blink;						\
		Blink = (_ListHead_)->Blink;			\
		(_Entry_)->Flink = (_ListHead_);		\
		(_Entry_)->Blink = Blink;				\
		Blink->Flink = (_Entry_);				\
		(_ListHead_)->Blink = (_Entry_);		\
	} while (FALSE)


#define __InsertCurrentToNodeBlink(__CURRENT__, __NODE__)		__InsertMidListNode((__CURRENT__), ((__NODE__)->Blink), (__NODE__))

#define __InsertCurrentToNodeFlink(__CURRENT__, __NODE__)		__InsertMidListNode((__CURRENT__), (__NODE__), ((__NODE__)->Flink))

#define __LIST_FOR_EACH_SAFE(pos, n, head) \
	for (pos = (head)->Flink, n = pos->Flink; pos != (head); pos = n, n = pos->Flink)

#ifndef __CONTAINING_RECORD
#define __CONTAINING_RECORD(address, type, field) ((type *)( (PCHAR)(address) - (ULONG_PTR)(&((type *)0)->field)))
#endif

#define __LIST_FOR_EACH_ENTRY_SAFE(pos, n, head, _type, _member)				\
	for (pos = __CONTAINING_RECORD((head)->Flink, _type, _member),				\
	n = __CONTAINING_RECORD(pos->_member.Flink, _type, _member);				\
	&pos->_member != (head); 													\
	pos = n, n = __CONTAINING_RECORD(n->_member.Flink, _type, _member))



typedef struct _LIST_LAYER
{
	LIST_ENTRY head;						//	内部链表头
	PVOID pListLayerLock;					//	这个锁用来操作链表头

	ULONG dwCount;							//	内部列表个数

	ULONG dwIndex;							//	当前链表指针所在的索引
	PLIST_ENTRY pCurrent;					//	当前链表指针，配合索引可以在循环遍历LIST的时候，更快一点

	LIST_LAYER_MEMORY_FUNCTION lmf;			//	内部需要用到的函数组
	LIST_INFO_MEMORY_FUNCTION imf;			//	内部需要用到的函数组
	LIST_RULE_MEMORY_FUNCTION rmf;

	ULONG dwRuleTableBufLen;				//	规则结构的长度，也就是新建规则的时候，内部申请的长度

	PVOID pListInfoPool;					//	内部资源缓冲区
	PVOID pListInfoPoolLock;				//	访问缓冲区所需要的锁
}LIST_LAYER, *PLIST_LAYER;




typedef struct _LAYER_INFO
{
	LIST_ENTRY node;

	MODE_INFO info;

	PVOID pRuleTablePool;					//	内部资源缓冲区
	PVOID pRuleTablePoolLock;				//	访问缓冲区所需要的锁

	ULONG dwUse;							//	引用计数，还没使用

	ULONG dwRuleTableBufLen;				//	规则结构的长度，也就是新建规则的时候，内部申请的长度

	LIST_INFO_MEMORY_FUNCTION imf;			//	内部需要用到的函数组
	LIST_RULE_MEMORY_FUNCTION rmf;

	PVOID RuleTable;						//	这是个存储结构，这里放着全部规则，这个存储结构使用什么，再议
}LAYER_INFO, *PLAYER_INFO;




//////////////////////////////////////////////////////////////////////////
//	初始化整个规则层链
//
BOOLEAN ZooListLayer_InitListLayer(PVOID *ppThis, ULONG dwRuleLen, PLIST_FUNCTION_LIST pLfl);

//////////////////////////////////////////////////////////////////////////
//	销毁规则层链
//
BOOLEAN ZooListLayer_DestoryListLayer(PVOID *pThis);

//////////////////////////////////////////////////////////////////////////
//	获取当前内部列表层数量
//	参数：
//		pListLayer	：整个规则列表的表头
//	返回值：
//		返回内部列表层数量
//
ULONG ZooListLayer_GetLayerInfoCount(PVOID pThis);

BOOLEAN ZooListLayer_GetLayerInfoModeByIndex(PVOID pThis, ULONG ulIndex, MODE_INFO* pMode);

BOOLEAN ZooListLayer_AddRuleToSpecifyRuleTable(PVOID pThis, MODE_INFO *pMod, PVOID pRule);

BOOLEAN ZooListLayer_RemoveRuleFromSpecifyRuleTable(PVOID pThis, MODE_INFO *pMod, PVOID pRule);

BOOLEAN ZooListLayer_SearchRuleFromAllRuleTable(PVOID pThis, PVOID pRule, PVOID pOut);

//////////////////////////////////////////////////////////////////////////
//	根据 MODE_INFO 从链表中获取对应层信息
//	参数：
//		pListLayer	：整个规则列表的表头
//		pModInfo	：要获取的对应层信息
//		ppLayerInfo	：最终取出的层指针，如果获取失败，此参数值不变
//						如果此参数为空，则默认不返回此值，且不影响函数调用
//	返回值：
//		0		：存在完全匹配的
//		>0		：层相同，但是名字不同
//		<0		：都不相同，没有找到合适的
//
LONG ZooListLayer_GetLayerInfoFromListByID(PVOID pThis, MODE_INFO *pModInfo, PVOID *ppLayerInfo);

//////////////////////////////////////////////////////////////////////////
//	根据 MODE_INFO 从链表中获取对应层信息
//		如果没有找到则创建，创建失败则返回失败
//	参数：
//		pListLayer	：整个规则列表的表头
//		pModInfo	：要获取的对应层信息
//		ppLayerInfo	：最终取出的层指针，如果获取失败，此参数值不变
//	返回值：
//		TRUE	：存在，或者不存在的情况下创建成功了
//		FALSE	：不存在，并且创建也失败
//
BOOLEAN ZooListLayer_GetLayerInfoFromListByIDAndCreate(PVOID pThis, MODE_INFO *pModInfo, PVOID *ppLayerInfo);

//////////////////////////////////////////////////////////////////////////
//	根据层索引从链表中获取对应层信息，一般用来做枚举，不提供创建功能
//	参数：
//		pListLayer	：整个规则列表的表头
//		ulIndex		：要获取的对应层索引，索引从0 开始
//		ppLayerInfo	：最终取出的层指针，如果获取失败，此参数值不变
//	返回值：
//		TRUE	：存在
//		FALSE	：不存在
//
BOOLEAN ZooListLayer_GetLayerInfoFromListByIndex(PVOID pThis, ULONG ulIndex, PVOID *ppLayerInfo);

//////////////////////////////////////////////////////////////////////////
//	创建一个层信息，插入层列表，并且返回
//	参数：
//		pListLayer	：整个规则列表的表头
//		pModInfo	：要创建的对应层信息
//		ppLayerInfo	：最终取出的层指针，如果创建失败，此参数值不变
//	返回值：
//		TRUE	：创建成功
//		FALSE	：创建失败
//
BOOLEAN ZooListLayer_CreateLayerInfoToList(PVOID pThis, MODE_INFO *pModInfo, PVOID *ppLayerInfo);

//	获取当前指定层的 Mode
BOOLEAN ZooListLayer_GetLayerInfoMode(PVOID pThis, PVOID pLayer, MODE_INFO* pMode);

//////////////////////////////////////////////////////////////////////////
//	创建一个层信息，插入层列表，并且返回，
//		当前函数非常危险，此函数不判断原层次节点是否存在，不建议外部使用
//	参数：
//		pListLayer	：整个规则列表的表头
//		pModInfo	：要创建的对应层信息
//		ppLayerInfo	：最终取出的层指针，如果创建失败，此参数值不变
//	返回值：
//		TRUE	：创建成功
//		FALSE	：创建失败
//
BOOLEAN ZooListLayer_CreateLayerInfoToListNoSearch(PVOID pThis, MODE_INFO *pModInfo, PVOID *ppLayerInfo);


//////////////////////////////////////////////////////////////////////////
//	匹配当前规则是否在当前层中
//	参数：
//		pLayerInfo	：当前层指针
//		pRule		：要寻找的规则
//		pOut		：输出的数据，当前匹配到的规则，供上层记录
//	返回值：
//		TRUE	：匹配到规则，也就是命中规则
//		FALSE	：没有命中规则
//		
BOOLEAN ZooLayerInfo_SearchRuleTable(PVOID pThis, PVOID pRule, PVOID pOut);

//////////////////////////////////////////////////////////////////////////
//	初始化层节点结构
//		当前函数只负责初始化，其他的啥都不干
//	参数：
//		pLayerInfo	：要初始化的节点指针
//		pMode		：节点当前信息
//	返回值：
//		TRUE	：初始化成功
//		FALSE	：初始化失败
//
BOOLEAN ZooLayerInfo_InitLayerInfo(PVOID pThis, MODE_INFO* pMode, ULONG dwRuleTableBufLen, PLIST_FUNCTION_LIST pFun);

//////////////////////////////////////////////////////////////////////////
//	销毁层节点结构
//	参数：
//		pLayerInfo	：要销毁的节点指针
//	返回值：
//		TRUE	：销毁成功
//		FALSE	：销毁失败
//
BOOLEAN ZooLayerInfo_DestoryLayerInfo(PVOID pThis);



//	获取当前层的 MODE
BOOLEAN ZooLayerInfo_GetLayerInfoMode(PVOID pThis, MODE_INFO* pMode);


#ifdef __ZOO_RULE_TABLE_INSIDE_INIT__

//////////////////////////////////////////////////////////////////////////
//	初始化层规则列表
//	参数：
//		ppRuleTable	：规则列表指针
//	返回值：
//		TRUE	：初始化成功
//		FALSE	：初始化失败
//
BOOLEAN ZooRuleTable_InitRuleTable(PVOID* ppRuleTable);

//////////////////////////////////////////////////////////////////////////
//	销毁层规则列表
//	参数：
//		ppRuleTable	：要销毁的节点指针
//	返回值：
//		TRUE	：销毁成功
//		FALSE	：销毁失败
//
BOOLEAN ZooRuleTable_DestoryRuleTable(PVOID* ppRuleTable);

//////////////////////////////////////////////////////////////////////////
//	从层规则表中，判断规则，是否存在
//	参数：
//		pRuleTable	：要查找的规则列表
//		pRule		：要判断的规则
//		pOut		：输出的数据，当前匹配到的规则，供上层记录
//	返回值：
//		TRUE	：匹配到规则，也就是命中规则
//		FALSE	：没有命中规则
//
BOOLEAN ZooRuleTable_SearchRuleTable(PVOID pRuleTable, PVOID pRule, PVOID pOut);

//////////////////////////////////////////////////////////////////////////
//	从层规则表中，判断规则，是否存在
//	参数：
//		pRuleTable	：要增加的规则列表
//		pRule		：新增规则
//	返回值：
//		TRUE	：新增规则成功
//		FALSE	：新增规则失败
//
BOOLEAN ZooRuleTable_AddRuleToTable(PVOID pRuleTable, PVOID pRule);

#endif

static BOOLEAN _TestListLayerMemoryFunctionIsInvalid(PLIST_LAYER_MEMORY_FUNCTION pFun)
{
	BOOLEAN bRet = TRUE;
	do 
	{
		if (__IsInvalidPoint(pFun->AllocListMemory))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->DestroyListMemory))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->InitListInfoPool))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->DestroyListInfoPool))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->InitListLayerLock))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->DestroyListLayerLock))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->GetListLayerLock))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->ReleaseListLayerLock))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->InitRuleInfoPoolLock))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->DestroyRuleInfoPoolLock))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->GetRuleInfoPoolLockForRead))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->ReleaseRuleInfoPoolLockForRead))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->GetRuleInfoPoolLockForWrite))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->ReleaseRuleInfoPoolLockForWrite))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->AllocListInfoFromPool))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->FreeListInfoToPool))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->MemSet))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->MemCpy))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->MemCmp))
		{
			break;
		}

		bRet = FALSE;
	} while (FALSE);
	return bRet;
}


static BOOLEAN _TestListInfoMemoryFunctionIsInvalid(PLIST_INFO_MEMORY_FUNCTION pFun)
{
	BOOLEAN bRet = TRUE;

	do 
	{
		if (__IsInvalidPoint(pFun->InitRulePool))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->DestroyRulePool))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->AllocRuleFromPool))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->FreeRuleToPool))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->InitRuleTablePoolLock))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->DestroyRuleTablePoolLock))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->GetRuleTablePoolLockForRead))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->ReleaseRuleTablePoolLockForRead))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->GetRuleTablePoolLockForWrite))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->ReleaseRuleTablePoolLockForWrite))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->MemSet))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->MemCpy))
		{
			break;
		}

		bRet = FALSE;
	} while (FALSE);
	return bRet;
}

static BOOLEAN _TestRuleTableMemoryFunctionIsInvalid(PLIST_RULE_MEMORY_FUNCTION pFun)
{
	BOOLEAN bRet = TRUE;

	do 
	{
		if (__IsInvalidPoint(pFun->InitRuleTable))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->DestoryRuleTable))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->SearchRuleInTable))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->AddRuleToTable))
		{
			break;
		}
		if (__IsInvalidPoint(pFun->RemoveRuleFromTable))
		{
			break;
		}

		bRet = FALSE;
	} while (FALSE);
	return bRet;
}

BOOLEAN ZooListLayer_InitListLayer(PVOID *pThis, ULONG dwRuleTableBufLen, PLIST_FUNCTION_LIST pLfl)
{
	PLIST_LAYER pListLayer = NULL;
	PLIST_LAYER_MEMORY_FUNCTION pFun = NULL;
	BOOLEAN bRet = FALSE;
	NTSTATUS ntStatus = STATUS_SUCCESS;
	do 
	{
		if (__IsInvalidPoint(pLfl))
		{
			break;
		}
		if (dwRuleTableBufLen == 0)
		{
			break;
		}
		if (_TestListLayerMemoryFunctionIsInvalid(&pLfl->lmf))
		{
			break;
		}
		if (_TestListInfoMemoryFunctionIsInvalid(&pLfl->imf))
		{
			break;
		}

		pListLayer = pLfl->lmf.AllocListMemory(sizeof(LIST_LAYER));
		if (__IsInvalidPoint(pListLayer))
		{
			break;
		}

		//	初始化函数列表
		pListLayer->lmf = pLfl->lmf;
		pListLayer->imf = pLfl->imf;
		pListLayer->rmf = pLfl->rmf;

		pFun = &pListLayer->lmf;

		//	初始化链表
		__InitializeListHead(&pListLayer->head);
		//	初始化链表的锁
		pListLayer->pListLayerLock = pFun->InitListLayerLock(pThis);

		//	初始化快速寻找的链表
		pListLayer->dwIndex = 0;
		pListLayer->pCurrent = NULL;

		//	初始化信息内存池
		pListLayer->pListInfoPool = pFun->InitListInfoPool(pListLayer, sizeof(LAYER_INFO));
		//	初始化信息内存池的锁
		pListLayer->pListInfoPoolLock = pFun->InitRuleInfoPoolLock(pListLayer);

		//	初始化目前内部数量
		pListLayer->dwCount = 0;

		pListLayer->dwRuleTableBufLen = dwRuleTableBufLen;

		*pThis = pListLayer;
		bRet = TRUE;
	} while (FALSE);

	return bRet;
}

BOOLEAN ZooListLayer_DestoryListLayer(PVOID *pThis)
{
	PLIST_LAYER pListLayer = (PLIST_LAYER)(*pThis);
	PLIST_LAYER_MEMORY_FUNCTION pFun = NULL;
	BOOLEAN bRet = FALSE;
	NTSTATUS ntStatus = STATUS_SUCCESS;
	PLAYER_INFO pLayerInfo;
	PLAYER_INFO n;
	do
	{
		if (__IsInvalidPoint(pListLayer))
		{
			break;
		}
		pFun = &pListLayer->lmf;

		pFun->GetRuleInfoPoolLockForWrite(*pThis, pListLayer->pListInfoPoolLock);

		//	循环销毁内部
		__LIST_FOR_EACH_ENTRY_SAFE(pLayerInfo, n, &pListLayer->head, LAYER_INFO, node)
		{
			//	拆链
			__RemoveEntryList(&(pLayerInfo->node));

			//	销毁内存
			ZooLayerInfo_DestoryLayerInfo(pLayerInfo);

			//	释放内存
			pFun->FreeListInfoToPool(*pThis, pListLayer->pListInfoPool, pLayerInfo);
		}

		//	恢复链表头
		__InitializeListHead(&pListLayer->head);

		//	初始华内部元素个数
		pListLayer->dwCount = 0;

		//	释放掉池
		pFun->DestroyListInfoPool(*pThis, pListLayer->pListInfoPool);

		//	释放掉锁，没有用处了
		pFun->DestroyRuleInfoPoolLock(*pThis, pListLayer->pListInfoPoolLock);

		//	正常初始化最后一步，把内存清理了
		pFun->DestroyListMemory(*pThis);

		*pThis = NULL;
		bRet = TRUE;
	} while (FALSE);

	return bRet;
}

LONG ZooListLayer_GetLayerInfoFromListByID(PVOID pThis, MODE_INFO *pModInfo, PVOID *ppLayerInfo)
{
	PLIST_LAYER pListLayer = (PLIST_LAYER)pThis;
	LONG lRet = -1;
	PLIST_ENTRY pNode = NULL;
	PLAYER_INFO pLayerInfo = NULL;
	PLAYER_INFO n;
	do 
	{
		if (__IsInvalidPoint(pListLayer))
		{
			break;
		}
		if (__IsInvalidPoint(pModInfo))
		{
			break;
		}

		if (__IsInvalidPoint(ppLayerInfo))
		{
			break;
		}

		//	循环寻找内部
		__LIST_FOR_EACH_ENTRY_SAFE(pLayerInfo, n, &pListLayer->head, LAYER_INFO, node)
		{
			//	层相同
			if (pModInfo->dwLayer == pLayerInfo->info.dwLayer)
			{
				//	层相同并且名字相同
				if (pListLayer->lmf.MemCmp(NULL, pModInfo, &pLayerInfo->info, sizeof(*pModInfo)) == 0)
				{
					if (!__IsInvalidPoint(ppLayerInfo))
					{
						*ppLayerInfo = pLayerInfo;
					}
					lRet = 0;
				}
				//	层相同，但是名字不同
				else
				{
					lRet = 1;
				}
				break;
			}
		}

	} while (FALSE);

	return lRet;
}

//////////////////////////////////////////////////////////////////////////
//	创建一个层信息，插入层列表，并且返回，
//		当前函数非常危险，此函数不判断原层次节点是否存在，不建议外部使用
//	参数：
//		pListLayer	：整个规则列表的表头
//		pModInfo	：要创建的对应层信息
//		ppLayerInfo	：最终取出的层指针，如果创建失败，此参数值不变
//	返回值：
//		TRUE	：创建成功
//		FALSE	：创建失败
//
BOOLEAN ZooListLayer_CreateLayerInfoToListNoSearch(PVOID pThis, MODE_INFO *pModInfo, PVOID *ppLayerInfo)
{
	PLIST_LAYER pListLayer = (PLIST_LAYER)pThis;
	PLIST_LAYER_MEMORY_FUNCTION pFun = NULL;
	LIST_FUNCTION_LIST lfl = { 0 };
	BOOLEAN bRet = FALSE;
	PLAYER_INFO pLayerInfo = NULL;
	PLAYER_INFO pLayerInfoTemp = NULL;
	PLAYER_INFO n = NULL;
	do 
	{
		if (__IsInvalidPoint(pListLayer))
		{
			break;
		}
		if (__IsInvalidPoint(pModInfo))
		{
			break;
		}
		pFun = &pListLayer->lmf;

		//	直接创建一个
		pLayerInfo = pFun->AllocListInfoFromPool(pThis, pListLayer->pListInfoPool, sizeof(LAYER_INFO));
		if (pLayerInfo == NULL)
		{
			break;
		}

		lfl.lmf = pListLayer->lmf;
		lfl.imf = pListLayer->imf;
		lfl.rmf = pListLayer->rmf;
		//	初始化
		if (!ZooLayerInfo_InitLayerInfo(pLayerInfo, pModInfo, pListLayer->dwRuleTableBufLen, &lfl))
		{
			break;
		}

		//	插入链表，不会失败
		if (__IsListEmpty(&pListLayer->head))
		{
			pFun->GetListLayerLock(pThis, pListLayer->pListLayerLock);
			__InsertHeadList(&pListLayer->head, &pLayerInfo->node);
			pFun->ReleaseListLayerLock(pThis, pListLayer->pListLayerLock);
		}
		else
		{
			BOOLEAN bLink = FALSE;
			__LIST_FOR_EACH_ENTRY_SAFE(pLayerInfoTemp, n, &pListLayer->head, LAYER_INFO, node)
			{
				//	找到了比自己大的，插入它前面
				if (pLayerInfoTemp->info.dwLayer > pLayerInfo->info.dwLayer)
				{
					__InsertCurrentToNodeBlink(&pLayerInfo->node, &pLayerInfoTemp->node);
					bLink = TRUE;
					break;
				}
			}
			//	没有比自己大的，那么自己是最大的。添加到尾部，原则上没问题
			if (!bLink)
			{
				pFun->GetListLayerLock(pThis, pListLayer->pListLayerLock);
				__InsertTailList(&pListLayer->head, &pLayerInfo->node);
				pFun->ReleaseListLayerLock(pThis, pListLayer->pListLayerLock);
			}
		}

		pListLayer->dwCount ++;

		pListLayer->dwIndex = -1;
		pListLayer->pCurrent = NULL;

		*ppLayerInfo = pLayerInfo;
		bRet = TRUE;
	} while (FALSE);

	return bRet;
}

BOOLEAN ZooListLayer_GetLayerInfoFromListByIDAndCreate(PVOID pThis, MODE_INFO *pModInfo, PVOID *ppLayerInfo)
{
	PLIST_LAYER pListLayer = (PLIST_LAYER)pThis;
	BOOLEAN bRet = FALSE;
	LONG lRet = 0;
	do
	{
		if (__IsInvalidPoint(pListLayer))
		{
			break;
		}
		if (__IsInvalidPoint(pModInfo))
		{
			break;
		}

		lRet = ZooListLayer_GetLayerInfoFromListByID(pListLayer, pModInfo, ppLayerInfo);
		if (lRet == 0)
		{
			//	如果找到则直接返回
			bRet = TRUE;
			break;
		}

		if (lRet > 0)
		{
			break;
		}

		if (ZooListLayer_CreateLayerInfoToListNoSearch(pListLayer, pModInfo, ppLayerInfo))
		{
			//	创建成功了
			bRet = TRUE;
			break;
		}
		//	创建失败了

	} while (FALSE);

	return bRet;
}

BOOLEAN ZooListLayer_GetLayerInfoFromListByIndex(PVOID pThis, ULONG ulIndex, PVOID *ppLayerInfo)
{
	PLIST_LAYER pListLayer = (PLIST_LAYER)pThis;
	PLIST_LAYER_MEMORY_FUNCTION pFun = NULL;
	BOOLEAN bRet = FALSE;
	NTSTATUS ntStatus = STATUS_SUCCESS;
	ULONG uIndex = 0;
	do
	{
		if (__IsInvalidPoint(pListLayer))
		{
			break;
		}

		if (__IsInvalidPoint(ppLayerInfo))
		{
			break;
		}

		if (pListLayer->dwCount == 0)
		{
			break;
		}

		if (ulIndex >= pListLayer->dwCount)
		{
			break;
		}

		pFun = &pListLayer->lmf;

		if (pListLayer->dwIndex == -1)
		{
			pListLayer->dwIndex = 0;
			pListLayer->pCurrent = pListLayer->head.Flink;
		}

		if (ulIndex == pListLayer->dwIndex)
		{
			*ppLayerInfo = __CONTAINING_RECORD(pListLayer->pCurrent, LAYER_INFO, node);
		}
		else
		{
			pFun->GetRuleInfoPoolLockForRead(pThis, pListLayer->pListInfoPoolLock);
#if 0
			//	原始套路，这里判断要查询的点距离哪个位置近，前/Current/后，然后从距离近的地方开始找，但是放弃
			if (ulIndex < pListLayer->pCurrent)
			{
				//	要找的点，在 pListLayer->pCurrent 前面
				if ((pListLayer->pCurrent - ulIndex) < (pListLayer->pCurrent / 2))
				{
					//	要找的点距离 pListLayer->pCurrent 更近一点
				}
				//	距离头更近，常规遍历从头开始找
			}
			else if (ulIndex > pListLayer->pCurrent)
			{
				//	要找的点，在 pListLayer->pCurrent 后面
				if ((ulIndex - pListLayer->pCurrent) < ((pListLayer->dwCount - pListLayer->pCurrent) / 2))
				{
					//	要找的点距离 pListLayer->pCurrent 更近一点
				}
				//	距离尾更近
			}

			//	常规遍历
			{
				PLAYER_INFO n;
				PLAYER_INFO pLayerInfo;
				LIST_FOR_EACH_ENTRY_SAFE(pLayerInfo, n, &pListLayer->head, LAYER_INFO, node)
				{
					if (uIndex == ulIndex)
					{
						break;
					}
				}
				*ppLayerInfo = pLayerInfo;
				pListLayer->pCurrent = &(pLayerInfo->node);
				pListLayer->dwIndex = ulIndex;
			}
#else
			//	加速找
			{
				PLIST_ENTRY ple;
				ple = pListLayer->pCurrent;
				if (ulIndex < pListLayer->dwIndex)			//	向前找
				{
					for (uIndex = 0; uIndex < (pListLayer->dwIndex - ulIndex); uIndex++)
					{
						ple = ple->Blink;
					}
				}
				else if (ulIndex > pListLayer->dwIndex)		//	向后找
				{
					for (uIndex = 0; uIndex < (ulIndex - pListLayer->dwIndex); uIndex++)
					{
						ple = ple->Flink;
					}
				}
				pListLayer->pCurrent = ple;
				pListLayer->dwIndex = ulIndex;
				*ppLayerInfo = __CONTAINING_RECORD(ple, LAYER_INFO, node);
			}
#endif
			pFun->ReleaseRuleInfoPoolLockForRead(pThis, pListLayer->pListInfoPoolLock);
		}

		bRet = TRUE;
	} while (FALSE);
	return bRet;
}

BOOLEAN ZooListLayer_CreateLayerInfoToList(PVOID pThis, MODE_INFO *pModInfo, PVOID *ppLayerInfo)
{
	PLIST_LAYER pListLayer = (PLIST_LAYER)pThis;
	BOOLEAN bRet = FALSE;
	do
	{
		if (__IsInvalidPoint(pListLayer))
		{
			break;
		}
		if (__IsInvalidPoint(pModInfo))
		{
			break;
		}

		if (!ZooListLayer_GetLayerInfoFromListByID(pListLayer, pModInfo, ppLayerInfo))
		{
			//	如果找到则直接返回失败
			break;
		}

		if (ZooListLayer_CreateLayerInfoToListNoSearch(pListLayer, pModInfo, ppLayerInfo))
		{
			//	创建成功了
			bRet = TRUE;
			break;
		}
		//	创建失败了

	} while (FALSE);

	return bRet;
}

BOOLEAN ZooListLayer_AddRuleToSpecifyRuleTable(PVOID pThis, MODE_INFO *pMod, PVOID pRule)
{
	PLIST_LAYER pListLayer = (PLIST_LAYER)pThis;
	BOOLEAN bRet = FALSE;
	PLAYER_INFO pLayerInfo;
	do
	{
		if (__IsInvalidPoint(pListLayer))
		{
			break;
		}
		if (__IsInvalidPoint(pRule))
		{
			break;
		}
		if (!ZooListLayer_GetLayerInfoFromListByIDAndCreate(pListLayer, pMod, &pLayerInfo))
		{
			//	没找到完全相同的
			break;
		}
		//	找到了，或者创建了

		//	直接插入
		bRet = pLayerInfo->rmf.AddRuleToTable(pLayerInfo, pLayerInfo->RuleTable, pLayerInfo->pRuleTablePool, pLayerInfo->pRuleTablePoolLock, pRule);
	} while (FALSE);

	return bRet;
}

BOOLEAN ZooListLayer_RemoveRuleFromSpecifyRuleTable(PVOID pThis, MODE_INFO *pMod, PVOID pRule)
{
	PLIST_LAYER pListLayer = (PLIST_LAYER)pThis;
	BOOLEAN bRet = FALSE;
	PLAYER_INFO pLayerInfo;
	do
	{
		if (__IsInvalidPoint(pListLayer))
		{
			break;
		}
		if (__IsInvalidPoint(pRule))
		{
			break;
		}

		if (ZooListLayer_GetLayerInfoFromListByID(pListLayer, pMod, &pLayerInfo))
		{
			//	没找到完全相同的
			break;
		}

		//	删除规则
		bRet = pLayerInfo->rmf.RemoveRuleFromTable(pLayerInfo, pLayerInfo->RuleTable, pLayerInfo->pRuleTablePool, pLayerInfo->pRuleTablePoolLock, pRule);
	} while (FALSE);

	return bRet;
}

BOOLEAN ZooListLayer_SearchRuleFromAllRuleTable(PVOID pThis, PVOID pRule, PVOID pOut)
{
	PLIST_LAYER pListLayer = (PLIST_LAYER)pThis;
	BOOLEAN bRet = FALSE;
	PLAYER_INFO pLayerInfo;
	PLAYER_INFO n;

	do
	{
		if (__IsInvalidPoint(pListLayer))
		{
			break;
		}
		if (__IsInvalidPoint(pRule))
		{
			break;
		}

		//	遍历自己的内部，找每个子层，看是否有信息
		__LIST_FOR_EACH_ENTRY_SAFE(pLayerInfo, n, &pListLayer->head, LAYER_INFO, node)
		{
			bRet = ZooLayerInfo_SearchRuleTable(pLayerInfo, pRule, pOut);
			if (bRet == TRUE)
			{
				break;
			}
		}
		//bRet = TRUE;
	} while (FALSE);

	return bRet;
}

ULONG ZooListLayer_GetLayerInfoCount(PVOID pThis)
{
	PLIST_LAYER pListLayer = (PLIST_LAYER)pThis;
	do
	{
		if (__IsInvalidPoint(pListLayer))
		{
			break;
		}

		return pListLayer->dwCount;
	} while (FALSE);

	return 0;
}

BOOLEAN ZooListLayer_GetLayerInfoMode(PVOID pThis, PVOID pLayer, MODE_INFO* pMode)
{
	if (__IsInvalidPoint(pThis))
	{
		return FALSE;
	}
	return ZooLayerInfo_GetLayerInfoMode(pLayer, pMode);
}

BOOLEAN ZooListLayer_GetLayerInfoModeByIndex(PVOID pThis, ULONG ulIndex, MODE_INFO* pMode)
{
	PVOID pLayer = NULL;
	BOOLEAN bRet = FALSE;
	if (__IsInvalidPoint(pThis))
	{
		return FALSE;
	}
	
	do 
	{
		if (!ZooListLayer_GetLayerInfoFromListByIndex(pThis, ulIndex, &pLayer))
		{
			break;
		}

		bRet = ZooLayerInfo_GetLayerInfoMode(pLayer, pMode);;
	} while (FALSE);

	return bRet;
}







BOOLEAN ZooLayerInfo_SearchRuleTable(PVOID pThis, PVOID pRule, PVOID pOut)
{
	PLAYER_INFO pLayerInfo = (PLAYER_INFO)pThis;
	BOOLEAN bRet = FALSE;

	if (__IsInvalidPoint(pLayerInfo))
	{
		return FALSE;
	}
	if (__IsInvalidPoint(pRule))
	{
		return FALSE;
	}
	do 
	{
		//	查询规则
		//bRet = RuleTable_SearchRuleTable(pLayerInfo->RuleTable, pRule, pOut);
		bRet = pLayerInfo->rmf.SearchRuleInTable(pLayerInfo, pLayerInfo->RuleTable, pLayerInfo->pRuleTablePool, pLayerInfo->pRuleTablePoolLock, pRule, pOut);
	} while (FALSE);

	return bRet;
}

BOOLEAN ZooLayerInfo_InitLayerInfo(PVOID pThis, MODE_INFO* pMode, ULONG dwRuleTableBufLen, PLIST_FUNCTION_LIST pFun)
{
	PLAYER_INFO pLayerInfo = (PLAYER_INFO)pThis;
	BOOLEAN bRet = FALSE;
	if (__IsInvalidPoint(pLayerInfo))
	{
		return FALSE;
	}

	if (__IsInvalidPoint(pMode))
	{
		return FALSE;
	}

	if (_TestListInfoMemoryFunctionIsInvalid(&pFun->imf))
	{
		return FALSE;
	}

	if (dwRuleTableBufLen == 0)
	{
		return FALSE;
	}

	do 
	{
		pLayerInfo->imf = pFun->imf;
		pLayerInfo->rmf = pFun->rmf;

		__InitializeListHead(&pLayerInfo->node);

		//	初始化层信息
		pLayerInfo->imf.MemCpy(NULL, &pLayerInfo->info, pMode, sizeof(*pMode));

		pLayerInfo->dwRuleTableBufLen = dwRuleTableBufLen;

		//	初始化内存池
		pLayerInfo->pRuleTablePool = pLayerInfo->imf.InitRulePool(pLayerInfo, dwRuleTableBufLen);

		//	初始化内存池操作的锁
		pLayerInfo->pRuleTablePoolLock = pLayerInfo->imf.InitRuleTablePoolLock(pLayerInfo);

		//	最后初始化存储结构
		//		pInfo->RuleTable
		//RuleTable_InitRuleTable(&pLayerInfo->RuleTable);
		pLayerInfo->RuleTable = pLayerInfo->rmf.InitRuleTable(pLayerInfo, pLayerInfo->pRuleTablePool);

		bRet = TRUE;
	} while (FALSE);

	return bRet;
}

BOOLEAN ZooLayerInfo_DestoryLayerInfo(PVOID pThis)
{
	PLAYER_INFO pLayerInfo = (PLAYER_INFO)pThis;
	PLIST_INFO_MEMORY_FUNCTION pFun = NULL;
	BOOLEAN bRet = FALSE;
	if (__IsInvalidPoint(pLayerInfo))
	{
		return FALSE;
	}

	do 
	{
		pFun = &pLayerInfo->imf;

		//	取锁
		pFun->GetRuleTablePoolLockForWrite(pThis, pLayerInfo->pRuleTablePoolLock);

		//	释放内部结构
		//RuleTable_DestoryRuleTable(&pLayerInfo->RuleTable);
		pLayerInfo->rmf.DestoryRuleTable(pLayerInfo, pLayerInfo->RuleTable, pLayerInfo->pRuleTablePool, pLayerInfo->pRuleTablePoolLock);
		pLayerInfo->RuleTable = NULL;

		//	放锁
		pFun->ReleaseRuleTablePoolLockForWrite(pThis, pLayerInfo->pRuleTablePoolLock);

		//	销毁锁
		pFun->DestroyRuleTablePoolLock(pThis, pLayerInfo->pRuleTablePoolLock);

		//	销毁池
		pFun->DestroyRulePool(pThis, pLayerInfo->pRuleTablePool);

		//	恢复链表，外面已经拆链了，这里就可以恢复了
		__InitializeListHead(&pLayerInfo->node);

		bRet = TRUE;
	} while (FALSE);
	return bRet;
}

BOOLEAN ZooLayerInfo_GetLayerInfoMode(PVOID pThis, MODE_INFO* pMode)
{
	PLAYER_INFO pLayerInfo = (PLAYER_INFO)pThis;
	PLIST_INFO_MEMORY_FUNCTION pFun = NULL;
	BOOLEAN bRet = FALSE;
	if (__IsInvalidPoint(pLayerInfo))
	{
		return FALSE;
	}
	if (__IsInvalidPoint(pMode))
	{
		return FALSE;
	}
	pFun = &pLayerInfo->imf;
	do 
	{
		pFun->MemCpy(NULL, pMode, &pLayerInfo->info, sizeof(*pMode));
		bRet = TRUE;
	} while (FALSE);

	return bRet;
}








#ifdef __ZOO_RULE_TABLE_INSIDE_INIT__

BOOLEAN ZooRuleTable_InitRuleTable(PVOID* ppRuleTable)
{
	BOOLEAN bRet = FALSE;

	if (__IsInvalidPoint(ppRuleTable))
	{
		return FALSE;
	}
	do 
	{
		*ppRuleTable = NULL;
		bRet = TRUE;
	} while (FALSE);

	return bRet;
}

BOOLEAN ZooRuleTable_DestoryRuleTable(PVOID* ppRuleTable)
{
	BOOLEAN bRet = FALSE;

	if (__IsInvalidPoint(ppRuleTable))
	{
		return FALSE;
	}
	do 
	{
		*ppRuleTable = NULL;
		bRet = TRUE;
	} while (FALSE);

	return bRet;
}

BOOLEAN ZooRuleTable_SearchRuleTable(PVOID pRuleTable, PVOID pRule, PVOID pOut)
{
	BOOLEAN bRet = FALSE;

	if (__IsInvalidPoint(pRuleTable))
	{
		return FALSE;
	}
	if (__IsInvalidPoint(pRule))
	{
		return FALSE;
	}
	do 
	{
		bRet = TRUE;
	} while (FALSE);

	return bRet;
}

BOOLEAN ZooRuleTable_AddRuleToTable(PVOID pRuleTable, PVOID pRule)
{
	BOOLEAN bRet = FALSE;

	if (__IsInvalidPoint(pRuleTable))
	{
		return FALSE;
	}
	if (__IsInvalidPoint(pRule))
	{
		return FALSE;
	}
	do 
	{
		bRet = TRUE;
	} while (FALSE);

	return bRet;
}

#endif


#undef __LIST_FOR_EACH_SAFE
#undef __LIST_FOR_EACH_ENTRY_SAFE
#undef __InsertCurrentToNodeBlink
#undef __InsertCurrentToNodeFlink
#undef __InsertMidListNode
