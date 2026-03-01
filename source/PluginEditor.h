#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class GuitarFXAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                           private juce::Timer
{
public:
    explicit GuitarFXAudioProcessorEditor (GuitarFXAudioProcessor&);
    ~GuitarFXAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    class ProLookAndFeel;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    void setupKnob (juce::Slider& knob, juce::Label& label, const juce::String& text);
    void setupBypassButton (juce::ToggleButton& button);
    void createBackgroundImage();
    void drawMeterPair (juce::Graphics& g, const juce::Rectangle<int>& inArea, const juce::Rectangle<int>& outArea, int stage);
    void drawMeter (juce::Graphics& g, const juce::Rectangle<int>& area, float db, bool outputMeter);
    void drawWaveform (juce::Graphics& g, const juce::Rectangle<int>& area);
    void timerCallback() override;

    GuitarFXAudioProcessor& processor;
    std::unique_ptr<ProLookAndFeel> lookAndFeel;
    juce::Image backgroundImage;

    juce::Label titleLabel;
    juce::Label subtitleLabel;

    juce::ToggleButton gateBypassButton;
    juce::ToggleButton compBypassButton;
    juce::ToggleButton driveBypassButton;
    juce::ToggleButton ampBypassButton;
    juce::ToggleButton cabBypassButton;
    juce::ToggleButton delayBypassButton;
    juce::ToggleButton reverbBypassButton;
    juce::ToggleButton eqBypassButton;
    juce::TextButton inputInfoButton;
    juce::TextButton gateInfoButton;
    juce::TextButton compInfoButton;
    juce::TextButton driveInfoButton;
    juce::TextButton ampInfoButton;
    juce::TextButton cabInfoButton;
    juce::TextButton delayInfoButton;
    juce::TextButton reverbInfoButton;
    juce::TextButton outputInfoButton;
    juce::TextButton eqInfoButton;

    juce::Slider inputGainKnob;
    juce::Slider inputHpKnob;
    juce::Slider gateThresholdKnob;
    juce::Slider gateRangeKnob;
    juce::Slider compThresholdKnob;
    juce::Slider compRatioKnob;
    juce::Slider driveKnob;
    juce::Slider driveToneKnob;
    juce::Slider ampGainKnob;
    juce::Slider ampPresenceKnob;
    juce::Slider cabHighPassKnob;
    juce::Slider cabLowPassKnob;
    juce::Slider delayTimeKnob;
    juce::Slider delayFeedbackKnob;
    juce::Slider reverbRoomKnob;
    juce::Slider reverbWetKnob;
    juce::Slider outputGainKnob;
    std::array<std::unique_ptr<juce::Slider>, GuitarFXAudioProcessor::eqNumBands> eqBandSliders;
    std::array<std::unique_ptr<juce::Label>, GuitarFXAudioProcessor::eqNumBands> eqBandLabels;

    juce::Label inputGainLabel;
    juce::Label inputHpLabel;
    juce::Label gateThresholdLabel;
    juce::Label gateRangeLabel;
    juce::Label compThresholdLabel;
    juce::Label compRatioLabel;
    juce::Label driveLabel;
    juce::Label driveToneLabel;
    juce::Label ampGainLabel;
    juce::Label ampPresenceLabel;
    juce::Label cabHighPassLabel;
    juce::Label cabLowPassLabel;
    juce::Label delayTimeLabel;
    juce::Label delayFeedbackLabel;
    juce::Label reverbRoomLabel;
    juce::Label reverbWetLabel;
    juce::Label outputGainLabel;

    std::unique_ptr<ButtonAttachment> gateBypassAttachment;
    std::unique_ptr<ButtonAttachment> compBypassAttachment;
    std::unique_ptr<ButtonAttachment> driveBypassAttachment;
    std::unique_ptr<ButtonAttachment> ampBypassAttachment;
    std::unique_ptr<ButtonAttachment> cabBypassAttachment;
    std::unique_ptr<ButtonAttachment> delayBypassAttachment;
    std::unique_ptr<ButtonAttachment> reverbBypassAttachment;
    std::unique_ptr<ButtonAttachment> eqBypassAttachment;

    std::unique_ptr<SliderAttachment> inputGainAttachment;
    std::unique_ptr<SliderAttachment> inputHpAttachment;
    std::unique_ptr<SliderAttachment> gateThresholdAttachment;
    std::unique_ptr<SliderAttachment> gateRangeAttachment;
    std::unique_ptr<SliderAttachment> compThresholdAttachment;
    std::unique_ptr<SliderAttachment> compRatioAttachment;
    std::unique_ptr<SliderAttachment> driveAttachment;
    std::unique_ptr<SliderAttachment> driveToneAttachment;
    std::unique_ptr<SliderAttachment> ampGainAttachment;
    std::unique_ptr<SliderAttachment> ampPresenceAttachment;
    std::unique_ptr<SliderAttachment> cabHighPassAttachment;
    std::unique_ptr<SliderAttachment> cabLowPassAttachment;
    std::unique_ptr<SliderAttachment> delayTimeAttachment;
    std::unique_ptr<SliderAttachment> delayFeedbackAttachment;
    std::unique_ptr<SliderAttachment> reverbRoomAttachment;
    std::unique_ptr<SliderAttachment> reverbWetAttachment;
    std::unique_ptr<SliderAttachment> outputGainAttachment;
    std::array<std::unique_ptr<SliderAttachment>, GuitarFXAudioProcessor::eqNumBands> eqBandAttachments;

    juce::Rectangle<int> inputCard;
    juce::Rectangle<int> gateCard;
    juce::Rectangle<int> compCard;
    juce::Rectangle<int> driveCard;
    juce::Rectangle<int> ampCard;
    juce::Rectangle<int> cabCard;
    juce::Rectangle<int> delayCard;
    juce::Rectangle<int> reverbCard;
    juce::Rectangle<int> outputCard;
    juce::Rectangle<int> eqSection;
    juce::Rectangle<int> eqWaveformArea;
    juce::Rectangle<int> eqSlidersArea;

    std::array<juce::Rectangle<int>, GuitarFXAudioProcessor::stageCount> meterInAreas;
    std::array<juce::Rectangle<int>, GuitarFXAudioProcessor::stageCount> meterOutAreas;
    std::array<float, GuitarFXAudioProcessor::stageCount> uiInDb;
    std::array<float, GuitarFXAudioProcessor::stageCount> uiOutDb;
    std::array<float, GuitarFXAudioProcessor::waveformSize> uiWaveform {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarFXAudioProcessorEditor)
};
