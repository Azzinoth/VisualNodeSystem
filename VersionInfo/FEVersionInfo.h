#ifndef FE_VERSION_INFO_H
#define FE_VERSION_INFO_H

#include <string>

// Usage:
//  #include "${Project_PREFIX}Version.h"
//  #include "FEVersionInfo.h"
//  FE_DEFINE_VERSION_INFO(Project_PREFIX)

struct FEVersionInfo
{
    int Major = 0;
    int Minor = 0;
    int Patch = 0;
    int BuildNumber = 0;
    int BranchOffset = 0;
    int Dirty = 0;
    std::string GitHash;
    std::string GitBranch;
    std::string DefaultBranch;
    std::string BuildTimestamp;

    std::string GetVersion() const
    {
        return std::to_string(Major) + "." +
               std::to_string(Minor) + "." +
               std::to_string(Patch);
    }

    std::string GetBuildInfo() const
    {
        std::string Result = "build " + std::to_string(BuildNumber) + " (" + GitHash;

        if (!GitBranch.empty())
            Result += " " + GitBranch;

        if (BranchOffset > 0)
            Result += " +" + std::to_string(BranchOffset) + " from " + DefaultBranch;

        if (Dirty)
            Result += ", dirty";

        Result += ")";
        return Result;
    }

    std::string GetFullVersionString() const
    {
        return GetVersion() + " " + GetBuildInfo();
    }
};

// Macro that reads the project-specific #defines and creates an accessor function.
// PREFIX must match the prefix used in the generated *Version.h header.
//
// Safely stringify macros that may be empty or undefined, avoiding compilation errors.
#define FE_VERSION_STRINGIFY(X) #X
#define FE_VERSION_TOSTRING(X) ("" FE_VERSION_STRINGIFY(X))

#define FE_DEFINE_VERSION_INFO(PREFIX)                                          \
    inline FEVersionInfo Get##PREFIX##VersionInfo()                             \
    {                                                                           \
        FEVersionInfo Info;                                                     \
        Info.Major          = PREFIX##_VERSION_MAJOR;                           \
        Info.Minor          = PREFIX##_VERSION_MINOR;                           \
        Info.Patch          = PREFIX##_VERSION_PATCH;                           \
        Info.BuildNumber    = PREFIX##_BUILD_NUMBER;                            \
        Info.BranchOffset   = PREFIX##_BUILD_BRANCH_OFFSET;                     \
        Info.Dirty          = PREFIX##_GIT_DIRTY;                               \
        Info.GitHash        = PREFIX##_GIT_HASH;                                \
        Info.GitBranch      = PREFIX##_GIT_BRANCH;                              \
        Info.DefaultBranch  = PREFIX##_DEFAULT_BRANCH;                          \
        Info.BuildTimestamp  = FE_VERSION_TOSTRING(PREFIX##_BUILD_TIMESTAMP);   \
        return Info;                                                            \
    }

#endif // FE_VERSION_INFO_H