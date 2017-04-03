//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 by Ian Burgmyer
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	Generic linked list implementation.
//
//-----------------------------------------------------------------------------

#ifndef __M_LIST__
#define __M_LIST__

#include <SDL_stdinc.h>

#define LIST_ITERATE(v, l) for(v = list_get_first(l); \
	                           !list_is_at_end(l); \
	                           v = list_get_next(l))

#define LIST_REV_ITERATE(v, l) for(v = list_get_last(l); \
	                               v != NULL; \
	                               v = list_get_prev(l))

typedef struct list_node {
	void* data;

	struct list_node *prev;
	struct list_node *next;
} list_node;

typedef struct list {
	list_node *head;
	list_node *tail;
	list_node *current;

	size_t data_size;
} list;

SDL_bool list_delete_current(list *list);
void list_clear(list *list);
void* list_get_first(list *list);
void* list_get_last(list *list);
void* list_get_next(list *list);
void* list_get_prev(list *list);
void* list_insert_after_current(list *list);
void* list_insert_before_current(list *list);
void* list_insert_first(list *list);
void* list_insert_last(list *list);
SDL_bool list_is_at_end(list *list);
SDL_bool list_is_at_start(list *list);
SDL_bool list_is_empty(list *list);
void list_new(list *list, size_t data_size);

#endif /* __M_LIST__ */
