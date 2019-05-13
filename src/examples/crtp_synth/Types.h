#ifndef DAP_EXAMPLES_CRTP_SYNTH_TYPES_H
#define DAP_EXAMPLES_CRTP_SYNTH_TYPES_H

#include "crtp/nodes/Processor.h"

#include "dsp/CombFilter.h"
#include "dsp/LadderFilter.h"
#include "dsp/Mixer.h"
#include "dsp/NoiseGenerator.h"
#include "dsp/Oscillator.h"
#include "dsp/Phaser.h"
#include "dsp/Smoother.h"
#include "dsp/UniformDistribution.h"

namespace crtp_synth
{
    using dap::crtp::processor;

    using dap::operator""_s;
    using scalar_t            = float;
    using samplerate_t        = scalar_t;
    using max_delay_t         = std::integral_constant<size_t, 512 * 64>;
    using smoothing_samples_t = std::integral_constant<size_t, 512>;

    using portamento_t =
        decltype(processor<dap::dsp::Smoother<scalar_t>>::with_inputs<scalar_t, size_t>::named(
            "value"_s,
            "duration"_s));

    using control_t = decltype(
        processor<dap::dsp::FixedSmoother<scalar_t, smoothing_samples_t::value>>::with_inputs<
            scalar_t>::named("value"_s));

    using osc_shape_t = dap::dsp::OscillatorFunctions::Shape;
    template <typename Amp, typename Freq, typename Ph>
    using osc_t =
        decltype(processor<dap::dsp::Oscillator<scalar_t>>::
                     with_inputs<Amp, Freq, Ph, samplerate_t, osc_shape_t>::named("gain"_s,
                                                                                  "frequency"_s,
                                                                                  "phase"_s,
                                                                                  "samplerate"_s,
                                                                                  "shape"_s));
    using control_osc_t    = osc_t<control_t, control_t, control_t>;
    using mod_control_t    = decltype(control_osc_t{} + control_t{});
    using mod_portamento_t = decltype(control_osc_t{} + portamento_t{});
    using fm_t             = osc_t<control_t, mod_portamento_t, control_t>;
    using am_t             = osc_t<mod_control_t, control_t, control_t>;
    using am_fm_t          = osc_t<mod_control_t, mod_portamento_t, control_t>;
    using pm_t             = osc_t<control_t, control_t, mod_control_t>;

    using noise_gen_t = dap::dsp::NoiseGenerator<dap::dsp::UniformDistribution>;
    using noise_t     = decltype(
        processor<noise_gen_t>::with_inputs<control_t, noise_gen_t::Color>::named("gain"_s,
                                                                                  "color"_s));

    template <typename T>
    using flanger_t = decltype(
        processor<dap::dsp::FeedbackCombFilter<scalar_t, max_delay_t::value>>::
            with_inputs<T, mod_control_t, control_t>::named("signal"_s, "delay"_s, "feedback"_s));

    template <typename T>
    using phaser_t = decltype(
        processor<dap::dsp::Phaser<scalar_t>>::
            with_inputs<T, control_t, control_t, control_t, control_t, samplerate_t>::named(
                "signal"_s,
                "frequency"_s,
                "depth"_s,
                "feedback"_s,
                "wet"_s,
                "samplerate"_s));

    template <typename T>
    using filter_t =
        decltype(processor<dap::dsp::LadderFilter<scalar_t>>::
                     with_inputs<T, control_t, control_t, samplerate_t>::named("signal"_s,
                                                                               "frequency"_s,
                                                                               "resonance"_s,
                                                                               "samplerate"_s));

    template <typename T>
    using bus_t = decltype(
        processor<dap::dsp::Mixer::Bus>::with_inputs<control_t, T>::named("gain"_s, "signal"_s));

    template <typename... Ts>
    using mixer_t =
        decltype(processor<dap::dsp::Mixer>::with_inputs<bus_t<Ts>...>::prefixed_by("bus_"_s));
}

#endif // DAP_EXAMPLES_CRTP_SYNTH_SYNTH_H
