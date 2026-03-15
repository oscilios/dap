#include "AudioProcess.h"
#include "audioio/AudioDeviceList.h"
#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>
#include <thread>

#if DAP_HAS_QT
#include "SynthBridge.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#endif

using namespace dap::audioio;

static void listDevices()
{
    const auto devices = AudioDeviceList::create(dap::audioio::Scope::Output);
    std::cout << "Available output devices:" << std::endl;
    for (std::size_t i = 0; i < devices.size(); ++i)
    {
        std::cout << "  " << i << ": " << devices[i]->getName() << std::endl;
    }
}

static int parseDeviceIndex(int argc, char** argv)
{
    for (int i = 1; i < argc - 1; ++i)
    {
        if (std::strcmp(argv[i], "--device") == 0)
            return std::atoi(argv[i + 1]);
    }
    return 0;
}

static int runMelody(int argc, char** argv);

#if DAP_HAS_QT
static int runGui(int argc, char** argv);
#endif

int main(int argc, char** argv)
{
    for (int i = 1; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "--list-devices") == 0)
        {
            listDevices();
            return 0;
        }
    }

#if DAP_HAS_QT
    bool noGui = false;
    for (int i = 1; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "--no-gui") == 0)
        {
            noGui = true;
            break;
        }
    }
    if (!noGui)
        return runGui(argc, argv);
#endif
    return runMelody(argc, argv);
}

#if DAP_HAS_QT
static int runGui(int argc, char** argv)
{
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("Basic");

    int deviceIdx = parseDeviceIndex(argc, argv);

    QQmlApplicationEngine engine;

    engine.singletonInstance<SynthBridge*>("Sub37", "SynthBridge")->setDeviceIndex(deviceIdx);

    engine.loadFromModule("Sub37", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
#endif

static int runMelody(int argc, char** argv)
{
    const auto outputDevices   = AudioDeviceList::create(dap::audioio::Scope::Output);
    const auto outputDeviceIdx = static_cast<std::size_t>(parseDeviceIndex(argc, argv));

    if (outputDeviceIdx >= outputDevices.size())
    {
        std::cout << "Invalid output device index." << std::endl;
        listDevices();
        return -1;
    }

    auto outputDevice = outputDevices[outputDeviceIdx].get();
    assert(outputDevice != nullptr);

    sub37::AudioProcess process(outputDevice->getId(),
                                outputDevice->getChannelCount(),
                                outputDevice->getBufferSize(),
                                outputDevice->getSampleRate());

    auto& synth = process.getSynth();

    // Classic Moog bass patch
    synth.osc1().setShape(dap::dsp::OscillatorFunctions::Shape::Saw);
    synth.osc2().setShape(dap::dsp::OscillatorFunctions::Shape::Saw);
    synth.setOsc2BeatFreq(2.0f);

    // Enable slight noise for analog character
    synth.noise().setGain(0.03f);

    // Filter: moderate cutoff with envelope sweep
    synth.filterEnvelope().setBase(600.0f);
    synth.filterEnvelope().setAmount(4000.0f);
    synth.filterEnvelope().setAttack(0.001f);
    synth.filterEnvelope().setDecay(0.35f);
    synth.filterEnvelope().setSustain(0.0f);
    synth.filterEnvelope().setRelease(0.15f);
    synth.filter().setResonance(0.5f);

    // Amp envelope: punchy
    synth.envelope().setAttack(0.003f);
    synth.envelope().setDecay(0.25f);
    synth.envelope().setSustain(0.8f);
    synth.envelope().setRelease(0.1f);

    // MIDI note to frequency
    auto noteFreq = [](int midi) { return 440.0f * std::pow(2.0f, (midi - 69) / 12.0f); };

    auto playNote = [&](int midiNote, int durationMs)
    {
        float freq = noteFreq(midiNote);
        synth.setNote(freq);
        synth.envelope().setGate(1.0f);
        std::this_thread::sleep_for(std::chrono::milliseconds(durationMs - 30));
        synth.envelope().setGate(0.0f);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    };

    auto rest = [](int durationMs)
    { std::this_thread::sleep_for(std::chrono::milliseconds(durationMs)); };

    [[maybe_unused]] bool started = process.start();
    assert(started);

    std::cout << "\nPlaying Moog bass line..." << std::endl;

    // Funk bass line in E minor (2 repeats)
    for (int rep = 0; rep < 2; ++rep)
    {
        playNote(28, 200); // E1
        rest(50);
        playNote(28, 200); // E1
        playNote(31, 150); // G1
        playNote(33, 300); // A1
        playNote(28, 200); // E1
        rest(100);
        playNote(38, 150); // D2
        playNote(35, 300); // B1
        playNote(33, 400); // A1
        playNote(28, 600); // E1
        rest(100);
    }

    // Bridge: legato with glide
    synth.setGlide(2048);
    synth.envelope().setAttack(0.05f);
    synth.filterEnvelope().setDecay(0.8f);
    synth.filterEnvelope().setAmount(2000.0f);
    synth.setPitchLfo(5.5f, 2.0f);

    playNote(40, 1500); // E2
    playNote(43, 1500); // G2
    playNote(45, 1500); // A2
    playNote(47, 2000); // B2
    playNote(45, 1500); // A2
    playNote(43, 1500); // G2
    playNote(40, 2500); // E2

    // Return to staccato
    synth.setGlide(0);
    synth.setPitchLfo(5.5f, 0.0f);
    synth.envelope().setAttack(0.003f);
    synth.filterEnvelope().setDecay(0.35f);
    synth.filterEnvelope().setAmount(4000.0f);

    // Final phrase
    for (int rep = 0; rep < 2; ++rep)
    {
        playNote(28, 200);
        rest(50);
        playNote(28, 200);
        playNote(31, 150);
        playNote(33, 300);
        playNote(28, 200);
        rest(100);
        playNote(38, 150);
        playNote(35, 300);
        playNote(33, 400);
        playNote(28, 600);
        rest(100);
    }

    // Final low E with filter sweep
    synth.filterEnvelope().setAmount(6000.0f);
    synth.filterEnvelope().setDecay(1.5f);
    synth.filter().setResonance(0.7f);
    playNote(28, 3000);

    [[maybe_unused]] const bool stopped = process.stop();
    assert(stopped);

    return 0;
}
