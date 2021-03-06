#include <stdlib.h>

#ifndef _LINUX_LIST_H
#define _LINUX_LIST_H

typedef unsigned int DWORD;

typedef struct list_head_debug {
	struct list_head_debug *next, *prev;
} list_head_debug;

#ifndef LIST_HEAD_INIT
#define LIST_HEAD_INIT(name)			{ &(name), &(name) }
#endif

static void __list_add_DEBUG_(struct list_head_debug *newN,
struct list_head_debug *prev,
struct list_head_debug *next)
{
	next->prev = newN;
	newN->next = next;
	newN->prev = prev;
	prev->next = newN;
}

static void list_add_DEBUG_(struct list_head_debug *newN, struct list_head_debug *head)
{
	__list_add_DEBUG_(newN, head, head->next);
}

static void __list_del_DEBUG_(struct list_head_debug * prev, struct list_head_debug * next)
{
	next->prev = prev;
	prev->next = next;
}

static void list_del_DEBUG_(struct list_head_debug *entry)
{
	__list_del_DEBUG_(entry->prev, entry->next);
	entry->next = (struct list_head_debug *)0x0;
	entry->prev = (struct list_head_debug *)0x0;
}

#ifndef __list_for_each
#define __list_for_each(pos, head)			for (pos = (head)->next; pos != (head); pos = pos->next)
#endif


//////////////////////////////////////////////////////////////////////////


#pragma warning(disable:4200)
typedef struct malloc_info
{
	list_head_debug list;
	char *strFile;
	char *strFun;
	DWORD dwLine;
	int number;
	char memory[0];
}malloc_info;

static struct list_head_debug g_list = LIST_HEAD_INIT(g_list);

#ifdef malloc
#undef malloc
#endif

#ifdef free
#undef free
#endif

static void *DEBUG_malloc( char* strFile , char* strFun , DWORD dwLine , int number )
{
	malloc_info * mi;
	mi = (malloc_info *)malloc( sizeof(malloc_info) + number );
	if (mi)
	{
		mi->strFile = strFile;
		mi->strFun = strFun;
		mi->dwLine = dwLine;
		mi->number = number;
		list_add_DEBUG_(&mi->list , &g_list);
	}
	else
	{
		return NULL;
	}
	return mi->memory;
}

static void DEBUG_free( void *buf )
{
	malloc_info * mi;
	list_head_debug *list;
	__list_for_each(list , &g_list)
	{
		mi = (malloc_info *)list;
		if (mi->memory == buf)
		{
			list_del_DEBUG_(&mi->list);
			break;
		}
	}
}

static void DEBUG_printf()
{
	malloc_info * mi;
	list_head_debug *list;
	int i = 0;
	__list_for_each(list , &g_list)
	{
		i++;
		mi = (malloc_info *)list;
		printf( "\nfile = %s \n\t\tfun = %s , line = %d , long = %d\n" , mi->strFile , mi->strFun , mi->dwLine , mi->number );
	}
	if ( i == 0 )
	{
		printf("\n无泄漏\n");
	}
}

#define malloc(XXXXXX)	DEBUG_malloc(__FILE__,__FUNCTION__,__LINE__,XXXXXX)
#define free(XXXXXX)	DEBUG_free(XXXXXX)

#endif
