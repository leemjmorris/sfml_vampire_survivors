#include "stdafx.h"

int main()
{
    FRAMEWORK.Init(1280, 720, "Vampire Survivors");
    FRAMEWORK.Do();
    FRAMEWORK.Release();

    return 0;
}