#include "AudioProcess.h"
#include "audioio/AudioDeviceList.h"
#include <cassert>
#include <iostream>
#include <thread>

using namespace dap::audioio;

int main(int argc, char** argv)
{
    const auto outputDevices   = AudioDeviceList::create(dap::audioio::Scope::Output);
    const auto outputDeviceIdx = argc > 1 ? std::size_t(std::atoi(argv[1])) : 0u;

    if (outputDeviceIdx >= outputDevices.size())
    {
        std::cout << "Invalid output device index. Available output devices:" << std::endl;
        for (auto& device : outputDevices)
        {
            std::cout << device->getName() << std::endl;
        }
        return -1;
    }

    auto outputDevice = outputDevices[outputDeviceIdx].get();
    assert(outputDevice != nullptr);

    complex_fm::AudioProcess process(outputDevice->getId(),
                                     outputDevice->getChannelCount(),
                                     outputDevice->getBufferSize(),
                                     outputDevice->getSampleRate());

    auto& synth = process.getSynth();

    // Master settings
    auto freq = 196.0f; // G3
    synth.signal<0>().setFreq(freq);

    // below it's AI assisted code

    // Fundamental: More dynamic but harmonically stable
    synth.signal<0>().setCarrierLevel(0.7f);       // Strong fundamental
    synth.signal<0>().setAmplitudeModRate(0.8f);   // Moderate breathing
    synth.signal<0>().setAmplitudeModDepth(0.15f); // Dynamic but controlled
    synth.signal<0>().setVibratoRate(5.8f);        // Natural vibrato
    synth.signal<0>().setVibratoDepth(0.2f);       // Expressive vibrato
    synth.signal<0>().setModFreq(freq);            // Harmonic modulation
    synth.signal<0>().setModIdx(1.6f);             // Moderate FM for character

    // Signal 1: Octave with subtle character
    auto freq1 = freq * 2.0f + 0.7f; // Octave + gentle detuning (392.7 Hz)
    synth.signal<1>().setFreq(freq1);
    synth.signal<1>().setCarrierLevel(0.4f);       // Moderate octave presence
    synth.signal<1>().setAmplitudeModRate(1.0f);   // Different breathing rate
    synth.signal<1>().setAmplitudeModDepth(0.18f); // Dynamic variation
    synth.signal<1>().setVibratoRate(6.1f);        // Slightly different vibrato
    synth.signal<1>().setVibratoDepth(0.18f);      // Controlled vibrato variation
    synth.signal<1>().setModFreq(freq1);           // Harmonic modulation
    synth.signal<1>().setModIdx(1.2f);             // Moderate FM for texture

    // Signal 2: Third harmonic with controlled character
    auto freq2 = freq * 3.0f + 1.1f; // Third harmonic + gentle detuning (589.1 Hz)
    synth.signal<2>().setFreq(freq2);
    synth.signal<2>().setCarrierLevel(0.25f);     // Subtle high frequency presence
    synth.signal<2>().setAmplitudeModRate(1.3f);  // Moderate shimmer for brightness
    synth.signal<2>().setAmplitudeModDepth(0.2f); // Controlled timbral movement
    synth.signal<2>().setVibratoRate(6.4f);       // Different vibrato rate
    synth.signal<2>().setVibratoDepth(0.14f);     // Subtle but present vibrato
    synth.signal<2>().setModFreq(freq2);          // Harmonic modulation
    synth.signal<2>().setModIdx(0.9f);            // Gentle FM for high frequency texture

    // Configure envelope generator for bowed string character
    synth.envelope().setSampleRate(outputDevice->getSampleRate());
    synth.envelope().setAttack(2.0f);  // Very long attack for gradual bowed string build-up
    synth.envelope().setDecay(2.5f);   // Longer decay to let notes fade completely
    synth.envelope().setSustain(0.0f); // No sustain - let notes decay to silence
    synth.envelope().setRelease(0.6f); // Natural bow lift release

    // Ladder filter: warm the FM tone by rolling off harsh upper harmonics
    synth.filter().setCutoff(2200.0f); // Open enough for brightness, dark enough for warmth
    synth.filter().setResonance(0.6f); // Slight resonance for body

    // Phaser: slow sweep adds organic resonance movement, like string body
    synth.phaser().setRate(0.6f);
    synth.phaser().setDepth(0.4f);
    synth.phaser().setFeedback(0.25f);
    synth.phaser().setWet(0.3f);

    [[maybe_unused]] bool started = process.start();
    assert(started);

    // Initial envelope trigger to get it started
    synth.envelope().setGate(1.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Play a simple melody
    std::cout << "\nPlaying melody... Press q + enter to exit early..." << std::endl;

    // Define note frequencies relative to base frequency (196 Hz = G3)
    float baseFreq = 196.0f;
    auto playNote  = [&](float freqMultiplier, int durationMs)
    {
        float noteFreq = baseFreq * freqMultiplier;

        // Trigger envelope gate for new note
        synth.envelope().setGate(1.0f);

        // Update signal 0 (fundamental) with all scaled parameters
        synth.signal<0>().setFreq(noteFreq);
        synth.signal<0>().setModFreq(noteFreq);                   // Scale FM modulation
        synth.signal<0>().setVibratoRate(5.8f * freqMultiplier);  // Scale vibrato rate
        synth.signal<0>().setModIdx(1.6f / sqrt(freqMultiplier)); // Adjust FM index for frequency

        // Update signal 1 (octave) with scaled parameters
        float freq1 = noteFreq * 2.0f + 0.7f;
        synth.signal<1>().setFreq(freq1);
        synth.signal<1>().setModFreq(freq1);                      // Scale FM modulation
        synth.signal<1>().setVibratoRate(6.1f * freqMultiplier);  // Scale vibrato rate
        synth.signal<1>().setModIdx(1.2f / sqrt(freqMultiplier)); // Adjust FM index

        // Update signal 2 (third harmonic) with scaled parameters
        float freq2 = noteFreq * 3.0f + 1.1f;
        synth.signal<2>().setFreq(freq2);
        synth.signal<2>().setModFreq(freq2);                      // Scale FM modulation
        synth.signal<2>().setVibratoRate(6.4f * freqMultiplier);  // Scale vibrato rate
        synth.signal<2>().setModIdx(0.9f / sqrt(freqMultiplier)); // Adjust FM index

        // Hold note for most of the duration (longer for bowed character)
        std::this_thread::sleep_for(std::chrono::milliseconds(durationMs - 100));

        // Release envelope and allow gap between notes
        synth.envelope().setGate(0.0f);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    };

    // Simple melody with longer durations for very slow bowed string character: G - A - B - C - D -
    // E - D - C - B - A - G - D - G - B - D - G
    playNote(1.0f, 5500);   // G3
    playNote(1.125f, 5500); // A3
    playNote(1.26f, 5500);  // B3
    playNote(1.335f, 5500); // C4
    playNote(1.5f, 5500);   // D4
    playNote(1.68f, 5500);  // E4
    playNote(1.5f, 5500);   // D4
    playNote(1.335f, 5500); // C4
    playNote(1.26f, 5500);  // B3
    playNote(1.125f, 5500); // A3
    playNote(1.0f, 5500);   // G3
    playNote(1.5f, 5500);   // D4
    playNote(1.0f, 5500);   // G3
    playNote(1.26f, 5500);  // B3
    playNote(1.5f, 5500);   // D4
    playNote(1.0f, 6000);   // G3 (final note - extra long)

    std::cout << "\nMelody finished. Press q + enter to exit..." << std::endl;
    while (std::cin.get() != 'q')
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    [[maybe_unused]] const bool stopped = process.stop();
    assert(stopped);

    return 0;
}
