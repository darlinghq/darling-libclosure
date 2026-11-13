// Copyright (C) 2026 Lubos Dolezel
//
// Selects whether libsystem_blocks defines the Objective-C classes of Block objects (data.m) or only the
// zero-filled class placeholders (data.c). The 64-bit slices define the classes, so that retaining and releasing a
// Block through the Objective-C runtime works in a process that does not load CoreFoundation. The 32-bit slice
// uses the legacy Objective-C ABI, which has no non-lazy class attribute: there CoreFoundation creates the classes
// when it loads, as before.
#ifndef BLOCKS_OBJC_H
#define BLOCKS_OBJC_H

#ifndef HAVE_OBJC
#if defined(__LP64__)
#define HAVE_OBJC 1
#else
#define HAVE_OBJC 0
#endif
#endif

#endif
