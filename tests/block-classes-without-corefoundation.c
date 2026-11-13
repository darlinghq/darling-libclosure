/*
 * Copyright (C) 2026 Lubos Dolezel
 *
 * Blocks are Objective-C objects: the Objective-C runtime can retain and release them and reports their class,
 * also in a process that links only libSystem and libobjc (no CoreFoundation, no Foundation). A Swift program
 * that imports only Dispatch relies on this, since the compiler retains a block with objc_retain.
 * The test must not link or load any framework.
 */
#include <Block.h>
#include <objc/objc.h>
#include <objc/runtime.h>
#include <stdio.h>
#include <string.h>

extern id objc_retain(id);
extern void objc_release(id);

static int failures;

static void check(int ok, const char *what)
{
	printf("%s: %s\n", ok ? "ok" : "FAIL", what);
	if (!ok)
		failures++;
}

static void (^globalBlock)(void) = ^{ };

int main(void)
{
	int captured = 42;
	int seen = 0;
	void (^stackBlock)(void) = ^{ seen = captured; };
	void (^heapBlock)(void) = Block_copy(stackBlock);

	check(strcmp(object_getClassName((id)heapBlock), "__NSMallocBlock__") == 0, "a copied block is a __NSMallocBlock__");
	check(strcmp(object_getClassName((id)globalBlock), "__NSGlobalBlock__") == 0, "a block without captures is a __NSGlobalBlock__");

	id retained = objc_retain((id)heapBlock);
	check(retained == (id)heapBlock, "objc_retain returns the block");
	objc_release(retained);
	objc_release(objc_retain((id)globalBlock));

	heapBlock();
	check(seen == 42, "the block still runs after retain and release");
	Block_release(heapBlock);

	printf(failures ? "FAILED\n" : "PASS\n");
	return failures != 0;
}
