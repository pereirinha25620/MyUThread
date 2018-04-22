#pragma once

#include <Windows.h>

#ifndef CONTAINING_RECORD

#define CONTAINING_RECORD(addr, type, field) \
		((type *)((PCHAR)(addr) - (SIZE_T)(&((type *)0)->field)))

#endif

/* Initialize a doubly linked list. */
FORCEINLINE VOID InitializeListHead(PLIST_ENTRY ListHead) {
	ListHead->Flink = ListHead->Blink = ListHead;
}

/* If the doubly linked list is empty both Flink and Blink point to the ListHead itself. */
FORCEINLINE BOOLEAN IsListEmpty(PLIST_ENTRY ListHead) {
	return (BOOLEAN)(ListHead->Flink == ListHead->Blink && ListHead->Flink == ListHead);
}

/* Remove the entry from the doubly linked list that contains it. */
FORCEINLINE VOID RemoveEntryList(PLIST_ENTRY Entry) {
	PLIST_ENTRY FLink;
	PLIST_ENTRY BLink;

	FLink = Entry->Flink;
	BLink = Entry->Blink;

	BLink->Flink = FLink;
	FLink->Blink = BLink;

	Entry->Blink = Entry->Flink = Entry;
}


/* Remove the element at doubly linked list head. */
FORCEINLINE PLIST_ENTRY RemoveHeadList(PLIST_ENTRY ListHead) {
	PLIST_ENTRY head;

	head = ListHead->Flink;
	ListHead->Flink = head->Flink;
	ListHead->Flink->Blink = ListHead;

	head->Blink = head->Flink = head;

	return head;
}

/* Remove the element at doubly linked list tail. */
FORCEINLINE PLIST_ENTRY RemoveTailList(PLIST_ENTRY ListHead) {
	PLIST_ENTRY tail;
	tail = ListHead->Blink;

	ListHead->Blink = tail->Blink;
	tail->Blink->Flink = ListHead;

	tail->Blink = tail->Flink = tail;

	return tail;
}

/* Insert the new element at doubly linked list tail. */
FORCEINLINE VOID InsertTailList(PLIST_ENTRY ListHead, PLIST_ENTRY Entry) {
	ListHead->Blink->Flink = Entry;
	Entry->Blink = ListHead->Blink;
	ListHead->Blink = Entry;
	Entry->Flink = ListHead;
}

/* Insert the new element at doubly linked list head. */
FORCEINLINE VOID InsertHeadList(PLIST_ENTRY ListHead, PLIST_ENTRY Entry) {
	Entry->Blink = ListHead;
	Entry->Flink = ListHead->Flink;
	ListHead->Flink->Blink = Entry;
	ListHead->Flink = Entry;
}