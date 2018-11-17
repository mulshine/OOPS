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

//#include "../../JuceLibraryCode/JuceHeader.h"
#include "../../OOPS_JUCEPlugin/JuceLibraryCode/JuceHeader.h"

char memory[MPOOL_POOL_SIZE];
mpool_t oops_pool;

/**
 * private function
 */
static inline size_t mpool_align(size_t siz);


/**
 * create memory pool
 */
void mpool_create (size_t siz, mpool_t* pool)
{
    if (siz > MPOOL_POOL_SIZE) siz = MPOOL_POOL_SIZE;
    
    pool->mpool = (mpool_pool_t*)(void*)memory;
    
    pool->mpool->pool = (void*)memory;
    pool->mpool->siz = 0;
    pool->mpool->next = NULL;
    
    pool->begin = pool->mpool->pool;
    pool->head  = pool->mpool;
    pool->usiz  = 0;
    pool->msiz  = siz;
}

void oops_pool_init(size_t siz)
{
    mpool_create(siz, &oops_pool);
}


/**
 * extend memory pool
 */
/*
static inline bool mpool_extend(mpool_pool_t *p, size_t siz, mpool_t *pool) {
    siz = mpool_decide_create_siz(siz);
    mpool_pool_t *pp;
    
    pp = malloc(sizeof(*pp));
    if (pp == NULL) {
        return false;
    }
    
    pp->pool = malloc(siz);
    if (pp->pool == NULL) {
        return false;
    }
    
    memset(pp->pool, 0, siz);
    
    pp->next = NULL;
    
    p->next = pp;
    
    pool->begin = pp->pool;
    
    return true;
}
 */



/**
 * allocate memory from memory pool
 */
void* mpool_alloc(size_t siz, mpool_t* pool)
{
    mpool_pool_t **p = &pool->mpool;
    mpool_pool_t *pp = *p;
    size_t usiz = mpool_align(pool->usiz + siz);
    size_t msiz = pool->msiz;
    void     *d = pool->begin;
    
    if (usiz > msiz)
    {
        return NULL;
    }
    else
    {
        pool->usiz = usiz;
    
        mpool_pool_t* chunk = (mpool_pool_t*)d;
        chunk->siz = siz;
        chunk->pool = (void*)d;
        chunk->next = NULL;
        chunk->prev = NULL;
        
    
        // originally was just
        // pool->begin += mpool_align(siz);
        // but can't do void pointer arithmetic in C++ on some compilers
        // stackoverflow.com/questions/3377977/void-pointer-arithmetic
        char* ptr = (char*)pool->begin;
        ptr += mpool_align(siz);
        pool->begin = (void*)ptr;
        
        return chunk;
    }
    
    
}

void* oops_alloc(size_t siz)
{
    return mpool_alloc(siz, &oops_pool);
}

void mpool_free(void* ptr, mpool_t* pool)
{
    printf("finding %p\n", ptr);
    for (mpool_pool_t *cur = pool->head, *next; cur; cur = next)
    {
        next = cur->next;
        
        if (cur->pool == ptr)
        {
            DBG("found it");
            
            // unlink this menu item AND copy memory after this item backwards to this item
            
            // copy all memory in range [cur->next->pool, pool->begin] to cur->pool [copy mem after this item to mem where this item is]
            // set each item's pool to previous (iterate forward... item->pool = item->next->pool;
            // cur = cur->next [set this item to be next item]
            
            mpool_pool_t* prev = cur->prev;
            
            if (cur->next) // if not last item
            {
                size_t len_to_end = (size_t)((char*)pool->begin - (char*)cur->next->pool);
                
                char* write = (char*) cur->pool;
                char* read = (char*) cur->next->pool;
                
                // copy all memory in range [cur->next->pool, pool->begin] to cur->pool
                for (int i = 0; i < len_to_end; i++)
                {
                    write[i] = read[i];
                }
                
                
                // subtract siz of removed item from pool address of all items after
                for (mpool_pool_t* item = cur->next, *nextItem; nextItem; item = nextItem)
                {
                    nextItem = item->next;
                    
                    char* ptr = (char*)item->pool;
                    ptr -= cur->siz;
                    item->pool = (void*)ptr;
                }
                
                // cut this item off from chain
                prev->next = cur->next;
            }
            else
            {
                prev->next = NULL;
            }
            
            pool->usiz -= cur->siz;
            
            char* ptr = (char*)pool->begin;
            ptr -= cur->siz;
            pool->begin = (void*)ptr;
        
            break;
        }
    }
}

void oops_free(void* ptr)
{
    mpool_free(ptr, &oops_pool);
}

size_t mpool_get_size(mpool_t* pool)
{
    return pool->msiz;
}

size_t oops_pool_get_size(void)
{
    return mpool_get_size(&oops_pool);
}

size_t mpool_get_used(mpool_t* pool)
{
    return pool->usiz;
}

size_t oops_pool_get_used(void)
{
    return mpool_get_used(&oops_pool);
}

/**
 * align byte boundary
 */
static inline size_t mpool_align(size_t siz) {
    return (siz + (MPOOL_ALIGN_SIZE - 1)) & ~(MPOOL_ALIGN_SIZE - 1);
}

