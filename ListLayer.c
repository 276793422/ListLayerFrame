//#include "stdafx.h"
#include "ListLayer.h"

//	�ڲ�Ԥ����һ�׹������ʼ���ĺ��������Ǿ�û���ù�
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
	LIST_ENTRY head;						//	�ڲ�����ͷ
	PVOID pListLayerLock;					//	�����������������ͷ

	ULONG dwCount;							//	�ڲ��б�����

	ULONG dwIndex;							//	��ǰ����ָ�����ڵ�����
	PLIST_ENTRY pCurrent;					//	��ǰ����ָ�룬�������������ѭ������LIST��ʱ�򣬸���һ��

	LIST_LAYER_MEMORY_FUNCTION lmf;			//	�ڲ���Ҫ�õ��ĺ�����
	LIST_INFO_MEMORY_FUNCTION imf;			//	�ڲ���Ҫ�õ��ĺ�����
	LIST_RULE_MEMORY_FUNCTION rmf;

	ULONG dwRuleTableBufLen;				//	����ṹ�ĳ��ȣ�Ҳ�����½������ʱ���ڲ�����ĳ���

	PVOID pListInfoPool;					//	�ڲ���Դ������
	PVOID pListInfoPoolLock;				//	���ʻ���������Ҫ����
}LIST_LAYER, *PLIST_LAYER;




typedef struct _LAYER_INFO
{
	LIST_ENTRY node;

	MODE_INFO info;

	PVOID pRuleTablePool;					//	�ڲ���Դ������
	PVOID pRuleTablePoolLock;				//	���ʻ���������Ҫ����

	ULONG dwUse;							//	���ü�������ûʹ��

	ULONG dwRuleTableBufLen;				//	����ṹ�ĳ��ȣ�Ҳ�����½������ʱ���ڲ�����ĳ���

	LIST_INFO_MEMORY_FUNCTION imf;			//	�ڲ���Ҫ�õ��ĺ�����
	LIST_RULE_MEMORY_FUNCTION rmf;

	PVOID RuleTable;						//	���Ǹ��洢�ṹ���������ȫ����������洢�ṹʹ��ʲô������
}LAYER_INFO, *PLAYER_INFO;




//////////////////////////////////////////////////////////////////////////
//	��ʼ�������������
//
BOOLEAN ZooListLayer_InitListLayer(PVOID *ppThis, ULONG dwRuleLen, PLIST_FUNCTION_LIST pLfl);

//////////////////////////////////////////////////////////////////////////
//	���ٹ������
//
BOOLEAN ZooListLayer_DestoryListLayer(PVOID *pThis);

//////////////////////////////////////////////////////////////////////////
//	��ȡ��ǰ�ڲ��б�������
//	������
//		pListLayer	�����������б��ı�ͷ
//	����ֵ��
//		�����ڲ��б�������
//
ULONG ZooListLayer_GetLayerInfoCount(PVOID pThis);

BOOLEAN ZooListLayer_GetLayerInfoModeByIndex(PVOID pThis, ULONG ulIndex, MODE_INFO* pMode);

BOOLEAN ZooListLayer_AddRuleToSpecifyRuleTable(PVOID pThis, MODE_INFO *pMod, PVOID pRule);

BOOLEAN ZooListLayer_RemoveRuleFromSpecifyRuleTable(PVOID pThis, MODE_INFO *pMod, PVOID pRule);

BOOLEAN ZooListLayer_SearchRuleFromAllRuleTable(PVOID pThis, PVOID pRule, PVOID pOut);

//////////////////////////////////////////////////////////////////////////
//	���� MODE_INFO �������л�ȡ��Ӧ����Ϣ
//	������
//		pListLayer	�����������б��ı�ͷ
//		pModInfo	��Ҫ��ȡ�Ķ�Ӧ����Ϣ
//		ppLayerInfo	������ȡ���Ĳ�ָ�룬�����ȡʧ�ܣ��˲���ֵ����
//						����˲���Ϊ�գ���Ĭ�ϲ����ش�ֵ���Ҳ�Ӱ�캯������
//	����ֵ��
//		0		��������ȫƥ���
//		>0		������ͬ���������ֲ�ͬ
//		<0		��������ͬ��û���ҵ����ʵ�
//
LONG ZooListLayer_GetLayerInfoFromListByID(PVOID pThis, MODE_INFO *pModInfo, PVOID *ppLayerInfo);

//////////////////////////////////////////////////////////////////////////
//	���� MODE_INFO �������л�ȡ��Ӧ����Ϣ
//		���û���ҵ��򴴽�������ʧ���򷵻�ʧ��
//	������
//		pListLayer	�����������б��ı�ͷ
//		pModInfo	��Ҫ��ȡ�Ķ�Ӧ����Ϣ
//		ppLayerInfo	������ȡ���Ĳ�ָ�룬�����ȡʧ�ܣ��˲���ֵ����
//	����ֵ��
//		TRUE	�����ڣ����߲����ڵ�����´����ɹ���
//		FALSE	�������ڣ����Ҵ���Ҳʧ��
//
BOOLEAN ZooListLayer_GetLayerInfoFromListByIDAndCreate(PVOID pThis, MODE_INFO *pModInfo, PVOID *ppLayerInfo);

//////////////////////////////////////////////////////////////////////////
//	���ݲ������������л�ȡ��Ӧ����Ϣ��һ��������ö�٣����ṩ��������
//	������
//		pListLayer	�����������б��ı�ͷ
//		ulIndex		��Ҫ��ȡ�Ķ�Ӧ��������������0 ��ʼ
//		ppLayerInfo	������ȡ���Ĳ�ָ�룬�����ȡʧ�ܣ��˲���ֵ����
//	����ֵ��
//		TRUE	������
//		FALSE	��������
//
BOOLEAN ZooListLayer_GetLayerInfoFromListByIndex(PVOID pThis, ULONG ulIndex, PVOID *ppLayerInfo);

//////////////////////////////////////////////////////////////////////////
//	����һ������Ϣ��������б������ҷ���
//	������
//		pListLayer	�����������б��ı�ͷ
//		pModInfo	��Ҫ�����Ķ�Ӧ����Ϣ
//		ppLayerInfo	������ȡ���Ĳ�ָ�룬�������ʧ�ܣ��˲���ֵ����
//	����ֵ��
//		TRUE	�������ɹ�
//		FALSE	������ʧ��
//
BOOLEAN ZooListLayer_CreateLayerInfoToList(PVOID pThis, MODE_INFO *pModInfo, PVOID *ppLayerInfo);

//	��ȡ��ǰָ����� Mode
BOOLEAN ZooListLayer_GetLayerInfoMode(PVOID pThis, PVOID pLayer, MODE_INFO* pMode);

//////////////////////////////////////////////////////////////////////////
//	����һ������Ϣ��������б������ҷ��أ�
//		��ǰ�����ǳ�Σ�գ��˺������ж�ԭ��νڵ��Ƿ���ڣ��������ⲿʹ��
//	������
//		pListLayer	�����������б��ı�ͷ
//		pModInfo	��Ҫ�����Ķ�Ӧ����Ϣ
//		ppLayerInfo	������ȡ���Ĳ�ָ�룬�������ʧ�ܣ��˲���ֵ����
//	����ֵ��
//		TRUE	�������ɹ�
//		FALSE	������ʧ��
//
BOOLEAN ZooListLayer_CreateLayerInfoToListNoSearch(PVOID pThis, MODE_INFO *pModInfo, PVOID *ppLayerInfo);


//////////////////////////////////////////////////////////////////////////
//	ƥ�䵱ǰ�����Ƿ��ڵ�ǰ����
//	������
//		pLayerInfo	����ǰ��ָ��
//		pRule		��ҪѰ�ҵĹ���
//		pOut		����������ݣ���ǰƥ�䵽�Ĺ��򣬹��ϲ��¼
//	����ֵ��
//		TRUE	��ƥ�䵽����Ҳ�������й���
//		FALSE	��û�����й���
//		
BOOLEAN ZooLayerInfo_SearchRuleTable(PVOID pThis, PVOID pRule, PVOID pOut);

//////////////////////////////////////////////////////////////////////////
//	��ʼ����ڵ�ṹ
//		��ǰ����ֻ�����ʼ����������ɶ������
//	������
//		pLayerInfo	��Ҫ��ʼ���Ľڵ�ָ��
//		pMode		���ڵ㵱ǰ��Ϣ
//	����ֵ��
//		TRUE	����ʼ���ɹ�
//		FALSE	����ʼ��ʧ��
//
BOOLEAN ZooLayerInfo_InitLayerInfo(PVOID pThis, MODE_INFO* pMode, ULONG dwRuleTableBufLen, PLIST_FUNCTION_LIST pFun);

//////////////////////////////////////////////////////////////////////////
//	���ٲ�ڵ�ṹ
//	������
//		pLayerInfo	��Ҫ���ٵĽڵ�ָ��
//	����ֵ��
//		TRUE	�����ٳɹ�
//		FALSE	������ʧ��
//
BOOLEAN ZooLayerInfo_DestoryLayerInfo(PVOID pThis);



//	��ȡ��ǰ��� MODE
BOOLEAN ZooLayerInfo_GetLayerInfoMode(PVOID pThis, MODE_INFO* pMode);


#ifdef __ZOO_RULE_TABLE_INSIDE_INIT__

//////////////////////////////////////////////////////////////////////////
//	��ʼ��������б�
//	������
//		ppRuleTable	�������б�ָ��
//	����ֵ��
//		TRUE	����ʼ���ɹ�
//		FALSE	����ʼ��ʧ��
//
BOOLEAN ZooRuleTable_InitRuleTable(PVOID* ppRuleTable);

//////////////////////////////////////////////////////////////////////////
//	���ٲ�����б�
//	������
//		ppRuleTable	��Ҫ���ٵĽڵ�ָ��
//	����ֵ��
//		TRUE	�����ٳɹ�
//		FALSE	������ʧ��
//
BOOLEAN ZooRuleTable_DestoryRuleTable(PVOID* ppRuleTable);

//////////////////////////////////////////////////////////////////////////
//	�Ӳ������У��жϹ����Ƿ����
//	������
//		pRuleTable	��Ҫ���ҵĹ����б�
//		pRule		��Ҫ�жϵĹ���
//		pOut		����������ݣ���ǰƥ�䵽�Ĺ��򣬹��ϲ��¼
//	����ֵ��
//		TRUE	��ƥ�䵽����Ҳ�������й���
//		FALSE	��û�����й���
//
BOOLEAN ZooRuleTable_SearchRuleTable(PVOID pRuleTable, PVOID pRule, PVOID pOut);

//////////////////////////////////////////////////////////////////////////
//	�Ӳ������У��жϹ����Ƿ����
//	������
//		pRuleTable	��Ҫ���ӵĹ����б�
//		pRule		����������
//	����ֵ��
//		TRUE	����������ɹ�
//		FALSE	����������ʧ��
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

		//	��ʼ�������б�
		pListLayer->lmf = pLfl->lmf;
		pListLayer->imf = pLfl->imf;
		pListLayer->rmf = pLfl->rmf;

		pFun = &pListLayer->lmf;

		//	��ʼ������
		__InitializeListHead(&pListLayer->head);
		//	��ʼ����������
		pListLayer->pListLayerLock = pFun->InitListLayerLock(pThis);

		//	��ʼ������Ѱ�ҵ�����
		pListLayer->dwIndex = 0;
		pListLayer->pCurrent = NULL;

		//	��ʼ����Ϣ�ڴ��
		pListLayer->pListInfoPool = pFun->InitListInfoPool(pListLayer, sizeof(LAYER_INFO));
		//	��ʼ����Ϣ�ڴ�ص���
		pListLayer->pListInfoPoolLock = pFun->InitRuleInfoPoolLock(pListLayer);

		//	��ʼ��Ŀǰ�ڲ�����
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

		//	ѭ�������ڲ�
		__LIST_FOR_EACH_ENTRY_SAFE(pLayerInfo, n, &pListLayer->head, LAYER_INFO, node)
		{
			//	����
			__RemoveEntryList(&(pLayerInfo->node));

			//	�����ڴ�
			ZooLayerInfo_DestoryLayerInfo(pLayerInfo);

			//	�ͷ��ڴ�
			pFun->FreeListInfoToPool(*pThis, pListLayer->pListInfoPool, pLayerInfo);
		}

		//	�ָ�����ͷ
		__InitializeListHead(&pListLayer->head);

		//	��ʼ���ڲ�Ԫ�ظ���
		pListLayer->dwCount = 0;

		//	�ͷŵ���
		pFun->DestroyListInfoPool(*pThis, pListLayer->pListInfoPool);

		//	�ͷŵ�����û���ô���
		pFun->DestroyRuleInfoPoolLock(*pThis, pListLayer->pListInfoPoolLock);

		//	������ʼ�����һ�������ڴ�������
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

		//	ѭ��Ѱ���ڲ�
		__LIST_FOR_EACH_ENTRY_SAFE(pLayerInfo, n, &pListLayer->head, LAYER_INFO, node)
		{
			//	����ͬ
			if (pModInfo->dwLayer == pLayerInfo->info.dwLayer)
			{
				//	����ͬ����������ͬ
				if (pListLayer->lmf.MemCmp(NULL, pModInfo, &pLayerInfo->info, sizeof(*pModInfo)) == sizeof(*pModInfo))
				{
					if (!__IsInvalidPoint(ppLayerInfo))
					{
						*ppLayerInfo = pLayerInfo;
					}
					lRet = 0;
				}
				//	����ͬ���������ֲ�ͬ
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
//	����һ������Ϣ��������б������ҷ��أ�
//		��ǰ�����ǳ�Σ�գ��˺������ж�ԭ��νڵ��Ƿ���ڣ��������ⲿʹ��
//	������
//		pListLayer	�����������б��ı�ͷ
//		pModInfo	��Ҫ�����Ķ�Ӧ����Ϣ
//		ppLayerInfo	������ȡ���Ĳ�ָ�룬�������ʧ�ܣ��˲���ֵ����
//	����ֵ��
//		TRUE	�������ɹ�
//		FALSE	������ʧ��
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

		//	ֱ�Ӵ���һ��
		pLayerInfo = pFun->AllocListInfoFromPool(pThis, pListLayer->pListInfoPool, sizeof(LAYER_INFO));
		if (pLayerInfo == NULL)
		{
			break;
		}

		lfl.lmf = pListLayer->lmf;
		lfl.imf = pListLayer->imf;
		lfl.rmf = pListLayer->rmf;
		//	��ʼ��
		if (!ZooLayerInfo_InitLayerInfo(pLayerInfo, pModInfo, pListLayer->dwRuleTableBufLen, &lfl))
		{
			break;
		}

		//	��������������ʧ��
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
				//	�ҵ��˱��Լ���ģ�������ǰ��
				if (pLayerInfoTemp->info.dwLayer > pLayerInfo->info.dwLayer)
				{
					__InsertCurrentToNodeBlink(&pLayerInfo->node, &pLayerInfoTemp->node);
					bLink = TRUE;
					break;
				}
			}
			//	û�б��Լ���ģ���ô�Լ������ġ����ӵ�β����ԭ����û����
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
			//	����ҵ���ֱ�ӷ���
			bRet = TRUE;
			break;
		}

		if (lRet > 0)
		{
			break;
		}

		if (ZooListLayer_CreateLayerInfoToListNoSearch(pListLayer, pModInfo, ppLayerInfo))
		{
			//	�����ɹ���
			bRet = TRUE;
			break;
		}
		//	����ʧ����

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
			//	ԭʼ��·�������ж�Ҫ��ѯ�ĵ�����ĸ�λ�ý���ǰ/Current/��Ȼ��Ӿ�����ĵط���ʼ�ң����Ƿ���
			if (ulIndex < pListLayer->pCurrent)
			{
				//	Ҫ�ҵĵ㣬�� pListLayer->pCurrent ǰ��
				if ((pListLayer->pCurrent - ulIndex) < (pListLayer->pCurrent / 2))
				{
					//	Ҫ�ҵĵ���� pListLayer->pCurrent ����һ��
				}
				//	����ͷ���������������ͷ��ʼ��
			}
			else if (ulIndex > pListLayer->pCurrent)
			{
				//	Ҫ�ҵĵ㣬�� pListLayer->pCurrent ����
				if ((ulIndex - pListLayer->pCurrent) < ((pListLayer->dwCount - pListLayer->pCurrent) / 2))
				{
					//	Ҫ�ҵĵ���� pListLayer->pCurrent ����һ��
				}
				//	����β����
			}

			//	�������
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
			//	������
			{
				PLIST_ENTRY ple;
				ple = pListLayer->pCurrent;
				if (ulIndex < pListLayer->dwIndex)			//	��ǰ��
				{
					for (uIndex = 0; uIndex < (pListLayer->dwIndex - ulIndex); uIndex++)
					{
						ple = ple->Blink;
					}
				}
				else if (ulIndex > pListLayer->dwIndex)		//	�����
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
			//	����ҵ���ֱ�ӷ���ʧ��
			break;
		}

		if (ZooListLayer_CreateLayerInfoToListNoSearch(pListLayer, pModInfo, ppLayerInfo))
		{
			//	�����ɹ���
			bRet = TRUE;
			break;
		}
		//	����ʧ����

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
			//	û�ҵ���ȫ��ͬ��
			break;
		}
		//	�ҵ��ˣ����ߴ�����

		//	ֱ�Ӳ���
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
			//	û�ҵ���ȫ��ͬ��
			break;
		}

		//	ɾ������
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

		//	�����Լ����ڲ�����ÿ���Ӳ㣬���Ƿ�����Ϣ
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
		//	��ѯ����
		//bRet = RuleTable_SearchRuleTable(pLayerInfo->RuleTable, pRule, pOut);
		pLayerInfo->rmf.SearchRuleInTable(pLayerInfo, pLayerInfo->RuleTable, pLayerInfo->pRuleTablePool, pLayerInfo->pRuleTablePoolLock, pRule, pOut);
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

		//	��ʼ������Ϣ
		pLayerInfo->imf.MemCpy(NULL, &pLayerInfo->info, pMode, sizeof(*pMode));

		pLayerInfo->dwRuleTableBufLen = dwRuleTableBufLen;

		//	��ʼ���ڴ��
		pLayerInfo->pRuleTablePool = pLayerInfo->imf.InitRulePool(pLayerInfo, dwRuleTableBufLen);

		//	��ʼ���ڴ�ز�������
		pLayerInfo->pRuleTablePoolLock = pLayerInfo->imf.InitRuleTablePoolLock(pLayerInfo);

		//	����ʼ���洢�ṹ
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

		//	ȡ��
		pFun->GetRuleTablePoolLockForWrite(pThis, pLayerInfo->pRuleTablePoolLock);

		//	�ͷ��ڲ��ṹ
		//RuleTable_DestoryRuleTable(&pLayerInfo->RuleTable);
		pLayerInfo->rmf.DestoryRuleTable(pLayerInfo, pLayerInfo->RuleTable, pLayerInfo->pRuleTablePool, pLayerInfo->pRuleTablePoolLock);
		pLayerInfo->RuleTable = NULL;

		//	����
		pFun->ReleaseRuleTablePoolLockForWrite(pThis, pLayerInfo->pRuleTablePoolLock);

		//	������
		pFun->DestroyRuleTablePoolLock(pThis, pLayerInfo->pRuleTablePoolLock);

		//	���ٳ�
		pFun->DestroyRulePool(pThis, pLayerInfo->pRuleTablePool);

		//	�ָ������������Ѿ������ˣ�����Ϳ��Իָ���
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