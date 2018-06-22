#ifndef DAP_BASE_SYSTEM_ANNOTATIONS_H
#define DAP_BASE_SYSTEM_ANNOTATIONS_H

// avoid warnings for unused vars
#define DAP_UNUSED(x) (void)(x);

// Some compilers warn about unused functions. When a function is sometimes
// used or not depending on build settings (e.g. a function only called from
// within "assert"), this attribute can be used to suppress such warnings.
// 
#ifndef __has_attribute // if we don't have __has_attribute, ignore it
    #define __has_attribute(x) 0
#endif

#if __has_attribute(unused)
    #define DAP_ATTRIBUTE_UNUSED __attribute__((__unused__))
#else
    #define DAP_ATTRIBUTE_UNUSED
#endif

// warn if result from function is unused
#if __has_attribute(warn_unused_result)
    #define DAP_ATTRIBUTE_WARN_UNUSED_RESULT __attribute__((__warn_unused_result__))
#elif defined(_MSC_VER)
    #define DAP_ATTRIBUTE_WARN_UNUSED_RESULT _Check_return_
#else
    #define DAP_ATTRIBUTE_WARN_UNUSED_RESULT
#endif

// warn if result is null
#if __has_attribute(returns_nonnull)
    #define DAP_ATTRIBUTE_RETURNS_NONNULL __attribute__((returns_nonnull))
#elif defined(_MSC_VER)
    #define DAP_ATTRIBUTE_RETURNS_NONNULL _Ret_notnull_
#else
    #define DAP_ATTRIBUTE_RETURNS_NONNULL
#endif

// Disable a particular sanitizer for a function
#if __has_attribute(no_sanitize)
    #define DAP_NO_SANITIZE(KIND) __attribute__((no_sanitize(KIND)))
#else
    #define DAP_NO_SANITIZE(KIND)
#endif

#if __has_attribute(nonnull)
    #define DAP_ATTRIBUTE_NON_NULL __attribute__((nonnull))
#else
    #define DAP_ATTRIBUTE_NON_NULL
#endif

// custom annotations

// helper for future annotations
#if defined(__clang__) || defined (__GNUC__)
    #define DAP_ANNOTATE__(user_string__) __attribute__ ((annotate(user_string__)))
#else
    #define DAP_ANNOTATE__(user_string)
#endif

// avoid clang-tidy warning if ctor is not marked explicit
#define DAP_NO_EXPLICIT_CTOR DAP_ANNOTATE__("no_explicit_ctor")
// avoid clang-tidy modernize warnings
#define DAP_NO_MODERNIZE DAP_ANNOTATE__("no_modernize")

#endif // DAP_BASE_SYSTEM_ANNOTATIONS_H
