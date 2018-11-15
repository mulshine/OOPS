/**
   In short, mpool is distributed under so called "BSD license",
   
   Copyright (c) 2009-2010 Tatsuhiko Kubo <cubicdaiya@gmail.com>
   All rights reserved.
   
   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:
   
   * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
   
   * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
   
   * Neither the name of the authors nor the names of its contributors
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.
   
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* written by C99 style */

#ifndef MPOOL_H
#define MPOOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MPOOL_POOL_SIZE   10000
#define MPOOL_ALIGN_SIZE (8)

//#define size_t unsigned long
      
/**
 * memory pool structure
 */
typedef struct mpool_pool_t {
    void                *pool;     // memory pool field
    struct mpool_pool_t *next;     // next memory pool's pointer
    struct mpool_pool_t *prev;
    size_t siz;
} mpool_pool_t;

typedef struct mpool_t {
    mpool_pool_t *head;       // memory pool's head
    mpool_pool_t *tail;       // memory pool's tail
    void         *begin;      // data for internal conduct
    size_t        usiz;       // used pool size of current pool
    size_t        msiz;       // max pool size of current pool
    mpool_pool_t *mpool;      // memory pool
} mpool_t;


void mpool_create (size_t siz, mpool_t* pool);

void *mpool_alloc(size_t siz, mpool_t* pool);
void mpool_free(void* ptr, mpool_t* pool);

size_t mpool_get_size(mpool_t* pool);
size_t mpool_get_used(mpool_t* pool);


void oops_pool_init(size_t siz);

void* oops_alloc(size_t siz);
void oops_free(void* ptr);

size_t oops_pool_get_size(void);
size_t oops_pool_get_used(void);



#endif


