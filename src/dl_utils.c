#include "libblastrampoline_internal.h"

void throw_dl_error(const char * path) {
    fprintf(stderr, "ERROR: Unable to load dependent library %s\n", path);
#if defined(_OS_WINDOWS_)
    LPWSTR wmsg = TEXT("");
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS |
                    FORMAT_MESSAGE_MAX_WIDTH_MASK,
                    NULL, GetLastError(),
                    MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                    (LPWSTR)&wmsg, 0, NULL);
    char err[256] = {0};
    wchar_to_utf8(wmsg, err, 255);        
#else
    const char * err = dlerror();
#endif
    fprintf(stderr, "Message: %s\n", err);
}


/*
 * Load the given `path`, using `RTLD_NOW | RTLD_LOCAL` and `RTLD_DEEPBIND`, if available
 * If `use_deepbind` is set to `0`, don't use `RTLD_DEEPBIND` even if it's available.
 */
void * load_library(const char * path) {
    void * new_handle = NULL;

#if defined(_OS_WINDOWS_)
    wchar_t wpath[2*PATH_MAX + 1] = {0};
    if (!utf8_to_wchar(path, wpath, 2*PATH_MAX)) {
        fprintf(stderr, "ERROR: Unable to convert path %s to wide string!\n", path);
        exit(1);
    }
    new_handle = (void *)LoadLibraryExW(wpath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
#else

    // If `use_deepbind` is set to `0`, we voluntarily avoid using
    // `RTLD_DEEPBIND` even if it's available.  This is primarily used
    // in conjunction with sanitizer tools, which abhor the presence of
    // deepbound libraries.
    if (use_deepbind == 0) {
        new_handle = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    }
#if defined(RTLD_DEEPBIND)
    else {
        new_handle = dlopen(path, RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
    }
#endif
#endif  // defined(_OS_WINDOWS_)

    if (new_handle == NULL) {
        throw_dl_error(path);
    }
    return new_handle;
}

/*
 * Close the given library handle
 */
void close_library(void * handle) {
#if defined(_OS_WINDOWS_)
    FreeLibrary((HMODULE)handle);
#else
    dlclose(handle);
#endif
}

/*
 * Look up the given symbol within the given library denoted by `handle`.
 */
void * lookup_symbol(const void * handle, const char * symbol_name) {
#if defined(_OS_WINDOWS_)
    return GetProcAddress((HMODULE) handle, symbol_name);
#else
    return dlsym((void *)handle, symbol_name);
#endif
}
