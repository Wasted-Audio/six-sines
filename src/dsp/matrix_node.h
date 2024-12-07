/*
 * BaconPaul's FM Atrocity
 *
 * A mess, with FM.
 *
 * Copyright 2024, Paul Walker and Various authors, as described in the github
 * transaction log.
 *
 * This source repo is released under the MIT license, but has
 * GPL3 dependencies, as such the combined work can also be
 * released under GPL3. You know the drill.
 */

#ifndef BACONPAUL_FMTHING_DSP_MATRIX_NODE_H
#define BACONPAUL_FMTHING_DSP_MATRIX_NODE_H

#include "sst/basic-blocks/modulators/DAHDSREnvelope.h"
#include "dsp/op_source.h"
#include "dsp/sr_provider.h"

namespace baconpaul::fm
{
struct MatrixNodeFrom
{
    OpSource &onto, &from;
    SRProvider sr;
    MatrixNodeFrom(OpSource &on, OpSource &fr) : onto(on), from(fr), env(&sr) {}

    sst::basic_blocks::modulators::DAHDSREnvelope<SRProvider, blockSize> env;

    float fmLevel{0.0};

    void attack() { env.attack(0.2); }

    void applyBlock(bool gated)
    {
        env.processBlock01AD(0.2, 0.4, 0.05, 0.3, 0.3, 0.3, gated);
        for (int j = 0; j < blockSize; ++j)
        {
            onto.phaseInput[0][j] =
                (int32_t)((1 << 27) * fmLevel * env.outputCache[j] * from.output[0][j]);
            onto.phaseInput[1][j] =
                (int32_t)((1 << 27) * fmLevel * env.outputCache[j] * from.output[1][j]);
        }
    }
};

struct MatrixNodeSelf
{
    OpSource &onto;
    SRProvider sr;
    MatrixNodeSelf(OpSource &on) : onto(on), env(&sr){};
    float fbBase{1.0};

    sst::basic_blocks::modulators::DAHDSREnvelope<SRProvider, blockSize> env;

    void attack() { env.attack(0.1); }
    void applyBlock(bool gated)
    {
        env.processBlock01AD(0.1, 0.1, 0.00, 0.7, 0.2, 0.7, gated);
        for (int j = 0; j < blockSize; ++j)
        {
            onto.feedbackLevel[j] = (int32_t)((1 << 24) * env.outputCache[j] * fbBase);
        }
    }
};

struct MixerNode
{
    float output alignas(16)[2][blockSize];
    OpSource &from;
    SRProvider sr;
    MixerNode(OpSource &f) : from(f), env(&sr) { memset(output, 0, sizeof(output)); }

    float baseLevel{1.0};

    sst::basic_blocks::modulators::DAHDSREnvelope<SRProvider, blockSize> env;

    void attack() { env.attack(0.0); }

    void renderBlock(bool gated)
    {
        env.processBlock01AD(0.0, 0.6, 0.00, 0.6, 0.2, 0.7, gated);
        for (int j = 0; j < blockSize; ++j)
        {
            // use mech blah
            output[0][j] = baseLevel * env.outputCache[j] * from.output[0][j];
            output[1][j] = baseLevel * env.outputCache[j] * from.output[1][j];
        }
    }
};

struct OutputNode
{
    float output alignas(16)[2][blockSize];
    std::array<MixerNode, numOps> &fromArr;
    SRProvider sr;
    OutputNode(std::array<MixerNode, numOps> &f) : fromArr(f), env(&sr)
    {
        memset(output, 0, sizeof(output));
    }

    float baseLevel{1.0};

    sst::basic_blocks::modulators::DAHDSREnvelope<SRProvider, blockSize> env;

    void attack() { env.attack(0.0); }

    void renderBlock(bool gated)
    {
        env.processBlock01AD(0.0, 0.6, 0.00, 0.6, 0.2, 0.7, gated);
        memset(output, 0, sizeof(output));
        for (const auto &from : fromArr)
        {
            for (int j = 0; j < blockSize; ++j)
            {
                // use mech blah
                output[0][j] += baseLevel * env.outputCache[j] * from.output[0][j];
                output[1][j] += baseLevel * env.outputCache[j] * from.output[1][j];
            }
        }
    }
};
} // namespace baconpaul::fm

#endif // MATRIX_NODE_H
