#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
constexpr auto gateBypassId = "gateBypass";
constexpr auto gateThresholdId = "gateThresholdDb";
constexpr auto gateReleaseId = "gateReleaseMs";
constexpr auto gateRangeId = "gateRangeDb";

constexpr auto compBypassId = "compBypass";
constexpr auto compThresholdId = "compThresholdDb";
constexpr auto compRatioId = "compRatio";
constexpr auto compAttackId = "compAttackMs";
constexpr auto compReleaseId = "compReleaseMs";
constexpr auto compMixId = "compMix";
constexpr auto compMakeupId = "compMakeupDb";

constexpr auto driveBypassId = "driveBypass";
constexpr auto driveId = "drive";
constexpr auto driveToneId = "driveToneHz";
constexpr auto driveLevelId = "driveLevelDb";
constexpr auto driveBlendId = "driveBlend";

constexpr auto ampBypassId = "ampBypass";
constexpr auto ampGainId = "ampGain";
constexpr auto ampToneId = "ampToneHz";
constexpr auto ampPresenceId = "ampPresence";
constexpr auto ampLevelId = "ampLevelDb";

constexpr auto cabBypassId = "cabBypass";
constexpr auto cabHPId = "cabHighPassHz";
constexpr auto cabLPId = "cabLowPassHz";
constexpr auto cabResId = "cabResonance";

constexpr auto delayBypassId = "delayBypass";
constexpr auto delayTimeId = "delayTimeMs";
constexpr auto delayFeedbackId = "delayFeedback";
constexpr auto delayMixId = "delayMix";
constexpr auto delayToneId = "delayToneHz";

constexpr auto reverbBypassId = "reverbBypass";
constexpr auto reverbRoomId = "reverbRoom";
constexpr auto reverbDampId = "reverbDamp";
constexpr auto reverbWetId = "reverbWet";
constexpr auto reverbWidthId = "reverbWidth";

constexpr auto inGainId = "inputGainDb";
constexpr auto inputHPId = "inputHighPassHz";
constexpr auto outGainId = "outputGainDb";
constexpr auto eqBypassId = "eqBypass";

constexpr std::array<float, GuitarFXAudioProcessor::eqNumBands> eqFrequencies {
    31.25f, 62.5f, 125.0f, 250.0f, 500.0f, 1000.0f, 2000.0f, 4000.0f, 8000.0f, 16000.0f
};

static juce::String getEqBandParamId (const int band)
{
    return "eqBand" + juce::String (band);
}
}

static float softClip (const float x)
{
    return std::tanh (x);
}

juce::AudioProcessorValueTreeState::ParameterLayout GuitarFXAudioProcessor::createParameterLayout()
{
    using APB = juce::AudioParameterBool;
    using APF = juce::AudioParameterFloat;

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<APF> (inGainId, "Input Gain (dB)", juce::NormalisableRange<float> (-24.0f, 24.0f, 0.01f), 0.0f));
    params.push_back (std::make_unique<APF> (inputHPId, "Input High Pass (Hz)", juce::NormalisableRange<float> (20.0f, 220.0f, 1.0f), 35.0f));

    params.push_back (std::make_unique<APB> (gateBypassId, "Gate Bypass", false));
    params.push_back (std::make_unique<APF> (gateThresholdId, "Gate Threshold (dB)", juce::NormalisableRange<float> (-80.0f, -20.0f, 0.1f), -55.0f));
    params.push_back (std::make_unique<APF> (gateReleaseId, "Gate Release (ms)", juce::NormalisableRange<float> (10.0f, 300.0f, 0.1f), 80.0f));
    params.push_back (std::make_unique<APF> (gateRangeId, "Gate Range (dB)", juce::NormalisableRange<float> (-80.0f, 0.0f, 0.1f), -60.0f));

    params.push_back (std::make_unique<APB> (compBypassId, "Compressor Bypass", false));
    params.push_back (std::make_unique<APF> (compThresholdId, "Compressor Threshold (dB)", juce::NormalisableRange<float> (-40.0f, 0.0f, 0.1f), -18.0f));
    params.push_back (std::make_unique<APF> (compRatioId, "Compressor Ratio", juce::NormalisableRange<float> (1.0f, 12.0f, 0.01f), 3.0f));
    params.push_back (std::make_unique<APF> (compAttackId, "Compressor Attack (ms)", juce::NormalisableRange<float> (0.5f, 60.0f, 0.1f), 8.0f));
    params.push_back (std::make_unique<APF> (compReleaseId, "Compressor Release (ms)", juce::NormalisableRange<float> (10.0f, 300.0f, 0.1f), 120.0f));
    params.push_back (std::make_unique<APF> (compMixId, "Compressor Mix", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 1.0f));
    params.push_back (std::make_unique<APF> (compMakeupId, "Compressor Makeup (dB)", juce::NormalisableRange<float> (-6.0f, 18.0f, 0.01f), 0.0f));

    params.push_back (std::make_unique<APB> (driveBypassId, "Drive Bypass", false));
    params.push_back (std::make_unique<APF> (driveId, "Drive", juce::NormalisableRange<float> (1.0f, 15.0f, 0.01f), 3.0f));
    params.push_back (std::make_unique<APF> (driveToneId, "Drive Tone (Hz)", juce::NormalisableRange<float> (700.0f, 7000.0f, 1.0f), 2400.0f));
    params.push_back (std::make_unique<APF> (driveLevelId, "Drive Level (dB)", juce::NormalisableRange<float> (-18.0f, 18.0f, 0.01f), 0.0f));
    params.push_back (std::make_unique<APF> (driveBlendId, "Drive Blend", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 1.0f));

    params.push_back (std::make_unique<APB> (ampBypassId, "Amp Bypass", false));
    params.push_back (std::make_unique<APF> (ampGainId, "Amp Gain", juce::NormalisableRange<float> (1.0f, 10.0f, 0.01f), 2.5f));
    params.push_back (std::make_unique<APF> (ampToneId, "Amp Tone (Hz)", juce::NormalisableRange<float> (600.0f, 4500.0f, 1.0f), 1800.0f));
    params.push_back (std::make_unique<APF> (ampPresenceId, "Amp Presence", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.3f));
    params.push_back (std::make_unique<APF> (ampLevelId, "Amp Level (dB)", juce::NormalisableRange<float> (-18.0f, 18.0f, 0.01f), 0.0f));

    params.push_back (std::make_unique<APB> (cabBypassId, "Cab Bypass", false));
    params.push_back (std::make_unique<APF> (cabHPId, "Cab HP (Hz)", juce::NormalisableRange<float> (50.0f, 250.0f, 1.0f), 85.0f));
    params.push_back (std::make_unique<APF> (cabLPId, "Cab LP (Hz)", juce::NormalisableRange<float> (1800.0f, 9000.0f, 1.0f), 5000.0f));
    params.push_back (std::make_unique<APF> (cabResId, "Cab Resonance", juce::NormalisableRange<float> (0.25f, 1.4f, 0.001f), 0.8f));

    params.push_back (std::make_unique<APB> (delayBypassId, "Delay Bypass", false));
    params.push_back (std::make_unique<APF> (delayTimeId, "Delay Time (ms)", juce::NormalisableRange<float> (20.0f, 900.0f, 0.1f), 320.0f));
    params.push_back (std::make_unique<APF> (delayFeedbackId, "Delay Feedback", juce::NormalisableRange<float> (0.0f, 0.9f, 0.001f), 0.35f));
    params.push_back (std::make_unique<APF> (delayMixId, "Delay Mix", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.2f));
    params.push_back (std::make_unique<APF> (delayToneId, "Delay Tone (Hz)", juce::NormalisableRange<float> (900.0f, 12000.0f, 1.0f), 5200.0f));

    params.push_back (std::make_unique<APB> (reverbBypassId, "Reverb Bypass", false));
    params.push_back (std::make_unique<APF> (reverbRoomId, "Reverb Room", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.35f));
    params.push_back (std::make_unique<APF> (reverbDampId, "Reverb Damp", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.4f));
    params.push_back (std::make_unique<APF> (reverbWetId, "Reverb Wet", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.15f));
    params.push_back (std::make_unique<APF> (reverbWidthId, "Reverb Width", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 1.0f));

    params.push_back (std::make_unique<APF> (outGainId, "Output Gain (dB)", juce::NormalisableRange<float> (-24.0f, 24.0f, 0.01f), 0.0f));
    params.push_back (std::make_unique<APB> (eqBypassId, "Equalizer Bypass", false));
    for (int i = 0; i < eqNumBands; ++i)
    {
        const auto label = juce::String (eqFrequencies[static_cast<size_t> (i)], 1) + " Hz";
        params.push_back (std::make_unique<APF> (getEqBandParamId (i), "Equalizer " + label + " (dB)",
                                                 juce::NormalisableRange<float> (-12.0f, 12.0f, 0.01f), 0.0f));
    }

    return { params.begin(), params.end() };
}

GuitarFXAudioProcessor::GuitarFXAudioProcessor()
    : juce::AudioProcessor (BusesProperties()
                                .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", createParameterLayout())
{
    for (int i = 0; i < stageCount; ++i)
    {
        stageInputMetersDb[static_cast<size_t> (i)].store (-100.0f);
        stageOutputMetersDb[static_cast<size_t> (i)].store (-100.0f);
    }
}

void GuitarFXAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32> (getTotalNumOutputChannels());

    inputGain.prepare (spec);
    outputGain.prepare (spec);
    compressor.prepare (spec);
    reverb.prepare (spec);
    for (auto& eqBand : graphicEqBands)
    {
        eqBand.prepare (spec);
        eqBand.reset();
    }

    inputGain.setRampDurationSeconds (0.02);
    outputGain.setRampDurationSeconds (0.02);

    for (size_t ch = 0; ch < 2; ++ch)
    {
        inputHighPassFilters[ch].prepare (spec);
        inputHighPassFilters[ch].setType (juce::dsp::StateVariableTPTFilterType::highpass);
        inputHighPassFilters[ch].setResonance (0.7f);

        driveToneFilters[ch].prepare (spec);
        driveToneFilters[ch].setType (juce::dsp::StateVariableTPTFilterType::lowpass);
        driveToneFilters[ch].setResonance (0.8f);

        ampToneFilters[ch].prepare (spec);
        ampToneFilters[ch].setType (juce::dsp::StateVariableTPTFilterType::lowpass);
        ampToneFilters[ch].setResonance (0.6f);

        ampPresenceFilters[ch].prepare (spec);
        ampPresenceFilters[ch].setType (juce::dsp::StateVariableTPTFilterType::highpass);
        ampPresenceFilters[ch].setResonance (0.6f);

        cabHighPassFilters[ch].prepare (spec);
        cabHighPassFilters[ch].setType (juce::dsp::StateVariableTPTFilterType::highpass);
        cabHighPassFilters[ch].setResonance (0.7f);

        cabLowPassFilters[ch].prepare (spec);
        cabLowPassFilters[ch].setType (juce::dsp::StateVariableTPTFilterType::lowpass);
        cabLowPassFilters[ch].setResonance (0.8f);

        delays[ch].prepare (spec);
        delays[ch].reset();
        delayToneState[ch] = 0.0f;
    }

    gateEnv = 0.0f;
    gateGain = 1.0f;

    updateDspParameters();
}

void GuitarFXAudioProcessor::releaseResources()
{
}

bool GuitarFXAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto inSet = layouts.getMainInputChannelSet();
    const auto outSet = layouts.getMainOutputChannelSet();
    const bool mono = (inSet == juce::AudioChannelSet::mono() && outSet == juce::AudioChannelSet::mono());
    const bool stereo = (inSet == juce::AudioChannelSet::stereo() && outSet == juce::AudioChannelSet::stereo());
    return mono || stereo;
}

void GuitarFXAudioProcessor::updateDspParameters()
{
    inputGain.setGainDecibels (apvts.getRawParameterValue (inGainId)->load());
    outputGain.setGainDecibels (apvts.getRawParameterValue (outGainId)->load());
    const auto inputHp = apvts.getRawParameterValue (inputHPId)->load();
    for (size_t ch = 0; ch < 2; ++ch)
        inputHighPassFilters[ch].setCutoffFrequency (inputHp);

    gateThresholdLin = juce::Decibels::decibelsToGain (apvts.getRawParameterValue (gateThresholdId)->load());
    gateRangeLin = juce::Decibels::decibelsToGain (apvts.getRawParameterValue (gateRangeId)->load());
    const auto gateReleaseSeconds = juce::jmax (0.01f, apvts.getRawParameterValue (gateReleaseId)->load() / 1000.0f);
    gateOpenCoeff = std::exp (-1.0f / static_cast<float> (currentSampleRate * 0.002));
    gateCloseCoeff = std::exp (-1.0f / static_cast<float> (currentSampleRate * gateReleaseSeconds));

    compressor.setThreshold (apvts.getRawParameterValue (compThresholdId)->load());
    compressor.setRatio (apvts.getRawParameterValue (compRatioId)->load());
    compressor.setAttack (apvts.getRawParameterValue (compAttackId)->load());
    compressor.setRelease (apvts.getRawParameterValue (compReleaseId)->load());
    compMix = apvts.getRawParameterValue (compMixId)->load();
    compMakeup = juce::Decibels::decibelsToGain (apvts.getRawParameterValue (compMakeupId)->load());

    driveAmount = apvts.getRawParameterValue (driveId)->load();
    driveLevel = juce::Decibels::decibelsToGain (apvts.getRawParameterValue (driveLevelId)->load());
    driveBlend = apvts.getRawParameterValue (driveBlendId)->load();
    const auto driveToneHz = apvts.getRawParameterValue (driveToneId)->load();
    for (size_t ch = 0; ch < 2; ++ch)
        driveToneFilters[ch].setCutoffFrequency (driveToneHz);

    ampAmount = apvts.getRawParameterValue (ampGainId)->load();
    ampPresence = apvts.getRawParameterValue (ampPresenceId)->load();
    ampLevel = juce::Decibels::decibelsToGain (apvts.getRawParameterValue (ampLevelId)->load());

    const auto ampToneHz = apvts.getRawParameterValue (ampToneId)->load();
    for (size_t ch = 0; ch < 2; ++ch)
    {
        ampToneFilters[ch].setCutoffFrequency (ampToneHz);
        ampPresenceFilters[ch].setCutoffFrequency (3000.0f);
    }

    const auto cabHpHz = apvts.getRawParameterValue (cabHPId)->load();
    const auto cabLpHz = apvts.getRawParameterValue (cabLPId)->load();
    const auto cabRes = apvts.getRawParameterValue (cabResId)->load();
    for (size_t ch = 0; ch < 2; ++ch)
    {
        cabHighPassFilters[ch].setCutoffFrequency (cabHpHz);
        cabLowPassFilters[ch].setCutoffFrequency (cabLpHz);
        cabHighPassFilters[ch].setResonance (cabRes);
        cabLowPassFilters[ch].setResonance (cabRes);
    }

    const auto delayTimeMs = apvts.getRawParameterValue (delayTimeId)->load();
    const auto delaySamps = juce::jlimit (1.0f, 191999.0f, (delayTimeMs / 1000.0f) * static_cast<float> (currentSampleRate));
    for (size_t ch = 0; ch < 2; ++ch)
        delays[ch].setDelay (delaySamps);
    delayFeedback = apvts.getRawParameterValue (delayFeedbackId)->load();
    delayMix = apvts.getRawParameterValue (delayMixId)->load();
    const auto delayToneHz = apvts.getRawParameterValue (delayToneId)->load();
    delayToneCoeff = 1.0f - std::exp (-2.0f * juce::MathConstants<float>::pi * delayToneHz / static_cast<float> (currentSampleRate));

    juce::dsp::Reverb::Parameters rvb;
    rvb.roomSize = apvts.getRawParameterValue (reverbRoomId)->load();
    rvb.damping = apvts.getRawParameterValue (reverbDampId)->load();
    rvb.wetLevel = apvts.getRawParameterValue (reverbWetId)->load();
    rvb.dryLevel = 1.0f - (rvb.wetLevel * 0.35f);
    rvb.width = apvts.getRawParameterValue (reverbWidthId)->load();
    rvb.freezeMode = 0.0f;
    reverb.setParameters (rvb);

    const auto nyquistSafe = juce::jmax (200.0f, static_cast<float> (currentSampleRate * 0.45));
    eqBypassed = apvts.getRawParameterValue (eqBypassId)->load() > 0.5f;
    for (int i = 0; i < eqNumBands; ++i)
    {
        const auto gainDb = apvts.getRawParameterValue (getEqBandParamId (i))->load();
        const auto freq = juce::jlimit (20.0f, nyquistSafe, eqFrequencies[static_cast<size_t> (i)]);
        const auto coeff = juce::dsp::IIR::Coefficients<float>::makePeakFilter (
            currentSampleRate, freq, 1.2f, juce::Decibels::decibelsToGain (gainDb));
        *graphicEqBands[static_cast<size_t> (i)].state = *coeff;
    }
}

void GuitarFXAudioProcessor::processNoiseGate (juce::AudioBuffer<float>& buffer)
{
    const auto numSamples = buffer.getNumSamples();
    const auto numChannels = buffer.getNumChannels();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float detector = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch)
            detector = juce::jmax (detector, std::abs (buffer.getReadPointer (ch)[sample]));

        gateEnv = juce::jmax (detector, gateEnv * 0.995f);
        const auto targetGain = (gateEnv >= gateThresholdLin) ? 1.0f : gateRangeLin;
        const auto coeff = (targetGain > gateGain) ? gateOpenCoeff : gateCloseCoeff;
        gateGain = targetGain + coeff * (gateGain - targetGain);

        for (int ch = 0; ch < numChannels; ++ch)
            buffer.getWritePointer (ch)[sample] *= gateGain;
    }
}

void GuitarFXAudioProcessor::processCompressor (juce::AudioBuffer<float>& buffer)
{
    juce::AudioBuffer<float> dry (buffer.getNumChannels(), buffer.getNumSamples());
    dry.makeCopyOf (buffer, true);

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    compressor.process (context);

    buffer.applyGain (compMakeup);

    if (compMix < 0.999f)
    {
        const auto dryMix = 1.0f - compMix;
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* wet = buffer.getWritePointer (ch);
            const auto* d = dry.getReadPointer (ch);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
                wet[i] = wet[i] * compMix + d[i] * dryMix;
        }
    }
}

void GuitarFXAudioProcessor::processDrive (juce::AudioBuffer<float>& buffer)
{
    const auto numSamples = buffer.getNumSamples();
    const auto numChannels = juce::jmin (2, buffer.getNumChannels());
    juce::AudioBuffer<float> dry (numChannels, numSamples);
    dry.makeCopyOf (buffer, true);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* x = buffer.getWritePointer (ch);
        for (int sample = 0; sample < numSamples; ++sample)
            x[sample] = softClip (x[sample] * driveAmount);

        juce::dsp::AudioBlock<float> block (buffer);
        auto singleChannel = block.getSingleChannelBlock (static_cast<size_t> (ch));
        juce::dsp::ProcessContextReplacing<float> context (singleChannel);
        driveToneFilters[static_cast<size_t> (ch)].process (context);
    }

    const auto dryMix = 1.0f - driveBlend;
    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* wet = buffer.getWritePointer (ch);
        const auto* d = dry.getReadPointer (ch);
        for (int sample = 0; sample < numSamples; ++sample)
            wet[sample] = (wet[sample] * driveBlend + d[sample] * dryMix) * driveLevel;
    }
}

void GuitarFXAudioProcessor::processAmpSim (juce::AudioBuffer<float>& buffer)
{
    const auto numSamples = buffer.getNumSamples();
    const auto numChannels = juce::jmin (2, buffer.getNumChannels());

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* x = buffer.getWritePointer (ch);
        for (int sample = 0; sample < numSamples; ++sample)
            x[sample] = 0.85f * softClip (x[sample] * ampAmount) + 0.15f * x[sample];

        juce::dsp::AudioBlock<float> block (buffer);
        auto singleChannel = block.getSingleChannelBlock (static_cast<size_t> (ch));
        juce::dsp::ProcessContextReplacing<float> context (singleChannel);
        ampToneFilters[static_cast<size_t> (ch)].process (context);

        juce::AudioBuffer<float> temp (1, numSamples);
        temp.copyFrom (0, 0, buffer, ch, 0, numSamples);
        juce::dsp::AudioBlock<float> tempBlock (temp);
        juce::dsp::ProcessContextReplacing<float> tempContext (tempBlock);
        ampPresenceFilters[static_cast<size_t> (ch)].process (tempContext);
        auto* hi = temp.getWritePointer (0);
        for (int sample = 0; sample < numSamples; ++sample)
            x[sample] = (x[sample] + hi[sample] * (0.85f * ampPresence)) * ampLevel;
    }
}

void GuitarFXAudioProcessor::processCabSim (juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block (buffer);
    const auto numChannels = juce::jmin (static_cast<size_t> (2), block.getNumChannels());
    for (size_t ch = 0; ch < numChannels; ++ch)
    {
        auto singleChannel = block.getSingleChannelBlock (ch);
        juce::dsp::ProcessContextReplacing<float> context (singleChannel);
        cabHighPassFilters[ch].process (context);
        cabLowPassFilters[ch].process (context);
    }
}

void GuitarFXAudioProcessor::processDelay (juce::AudioBuffer<float>& buffer)
{
    const auto numSamples = buffer.getNumSamples();
    const auto numChannels = juce::jmin (2, buffer.getNumChannels());

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* x = buffer.getWritePointer (ch);
        auto& delay = delays[static_cast<size_t> (ch)];

        for (int sample = 0; sample < numSamples; ++sample)
        {
            const auto dry = x[sample];
            auto delayed = delay.popSample (0);
            auto& state = delayToneState[static_cast<size_t> (ch)];
            state += delayToneCoeff * (delayed - state);
            delayed = state;

            delay.pushSample (0, dry + delayed * delayFeedback);
            x[sample] = dry * (1.0f - delayMix) + delayed * delayMix;
        }
    }
}

void GuitarFXAudioProcessor::processGraphicEq (juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    for (auto& eqBand : graphicEqBands)
        eqBand.process (context);
}

void GuitarFXAudioProcessor::captureEqInputWaveform (const juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumSamples() <= 0 || buffer.getNumChannels() <= 0)
        return;

    const auto* left = buffer.getReadPointer (0);
    const auto* right = buffer.getNumChannels() > 1 ? buffer.getReadPointer (1) : nullptr;
    const juce::SpinLock::ScopedLockType lock (waveformLock);
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        const float mono = right != nullptr ? 0.5f * (left[i] + right[i]) : left[i];
        eqWaveformHistory[static_cast<size_t> (eqWaveformWritePos)] = juce::jlimit (-1.0f, 1.0f, mono);
        eqWaveformWritePos++;
        if (eqWaveformWritePos >= waveformHistorySize)
            eqWaveformWritePos = 0;
    }
}

float GuitarFXAudioProcessor::computeBufferDb (const juce::AudioBuffer<float>& buffer)
{
    float rms = 0.0f;
    for (int ch = 0; ch < juce::jmin (2, buffer.getNumChannels()); ++ch)
        rms = juce::jmax (rms, buffer.getRMSLevel (ch, 0, buffer.getNumSamples()));
    return juce::Decibels::gainToDecibels (rms, -100.0f);
}

void GuitarFXAudioProcessor::updateStageMeters (int stage, const juce::AudioBuffer<float>& buffer, bool isInputMeter)
{
    const auto db = computeBufferDb (buffer);
    auto& meter = isInputMeter ? stageInputMetersDb[static_cast<size_t> (stage)] : stageOutputMetersDb[static_cast<size_t> (stage)];
    const auto prev = meter.load();
    meter.store (0.82f * prev + 0.18f * db);
}

void GuitarFXAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    for (auto ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());

    updateDspParameters();

    updateStageMeters (stageInput, buffer, true);
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        inputGain.process (context);

        const auto numChannels = juce::jmin (static_cast<size_t> (2), block.getNumChannels());
        for (size_t ch = 0; ch < numChannels; ++ch)
        {
            auto singleChannel = block.getSingleChannelBlock (ch);
            juce::dsp::ProcessContextReplacing<float> chContext (singleChannel);
            inputHighPassFilters[ch].process (chContext);
        }
    }
    updateStageMeters (stageInput, buffer, false);

    updateStageMeters (stageGate, buffer, true);
    if (apvts.getRawParameterValue (gateBypassId)->load() < 0.5f)
        processNoiseGate (buffer);
    updateStageMeters (stageGate, buffer, false);

    updateStageMeters (stageComp, buffer, true);
    if (apvts.getRawParameterValue (compBypassId)->load() < 0.5f)
        processCompressor (buffer);
    updateStageMeters (stageComp, buffer, false);

    updateStageMeters (stageDrive, buffer, true);
    if (apvts.getRawParameterValue (driveBypassId)->load() < 0.5f)
        processDrive (buffer);
    updateStageMeters (stageDrive, buffer, false);

    updateStageMeters (stageAmp, buffer, true);
    if (apvts.getRawParameterValue (ampBypassId)->load() < 0.5f)
        processAmpSim (buffer);
    updateStageMeters (stageAmp, buffer, false);

    updateStageMeters (stageCab, buffer, true);
    if (apvts.getRawParameterValue (cabBypassId)->load() < 0.5f)
        processCabSim (buffer);
    updateStageMeters (stageCab, buffer, false);

    updateStageMeters (stageDelay, buffer, true);
    if (apvts.getRawParameterValue (delayBypassId)->load() < 0.5f)
        processDelay (buffer);
    updateStageMeters (stageDelay, buffer, false);

    updateStageMeters (stageReverb, buffer, true);
    if (apvts.getRawParameterValue (reverbBypassId)->load() < 0.5f)
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        reverb.process (context);
    }
    updateStageMeters (stageReverb, buffer, false);

    updateStageMeters (stageOutput, buffer, true);
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        outputGain.process (context);
    }
    captureEqInputWaveform (buffer);
    if (!eqBypassed)
        processGraphicEq (buffer);
    updateStageMeters (stageOutput, buffer, false);
}

juce::AudioProcessorEditor* GuitarFXAudioProcessor::createEditor()
{
    return new GuitarFXAudioProcessorEditor (*this);
}

void GuitarFXAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (const auto state = apvts.copyState(); state.isValid())
    {
        juce::MemoryOutputStream stream (destData, true);
        state.writeToStream (stream);
    }
}

void GuitarFXAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (const auto state = juce::ValueTree::readFromData (data, static_cast<size_t> (sizeInBytes)); state.isValid())
        apvts.replaceState (state);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GuitarFXAudioProcessor();
}

float GuitarFXAudioProcessor::getStageInputMeterDb (int stage) const noexcept
{
    if (stage < 0 || stage >= stageCount)
        return -100.0f;

    return stageInputMetersDb[static_cast<size_t> (stage)].load();
}

float GuitarFXAudioProcessor::getStageOutputMeterDb (int stage) const noexcept
{
    if (stage < 0 || stage >= stageCount)
        return -100.0f;

    return stageOutputMetersDb[static_cast<size_t> (stage)].load();
}

void GuitarFXAudioProcessor::copyEqInputWaveform (std::array<float, waveformSize>& dest) const
{
    const juce::SpinLock::ScopedLockType lock (waveformLock);
    constexpr int readWindow = 4096;
    const int window = juce::jmin (readWindow, waveformHistorySize - 1);
    const int start = (eqWaveformWritePos - window + waveformHistorySize) % waveformHistorySize;

    for (int i = 0; i < waveformSize; ++i)
    {
        const float pos = (static_cast<float> (i) / static_cast<float> (waveformSize - 1)) * static_cast<float> (window - 1);
        const int idxA = static_cast<int> (pos);
        const int idxB = juce::jmin (window - 1, idxA + 1);
        const float frac = pos - static_cast<float> (idxA);

        const int ringA = (start + idxA) % waveformHistorySize;
        const int ringB = (start + idxB) % waveformHistorySize;
        const float a = eqWaveformHistory[static_cast<size_t> (ringA)];
        const float b = eqWaveformHistory[static_cast<size_t> (ringB)];
        dest[static_cast<size_t> (i)] = a + frac * (b - a);
    }
}

float GuitarFXAudioProcessor::getEqBandFrequency (int band)
{
    if (band < 0 || band >= eqNumBands)
        return 1000.0f;

    return eqFrequencies[static_cast<size_t> (band)];
}
