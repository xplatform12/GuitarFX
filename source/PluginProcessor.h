#pragma once

#include <JuceHeader.h>
#include <atomic>
#include <array>

class GuitarFXAudioProcessor final : public juce::AudioProcessor
{
public:
    static constexpr int eqNumBands = 10;
    static constexpr int waveformSize = 512;

    enum StageIndex
    {
        stageInput = 0,
        stageGate,
        stageComp,
        stageDrive,
        stageAmp,
        stageCab,
        stageDelay,
        stageReverb,
        stageOutput,
        stageCount
    };

    GuitarFXAudioProcessor();
    ~GuitarFXAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    float getStageInputMeterDb (int stage) const noexcept;
    float getStageOutputMeterDb (int stage) const noexcept;
    void copyEqInputWaveform (std::array<float, waveformSize>& dest) const;
    static float getEqBandFrequency (int band);

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void updateDspParameters();
    void processNoiseGate (juce::AudioBuffer<float>& buffer);
    void processCompressor (juce::AudioBuffer<float>& buffer);
    void processDrive (juce::AudioBuffer<float>& buffer);
    void processAmpSim (juce::AudioBuffer<float>& buffer);
    void processCabSim (juce::AudioBuffer<float>& buffer);
    void processDelay (juce::AudioBuffer<float>& buffer);
    void processGraphicEq (juce::AudioBuffer<float>& buffer);
    void captureEqInputWaveform (const juce::AudioBuffer<float>& buffer);
    void updateStageMeters (int stage, const juce::AudioBuffer<float>& buffer, bool isInputMeter);
    static float computeBufferDb (const juce::AudioBuffer<float>& buffer);

    double currentSampleRate = 44100.0;

    juce::dsp::Gain<float> inputGain;
    juce::dsp::Gain<float> outputGain;
    juce::dsp::Compressor<float> compressor;
    juce::dsp::Reverb reverb;
    using EqBandFilter = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
    std::array<EqBandFilter, eqNumBands> graphicEqBands;

    std::array<juce::dsp::StateVariableTPTFilter<float>, 2> ampToneFilters;
    std::array<juce::dsp::StateVariableTPTFilter<float>, 2> inputHighPassFilters;
    std::array<juce::dsp::StateVariableTPTFilter<float>, 2> cabHighPassFilters;
    std::array<juce::dsp::StateVariableTPTFilter<float>, 2> cabLowPassFilters;
    std::array<juce::dsp::StateVariableTPTFilter<float>, 2> driveToneFilters;
    std::array<juce::dsp::StateVariableTPTFilter<float>, 2> ampPresenceFilters;
    std::array<juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>, 2> delays {
        juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> (192000),
        juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> (192000)
    };

    float gateEnv = 0.0f;
    float gateGain = 1.0f;
    float gateOpenCoeff = 0.0f;
    float gateCloseCoeff = 0.0f;
    float gateThresholdLin = 0.02f;
    float gateRangeLin = 0.0f;

    float driveAmount = 3.0f;
    float driveLevel = 1.0f;
    float driveBlend = 1.0f;
    float ampAmount = 2.0f;
    float ampPresence = 0.3f;
    float ampLevel = 1.0f;
    float delayFeedback = 0.35f;
    float delayMix = 0.2f;
    float delayToneCoeff = 0.4f;
    std::array<float, 2> delayToneState { 0.0f, 0.0f };
    float compMix = 1.0f;
    float compMakeup = 1.0f;
    bool eqBypassed = false;
    mutable juce::SpinLock waveformLock;
    std::array<float, waveformSize> eqInputWaveform {};
    static constexpr int waveformHistorySize = 8192;
    std::array<float, waveformHistorySize> eqWaveformHistory {};
    int eqWaveformWritePos = 0;
    std::array<std::atomic<float>, stageCount> stageInputMetersDb;
    std::array<std::atomic<float>, stageCount> stageOutputMetersDb;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarFXAudioProcessor)
};
