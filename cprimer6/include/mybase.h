#pragma once

#ifndef __MYBASE_H__
#define __MYBASE_H__

#define SAFE_DELETE(ptr) \
    do                   \
    {                    \
        delete (ptr);    \
        (ptr) = NULL;    \
    } while (0)

#ifndef MY_GCC    
#define MY_GCC  1
#endif

#define SAFE_FREE(ptr) \
    do                 \
    {                  \
        free((ptr));   \
        (ptr) = NULL;  \
    } while (0)

#endif