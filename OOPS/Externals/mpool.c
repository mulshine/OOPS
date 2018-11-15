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

/* written with C99 style */

#include "mpool.h"

/**
 * private function
 */
static inline size_t mpool_align(size_t siz);


/**
 * create memory pool
 */
mpool_t* mpool_create (size_t siz)
{
    if (siz > MPOOL_POOL_SIZE) siz = MPOOL_POOL_SIZE;
    
    mpool_t* pool = oops_pool;
    
    pool->mpool = *memory;
    
    pool->mpool->pool = *memory;
    
    pool->mpool->next = NULL;
    
    pool->begin = pool->mpool->pool;
    pool->head  = pool->mpool;
    pool->usiz  = 0;
    pool->msiz  = siz;
    
    return pool;
}

/**
 * allocate memory from memory pool
 */
void* mpool_alloc(size_t siz)
{
    mpool_t* pool = oops_pool;
    
    mpool_pool_t **p = &pool->mpool;
    mpool_pool_t *pp = *p;
    size_t usiz = mpool_align(pool->usiz + siz);
    size_t msiz = pool->msiz;
    void     *d = pool->begin;
    
    if ((usiz + siz) > msiz)
    {
        return NULL;
    }
    else
    {
        pool->usiz = usiz;
        pool->begin += mpool_align(siz);
    }
    
    return d;
}

/**
 * release all memory pool
 */
void mpool_destroy (void)
{
    mpool_t* pool = oops_pool;
    
    for (mpool_pool_t *cur = pool->head, *next; cur; cur = next){
        next = cur->next;
    }
}

/**
 * align byte boundary
 */
static inline size_t mpool_align(size_t siz) {
    return (siz + (MPOOL_ALIGN_SIZE - 1)) & ~(MPOOL_ALIGN_SIZE - 1);
}

