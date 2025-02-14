#pragma once
#include "Utils/SampleGenerators/CPUSampleGenerator.h"

namespace Falcor
{
    class MidpointGenerator : public CPUSampleGenerator
    {
    public:
        MidpointGenerator() = default;
        virtual ~MidpointGenerator() = default;

        virtual uint32_t getSampleCount() const override { return 16u; }

        virtual void reset(uint32_t startID = 0) override { mCurSample = 0; }

        virtual float2 next() override
        {
            static const float2 kPattern[] =
            {
                float2(-0.375, -0.375),
                float2(-0.125, -0.375),
                float2(0.125, -0.375),
                float2(0.375, -0.375),
                float2(-0.375, -0.125),
                float2(-0.125, -0.125),
                float2(0.125, -0.125),
                float2(0.375, -0.125),
                float2(-0.375, 0.125),
                float2(-0.125, 0.125),
                float2(0.125, 0.125),
                float2(0.375, 0.125),
                float2(-0.375, 0.375),
                float2(-0.125, 0.375),
                float2(0.125, 0.375),
                float2(0.375, 0.375),
            };

            // permutation based on 4x4 dither matrix
            static constexpr uint kPermute[] = {
                0, 10, 2, 8,
                5, 15, 7, 13,
                1, 11, 3, 9,
                4, 14, 6, 12
            };

            //return kPattern[(mCurSample++) % getSampleCount()];
            return kPattern[kPermute[(mCurSample++) % getSampleCount()]];
        }

        uint32_t getCurSample() const { return mCurSample % getSampleCount(); }

    protected:
        uint32_t mCurSample = 0;
    };
}
