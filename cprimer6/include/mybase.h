#pragma once

#ifndef __MYBASE_H__
#define __MYBASE_H__

#define SAFE_FREE(ptr) \
    do                 \
    {                  \
        free((ptr));   \
        (ptr) = NULL;  \
    } while (0)

#endif