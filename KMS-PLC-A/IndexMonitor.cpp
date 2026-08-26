
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2022 KMS
// License   http://www.apache.org/licenses/LICENSE-2.0
// Product   KMS-PLC
// File      KMS-PLC-A/IndexMonitor.cpp

#include "Component.h"

// ===== Local ===============================================================
#include "../Common/Display.h"

#include "../Common/IndexMonitor.h"

// Static function declaration
// ///////////////////////////////////////////////////////////////////////////

static void Display_Range(unsigned int aFirst, unsigned int aLast);

// Public
// ///////////////////////////////////////////////////////////////////////////

IndexMonitor::IndexMonitor() : mName(nullptr), mQuantity(0) {}

IndexMonitor::~IndexMonitor()
{
    if (nullptr != mName)
    {
        assert(0 < mQuantity);

        std::cout << mName << "\n";
        std::cout << "    Ranges :";

        unsigned int lCount = 0;
        unsigned int lFree  = 0;

        for (const auto& lRange : mRanges)
        {
            assert(lRange.first <= lRange.second);
            assert(mQuantity > lRange.second);

            lCount++;
            lFree += lRange.second - lRange.first + 1;

            if (7 >= lCount)
            {
                Display_Range(lRange.first, lRange.second);
            }
            else if (8 == lCount)
            {
                std::cout << " ...";
            }
            else if (mRanges.size() - 2 <= lCount)
            {
                Display_Range(lRange.first, lRange.second);
            }
        }

        double lPC = lFree;

        lPC /= mQuantity;
        lPC *= 100.0;

        std::cout << " (" << lCount << " ranges)\n";
        std::cout << "    Free : " << lFree << " of " << mQuantity << " -> " << lPC << " %" << std::endl;
    }
}

void IndexMonitor::Init(const char* aName, unsigned int aFirst, unsigned int aLast)
{
    assert(nullptr != aName);
    assert(aFirst <= aLast);

    mName = aName;
    mQuantity = aLast - aFirst + 1;

    AddRange(aFirst, aLast);
}

void IndexMonitor::MarkUsed(unsigned int aIndex)
{
    auto lIt = mRanges.find(aIndex);
    if (mRanges.end() == lIt)
    {
        for (auto& lRange : mRanges)
        {
            assert(lRange.first <= lRange.second);

            if (lRange.first > aIndex)
            {
                break;
            }

            if (lRange.second >= aIndex)
            {
                assert(lRange.first < lRange.second);

                auto lSecond = lRange.second;

                lRange.second = aIndex - 1;

                if (aIndex < lSecond)
                {
                    AddRange(aIndex + 1, lSecond);
                }
                return;
            }
        }

        char lNameAndIndex[NAME_LENGTH];

        sprintf_s(lNameAndIndex, "%s %u", mName, aIndex);

        Display_Warning("Index more than once", lNameAndIndex);
    }
    else
    {
        auto lSecond = lIt->second;

        mRanges.erase(aIndex);

        if (aIndex != lSecond)
        {
            AddRange(aIndex + 1, lSecond);
        }
    }
}

// Private
// //////////////////////////////////////////////////////////////////////////

void IndexMonitor::AddRange(unsigned int aFirst, unsigned int aLast)
{
    assert(aFirst <= aLast);

    auto [lIt, lRet] = mRanges.insert(RangeMap::value_type(aFirst, aLast));
    assert(lRet);
}

// Static function
// ///////////////////////////////////////////////////////////////////////////

void Display_Range(unsigned int aFirst, unsigned int aLast)
{
    assert(aFirst <= aLast);

    if (aFirst == aLast)
    {
        std::cout << " " << aFirst;
    }
    else
    {
        std::cout << " [" << aFirst << "," << aLast << "]";
    }
}
