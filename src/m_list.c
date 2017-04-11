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

#include <SDL_stdinc.h>
#include <stdlib.h>

#include "m_list.h"

static void* create_data(size_t data_size) {
	static void* data;
	size_t i;

	/* Initialize the list item data. */
	data = malloc(data_size);
	for(i = 0; i < data_size; i++)
		((Uint8*)data)[i] = 0;

	return data;
}

void init_list(list* list) {
	list_node *new_node = malloc(sizeof(list_node));
	new_node->data = create_data(list->data_size);
	new_node->next = new_node->prev = NULL;
	list->head = list->tail = list->current = new_node;
	list->count++;
}

void list_clear(list *list) {
	if(list->head == NULL && list->tail == NULL) return;

	while(list_get_first(list) != NULL)
		list_delete_current(list);

	list->count = 0;
}

SDL_bool list_delete_current(list *list) {
	list_node *new_current;

	if(list_is_empty(list)) return SDL_FALSE;

	list->count--;

	if(list->current->prev == NULL) {
		/* Current object is the first one in the list. */
		list->head = list->current->next;
	} else {
		list->current->prev->next = list->current->next;
	}

	if(list->current->next == NULL) {
		/* Current object is the last one in the list. */
		list->tail = list->current->prev;
	} else {
		list->current->next->prev = list->current->prev;
	}

	/* Bump the current pointer back. If we deleted the first entry, set it to NULL. */
	new_current = list->current->prev;

	if(list->current->data != NULL)
		free(list->current->data);
	free(list->current);

	list->current = new_current;

	return SDL_TRUE;
}

void* list_get_first(list *list) {
	if(list->head == NULL)
		return NULL;
	list->current = list->head;
	return list->current->data;
}

void* list_get_last(list *list) {
	if(list->tail == NULL)
		return NULL;
	list->current = list->tail;
	return list->current->data;
}

void* list_get_next(list *list) {
	/* If the current entry is NULL, set this to a valid value. */
	if(list->current == NULL) return list_get_first(list);
	if(list->current->next == NULL)
		return NULL;
	list->current = list->current->next;
	return list->current->data;
}

void* list_get_prev(list *list) {
	if(list->current->prev == NULL)
		return NULL;
	list->current = list->current->prev;
	return list->current->data;
}

void* list_insert_after_current(list *list) {
	list->count++;
	if(list_is_empty(list)) {
		init_list(list);
		return list->current->data;
	}

	list_node *new_node = malloc(sizeof(list_node));
	new_node->data = create_data(list->data_size);

	new_node->prev = list->current;
	if(list->current->next == NULL) {
		/* Current object is the last one in the list. */
		list->tail = new_node;
		new_node->next = NULL;
	} else {
		new_node->next = list->current->next;
		new_node->next->prev = new_node;
	}
	list->current->next = new_node;

	return new_node->data;
}

void* list_insert_before_current(list *list) {
	list->count++;
	if(list_is_empty(list)) {
		init_list(list);
		return list->current->data;
	}

	list_node *new_node = malloc(sizeof(list_node));
	new_node->data = create_data(list->data_size);

	new_node->next = list->current;
	if(list->current->prev == NULL) {
		/* Current object is the first one in the list. */
		list->head = new_node;
		new_node->prev = NULL;
	} else {
		new_node->prev = list->current->prev;
		new_node->prev->next = new_node;
	}
	list->current->prev = new_node;

	return new_node->data;
}

void* list_insert_first(list *list) {
	list->count++;
	if(list_is_empty(list)) {
		init_list(list);
		return list->current->data;
	}

	list_node *new_node = malloc(sizeof(list_node));
	new_node->data = create_data(list->data_size);

	new_node->next = list->head;
	list->head->prev = new_node;
	list->head = new_node;
	new_node->prev = NULL;

	return new_node->data;
}

void* list_insert_last(list *list) {
	list->count++;
	if(list_is_empty(list)) {
		init_list(list);
		return list->current->data;
	}

	list_node *new_node = malloc(sizeof(list_node));
	new_node->data = create_data(list->data_size);

	new_node->prev = list->tail;
	list->tail->next = new_node;
	list->tail = new_node;
	new_node->next = NULL;

	return new_node->data;
}

SDL_bool list_is_at_end(list *list) {
	return list->current->next == NULL;
}

SDL_bool list_is_at_start(list *list) {
	return list->current->prev == NULL;
}

SDL_bool list_is_empty(list *list) {
	return list->head == NULL && list->tail == NULL;
}

void list_new(list *list, size_t data_size) {
	list->data_size = data_size;
	list->count = 0;
}
