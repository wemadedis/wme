#pragma once

#include "rte/GenericPool.hpp"

static void TestAllocateOneOver(int size)
{
    RTE::GenericPool<int> *pool = new RTE::GenericPool<int>(size);

    for (int i = 0; i < size + 1; i++)
    {
        int *obj = pool->CreateObject();

        *obj = i;
    }
}
static void TestAllocateAllClearAll(int size)
{
    RTE::GenericPool<int> *pool = new RTE::GenericPool<int>(size);

    int **saved = new int *[size];
    for (int i = 0; i < size; i++)
    {
        int **savedIndex = saved + i;
        *savedIndex = pool->CreateObject();
    }
    for (int i = 0; i < size; i++)
    {
        pool->FreeObject(*(saved + i));
    }
}

static void TestFillClearFill(int size)
{
    RTE::GenericPool<int> *pool = new RTE::GenericPool<int>(size);

    int **saved = new int *[size];
    for (int i = 0; i < size; i++)
    {
        int **savedIndex = saved + i;
        *savedIndex = pool->CreateObject();
    }
    for (int i = 0; i < size; i++)
    {
        pool->FreeObject(*(saved + i));
    }

    for (int i = 0; i < size; i++)
    {
        int **savedIndex = saved + i;
        *savedIndex = pool->CreateObject();
    }
}