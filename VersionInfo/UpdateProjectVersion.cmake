# ============================================================================
# UpdateProjectVersion.cmake
#
# Generic pre-build script that generates version information for any project.
# Runs every build to capture: git commit counts, branch, hash,
# dirty status and timestamp.
#
# Required input variables (set before including this script):
#   PROJECT_VERSION_PREFIX   - Macro prefix, e.g. "Focal Engine" or "HabiCAT3D"
#   PROJECT_VERSION_MAJOR    - Semantic version major
#   PROJECT_VERSION_MINOR    - Semantic version minor
#   PROJECT_VERSION_PATCH    - Semantic version patch
#   PROJECT_VERSION_DIR      - Source directory (where the generated header will be placed)
#
# Optional:
#   PROJECT_VERSION_TEMPLATE_DIR - Directory containing ProjectVersion.h.in
#                                  (defaults to the directory of this script)
#
# Generates:
#   ${PROJECT_VERSION_DIR}/${PROJECT_VERSION_PREFIX}Version.h
#
# Produces version strings like:
#   On master, clean:       "1.0.0 build 231 (ed4c7ce master)"
#   On master, dirty:       "1.0.0 build 231 (ed4c7ce master, dirty)"
#   On branch, with offset: "1.0.0 build 231 (ed4c7ce dev +52 from master, dirty)"
# ============================================================================

# --- Validate required inputs ---
foreach(_var PROJECT_VERSION_PREFIX PROJECT_VERSION_MAJOR PROJECT_VERSION_MINOR PROJECT_VERSION_PATCH PROJECT_VERSION_DIR)
    if(NOT DEFINED ${_var})
        message(FATAL_ERROR "UpdateProjectVersion.cmake: ${_var} must be set before including this script.")
    endif()
endforeach()

find_package(Git QUIET)
if(GIT_FOUND)
    # --- Detect default branch name (master or main) ---
    # Try origin/master first, fall back to origin/main.
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --verify origin/master
        WORKING_DIRECTORY ${PROJECT_VERSION_DIR}
        RESULT_VARIABLE _MASTER_CHECK_RC
        OUTPUT_QUIET
        ERROR_QUIET
    )
    if(_MASTER_CHECK_RC EQUAL 0)
        set(_DEFAULT_BRANCH "origin/master")
    else()
        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --verify origin/main
            WORKING_DIRECTORY ${PROJECT_VERSION_DIR}
            RESULT_VARIABLE _MAIN_CHECK_RC
            OUTPUT_QUIET
            ERROR_QUIET
        )
        if(_MAIN_CHECK_RC EQUAL 0)
            set(_DEFAULT_BRANCH "origin/main")
        else()
            set(_DEFAULT_BRANCH "")
        endif()
    endif()

    # --- Commit counts ---
    # Count total commits on the default branch - this is the stable build number.
    if(NOT "${_DEFAULT_BRANCH}" STREQUAL "")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-list --count ${_DEFAULT_BRANCH}
            WORKING_DIRECTORY ${PROJECT_VERSION_DIR}
            OUTPUT_VARIABLE _MASTER_COMMIT_COUNT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
    endif()

    # Fallback: count all commits on current branch.
    if("${_MASTER_COMMIT_COUNT}" STREQUAL "")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-list --count HEAD
            WORKING_DIRECTORY ${PROJECT_VERSION_DIR}
            OUTPUT_VARIABLE _MASTER_COMMIT_COUNT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
    endif()
    if("${_MASTER_COMMIT_COUNT}" STREQUAL "")
        set(_MASTER_COMMIT_COUNT 0)
    endif()

    # Count commits ahead of the default branch on current branch.
    # Will be 0 when building on the default branch itself.
    if(NOT "${_DEFAULT_BRANCH}" STREQUAL "")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-list --count ${_DEFAULT_BRANCH}..HEAD
            WORKING_DIRECTORY ${PROJECT_VERSION_DIR}
            OUTPUT_VARIABLE _BRANCH_COMMIT_COUNT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
    endif()

    # Fallback: if no default branch found or command fails, use 0.
    if("${_BRANCH_COMMIT_COUNT}" STREQUAL "")
        set(_BRANCH_COMMIT_COUNT 0)
    endif()

    # --- Commit hash ---
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${PROJECT_VERSION_DIR}
        OUTPUT_VARIABLE _GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # --- Branch name ---
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${PROJECT_VERSION_DIR}
        OUTPUT_VARIABLE _GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # --- Dirty status ---
    execute_process(
        COMMAND ${GIT_EXECUTABLE} diff --quiet HEAD
        WORKING_DIRECTORY ${PROJECT_VERSION_DIR}
        RESULT_VARIABLE GIT_DIRTY_RC
    )
    if(GIT_DIRTY_RC EQUAL 0)
        set(_GIT_DIRTY 0)
    else()
        set(_GIT_DIRTY 1)
    endif()
else()
    # Fallback when git is not available.
    set(_MASTER_COMMIT_COUNT 0)
    set(_BRANCH_COMMIT_COUNT 0)
    set(_GIT_HASH "unknown")
    set(_GIT_BRANCH "unknown")
    set(_GIT_DIRTY 0)
endif()

# --- Detached HEAD resolution ---
# Submodules are typically checked out in detached HEAD state.
# Try multiple strategies to resolve the actual branch name.
if("${_GIT_BRANCH}" STREQUAL "HEAD")
    # Strategy 1: Check if HEAD matches a known default branch exactly.
    # This is the most common case for submodules pinned to master/main.
    if(NOT "${_DEFAULT_BRANCH}" STREQUAL "")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
            WORKING_DIRECTORY ${PROJECT_VERSION_DIR}
            OUTPUT_VARIABLE _HEAD_SHA
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse ${_DEFAULT_BRANCH}
            WORKING_DIRECTORY ${PROJECT_VERSION_DIR}
            OUTPUT_VARIABLE _DEFAULT_BRANCH_SHA
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        if("${_HEAD_SHA}" STREQUAL "${_DEFAULT_BRANCH_SHA}")
            string(REGEX REPLACE "^origin/" "" _GIT_BRANCH "${_DEFAULT_BRANCH}")
        endif()
    endif()

    # Strategy 2: Try remote tracking branches containing this commit.
    if("${_GIT_BRANCH}" STREQUAL "HEAD")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} branch -r --contains HEAD
            WORKING_DIRECTORY ${PROJECT_VERSION_DIR}
            OUTPUT_VARIABLE _REMOTE_BRANCHES
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        if(NOT "${_REMOTE_BRANCHES}" STREQUAL "")
            # Filter out "HEAD ->" entries and take the first real branch.
            string(REGEX REPLACE "[^\n]*HEAD -> [^\n]*\n?" "" _REMOTE_BRANCHES "${_REMOTE_BRANCHES}")
            string(STRIP "${_REMOTE_BRANCHES}" _REMOTE_BRANCHES)
            if(NOT "${_REMOTE_BRANCHES}" STREQUAL "")
                string(REGEX MATCH "[^ \n]+" _GIT_BRANCH "${_REMOTE_BRANCHES}")
                string(REGEX REPLACE "^origin/" "" _GIT_BRANCH "${_GIT_BRANCH}")
            endif()
        endif()
    endif()

    # Strategy 3: Try git describe --all for tag or branch reference.
    if("${_GIT_BRANCH}" STREQUAL "HEAD")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --all --always HEAD
            WORKING_DIRECTORY ${PROJECT_VERSION_DIR}
            OUTPUT_VARIABLE _DESCRIBE_OUTPUT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        if(NOT "${_DESCRIBE_OUTPUT}" STREQUAL "")
            string(REGEX REPLACE "^(heads|remotes/origin)/" "" _GIT_BRANCH "${_DESCRIBE_OUTPUT}")
        endif()
    endif()

    # Final fallback.
    if("${_GIT_BRANCH}" STREQUAL "HEAD" OR "${_GIT_BRANCH}" STREQUAL "")
        set(_GIT_BRANCH "detached")
    endif()
endif()

# --- Build timestamp ---
string(TIMESTAMP _BUILD_TIMESTAMP \"%Y%m%d%H%M%S\")

# --- Propagate into generic template variables ---
# The generic ProjectVersion.h.in uses @PROJECT_VERSION_PREFIX@ for the macro prefix
# and @PROJECT_VERSION_PREFIX_*@ for each value. configure_file replaces these as
# flat string substitutions.
set(PROJECT_VERSION_PREFIX_VERSION_MAJOR       ${PROJECT_VERSION_MAJOR})
set(PROJECT_VERSION_PREFIX_VERSION_MINOR       ${PROJECT_VERSION_MINOR})
set(PROJECT_VERSION_PREFIX_VERSION_PATCH       ${PROJECT_VERSION_PATCH})
set(PROJECT_VERSION_PREFIX_MASTER_COMMIT_COUNT ${_MASTER_COMMIT_COUNT})
set(PROJECT_VERSION_PREFIX_BRANCH_COMMIT_COUNT ${_BRANCH_COMMIT_COUNT})
set(PROJECT_VERSION_PREFIX_GIT_HASH            ${_GIT_HASH})
set(PROJECT_VERSION_PREFIX_GIT_BRANCH          ${_GIT_BRANCH})
set(PROJECT_VERSION_PREFIX_GIT_DIRTY           ${_GIT_DIRTY})
set(PROJECT_VERSION_PREFIX_BUILD_TIMESTAMP     ${_BUILD_TIMESTAMP})

# Resolve default branch name without the "origin/" prefix.
if(NOT "${_DEFAULT_BRANCH}" STREQUAL "")
    string(REGEX REPLACE "^origin/" "" _DEFAULT_BRANCH_NAME "${_DEFAULT_BRANCH}")
else()
    set(_DEFAULT_BRANCH_NAME "master")
endif()
set(PROJECT_VERSION_PREFIX_DEFAULT_BRANCH    ${_DEFAULT_BRANCH_NAME})

# --- Determine template location ---
# Use PROJECT_VERSION_TEMPLATE_DIR if set, otherwise fall back to the
# directory containing this script (i.e. the VersionInfo folder).
if(NOT DEFINED PROJECT_VERSION_TEMPLATE_DIR)
    set(PROJECT_VERSION_TEMPLATE_DIR ${CMAKE_CURRENT_LIST_DIR})
endif()

# --- Generate header ---
# Uses the single generic template, outputs a project-specific header
# in the project's source directory.
configure_file(
    ${PROJECT_VERSION_TEMPLATE_DIR}/ProjectVersion.h.in
    ${PROJECT_VERSION_DIR}/${PROJECT_VERSION_PREFIX}Version.h
    @ONLY
)
