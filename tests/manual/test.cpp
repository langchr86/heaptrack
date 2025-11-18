/*
    SPDX-FileCopyrightText: 2014-2017 Milian Wolff <mail@milianw.de>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include "util/config.h"
#include <sys/mman.h>

#if defined(_ISOC11_SOURCE)
#define HAVE_ALIGNED_ALLOC 1
#else
#define HAVE_ALIGNED_ALLOC 0
#endif

struct Foo
{
    Foo()
        : i(new int)
    {
    }
    ~Foo()
    {
        delete i;
    }
    int* i;
};

void asdf()
{
    // int* i = new int;
    // printf("i in asdf: %p\n", (void*)i);

    const auto size = 1024*1024;
    auto mem = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    printf("mmap (%u) in asdf: %p\n", size, (void*)mem);
    usleep(10000);

    const auto new_size = size * 2;
    mem = mremap(mem, size, new_size, 0);
    printf("mremap (%u) in asdf: %p\n", new_size, (void*)mem);

    // munmap(mem, size);
}

void bar()
{
    asdf();
}

void laaa()
{
    bar();
}

void split()
{
    Foo f;
    asdf();
    bar();
    laaa();
}

static Foo foo;

int main()
{
    printf("startup\n");
    // return 0;

    const auto size = 1024*1024;
    auto mem = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    printf("mem in main: %p\n", (void*)mem);
    munmap(mem, size);

    Foo* f = new Foo;
    printf("new Foo: %p\n", (void*)f);
    delete f;

    char* c = new char[1000];
    printf("new char[]: %p\n", (void*)c);
    delete[] c;

    void* buf = malloc(100);
    printf("malloc: %p\n", buf);
    buf = realloc(buf, 200);
    printf("realloc: %p\n", buf);
    free(buf);

    buf = calloc(5, 5);
    printf("calloc: %p\n", buf);
#if HAVE_CFREE
    cfree(buf);
#else
    free(buf);
#endif

#if HAVE_ALIGNED_ALLOC
    buf = aligned_alloc(16, 160);
    printf("aligned_alloc: %p\n", buf);
    free(buf);
#endif

    buf = valloc(32);
    printf("valloc: %p\n", buf);
    free(buf);

    int ret = posix_memalign(&buf, 16, 64);
    printf("posix_memalign: %d %p\n", ret, buf);
    free(buf);

    for (int i = 0; i < 10; ++i) {
        laaa();
    }
    laaa();

    split();

    return 0;
}
