#pragma once

#ifdef VISUAL_NODE_SYSTEM_SHARED
    #ifdef VISUAL_NODE_SYSTEM_EXPORTS
        #define VISUAL_NODE_SYSTEM_API __declspec(dllexport)
    #else
        #define VISUAL_NODE_SYSTEM_API __declspec(dllimport)
    #endif
#else
    #define VISUAL_NODE_SYSTEM_API
#endif