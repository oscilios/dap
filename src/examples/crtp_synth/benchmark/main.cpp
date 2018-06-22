#include <benchmark/benchmark.h>
#include "../Synth.h"

static crtp_synth::Synth synth(512, 48000.0f);

static void BM_Synth(benchmark::State& state)
{
    for (auto _ : state)
    {
        synth.process();
    }
}
BENCHMARK(BM_Synth);

BENCHMARK_MAIN();
