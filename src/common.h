s32 MaxS32(s32 A, s32 B);
s32 MinS32(s32 A, s32 B);
s32 ClampS32(s32 Min, s32 Val, s32 Max);

s32 MaxS32(s32 A, s32 B)
{
    if (A < B)
    {
        return B;
    }
    else
    {
        return A;
    }
}

s32 MinS32(s32 A, s32 B)
{
    if (A > B)
    {
        return B;
    }
    else
    {
        return A;
    }
}

s32 ClampS32(s32 Min, s32 Val, s32 Max)
{
    return MinS32(Max, MaxS32(Min, Val));
}
