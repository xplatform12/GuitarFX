#include "PluginEditor.h"

class GuitarFXAudioProcessorEditor::ProLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ProLookAndFeel()
    {
        setColour (juce::Slider::textBoxTextColourId, juce::Colour::fromRGB (246, 236, 221));
        setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour::fromRGBA (8, 10, 14, 210));
        setColour (juce::Slider::textBoxOutlineColourId, juce::Colour::fromRGBA (232, 168, 97, 140));
        setColour (juce::ToggleButton::textColourId, juce::Colour::fromRGB (245, 236, 221));
        setColour (juce::TextButton::textColourOffId, juce::Colour::fromRGB (245, 236, 221));
        setColour (juce::TextButton::textColourOnId, juce::Colour::fromRGB (245, 236, 221));
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float sliderPos, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider&) override
    {
        const auto b = juce::Rectangle<float> (static_cast<float> (x), static_cast<float> (y), static_cast<float> (w), static_cast<float> (h)).reduced (5.0f);
        const auto r = juce::jmin (b.getWidth(), b.getHeight()) * 0.46f;
        const auto c = b.getCentre();
        const auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        juce::ColourGradient face (juce::Colour::fromRGB (52, 58, 66), c.x - r * 0.35f, c.y - r * 0.5f,
                                   juce::Colour::fromRGB (19, 23, 28), c.x + r * 0.5f, c.y + r * 0.6f, false);
        g.setGradientFill (face);
        g.fillEllipse (c.x - r, c.y - r, r * 2.0f, r * 2.0f);
        g.setColour (juce::Colour::fromRGBA (232, 166, 98, 180));
        g.drawEllipse (c.x - r, c.y - r, r * 2.0f, r * 2.0f, 1.1f);

        juce::Path arc;
        const float ar = r - 3.2f;
        arc.addCentredArc (c.x, c.y, ar, ar, 0.0f, rotaryStartAngle, angle, true);
        g.setColour (juce::Colour::fromRGB (255, 194, 115));
        g.strokePath (arc, juce::PathStrokeType (2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        juce::Path pointer;
        pointer.addRoundedRectangle (-2.0f, -ar + 2.4f, 4.0f, ar * 0.52f, 1.4f);
        g.setColour (juce::Colour::fromRGB (255, 238, 206));
        g.fillPath (pointer, juce::AffineTransform::rotation (angle).translated (c.x, c.y));
    }

    void drawLinearSlider (juce::Graphics& g, int x, int y, int w, int h, float sliderPos, float, float,
                           const juce::Slider::SliderStyle style, juce::Slider&) override
    {
        if (style != juce::Slider::LinearVertical)
            return;

        auto bounds = juce::Rectangle<float> (static_cast<float> (x), static_cast<float> (y), static_cast<float> (w), static_cast<float> (h)).reduced (4.0f, 2.0f);
        auto track = bounds.withWidth (6.0f).withCentre (bounds.getCentre());
        g.setColour (juce::Colour::fromRGBA (25, 30, 37, 220));
        g.fillRoundedRectangle (track, 3.0f);
        g.setColour (juce::Colour::fromRGBA (223, 166, 98, 120));
        g.drawRoundedRectangle (track, 3.0f, 1.0f);

        auto thumb = juce::Rectangle<float> (track.getX() - 5.0f, sliderPos - 4.0f, track.getWidth() + 10.0f, 8.0f);
        g.setColour (juce::Colour::fromRGB (241, 233, 221));
        g.fillRoundedRectangle (thumb, 2.5f);
        g.setColour (juce::Colour::fromRGBA (20, 24, 30, 140));
        g.drawRoundedRectangle (thumb, 2.5f, 1.0f);
    }

    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& b, bool, bool) override
    {
        auto r = b.getLocalBounds().toFloat().reduced (1.0f);
        const bool bypassed = b.getToggleState();

        g.setColour (juce::Colour::fromRGBA (35, 41, 49, 220));
        g.fillRoundedRectangle (r, 5.0f);
        g.setColour (juce::Colour::fromRGBA (183, 188, 196, 145));
        g.drawRoundedRectangle (r, 5.0f, 1.0f);

        g.setColour (juce::Colour::fromRGB (246, 236, 221));
        g.setFont (juce::Font (11.4f, juce::Font::bold));
        auto switchArea = b.getLocalBounds().reduced (6, 4).removeFromRight (46);
        auto textArea = b.getLocalBounds().reduced (8, 4);
        textArea.setRight (switchArea.getX() - 4);
        g.drawFittedText (b.getButtonText(), textArea, juce::Justification::centredLeft, 1);

        auto track = switchArea.toFloat().reduced (2.0f, 5.0f);
        const auto trackTop = bypassed ? juce::Colour::fromRGB (196, 72, 72) : juce::Colour::fromRGB (66, 184, 121);
        const auto trackBottom = bypassed ? juce::Colour::fromRGB (132, 40, 40) : juce::Colour::fromRGB (38, 123, 82);
        juce::ColourGradient trackGrad (trackTop, track.getX(), track.getY(), trackBottom, track.getX(), track.getBottom(), false);
        g.setGradientFill (trackGrad);
        g.fillRoundedRectangle (track, track.getHeight() * 0.5f);

        const float knobSize = track.getHeight() - 2.0f;
        const float knobX = bypassed ? (track.getRight() - knobSize - 1.0f) : (track.getX() + 1.0f);
        const float knobY = track.getY() + 1.0f;
        g.setColour (juce::Colour::fromRGB (248, 248, 250));
        g.fillEllipse (knobX, knobY, knobSize, knobSize);
    }
};

namespace
{
static void showInfoDialog (juce::Component* parent, const juce::String& title, const juce::String& text)
{
    juce::AlertWindow::showMessageBoxAsync (juce::MessageBoxIconType::InfoIcon, title, text, "Close", parent);
}
}

GuitarFXAudioProcessorEditor::GuitarFXAudioProcessorEditor (GuitarFXAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    lookAndFeel = std::make_unique<ProLookAndFeel>();
    setLookAndFeel (lookAndFeel.get());
    setSize (1220, 760);

    uiInDb.fill (-100.0f);
    uiOutDb.fill (-100.0f);
    uiWaveform.fill (0.0f);

    titleLabel.setText ("GuitarFX Version 2.1  |  Guitar Multi-Effects with Ten Band Equalizer", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    titleLabel.setFont (juce::Font (18.0f, juce::Font::bold));
    addAndMakeVisible (titleLabel);

    subtitleLabel.setText ("Equalizer is post effects chain and includes real-time waveform", juce::dontSendNotification);
    subtitleLabel.setJustificationType (juce::Justification::centredRight);
    subtitleLabel.setFont (juce::Font (12.0f, juce::Font::plain));
    addAndMakeVisible (subtitleLabel);

    setupBypassButton (gateBypassButton);
    setupBypassButton (compBypassButton);
    setupBypassButton (driveBypassButton);
    setupBypassButton (ampBypassButton);
    setupBypassButton (cabBypassButton);
    setupBypassButton (delayBypassButton);
    setupBypassButton (reverbBypassButton);
    setupBypassButton (eqBypassButton);

    auto setupInfoButton = [this] (juce::TextButton& button)
    {
        button.setButtonText ("i");
        addAndMakeVisible (button);
    };
    setupInfoButton (inputInfoButton); setupInfoButton (gateInfoButton); setupInfoButton (compInfoButton);
    setupInfoButton (driveInfoButton); setupInfoButton (ampInfoButton); setupInfoButton (cabInfoButton);
    setupInfoButton (delayInfoButton); setupInfoButton (reverbInfoButton); setupInfoButton (outputInfoButton);
    setupInfoButton (eqInfoButton);

    inputInfoButton.onClick = [this] { showInfoDialog (this, "Input Stage", "Input Stage prepares your guitar signal before effects."); };
    gateInfoButton.onClick = [this] { showInfoDialog (this, "Noise Gate", "Noise Gate reduces hiss and hum when you are not playing."); };
    compInfoButton.onClick = [this] { showInfoDialog (this, "Compressor", "Compressor evens out dynamics and sustains notes."); };
    driveInfoButton.onClick = [this] { showInfoDialog (this, "Drive", "Drive adds harmonic saturation and overdrive character."); };
    ampInfoButton.onClick = [this] { showInfoDialog (this, "Amplifier", "Amplifier stage adds amp-style gain and tone shaping."); };
    cabInfoButton.onClick = [this] { showInfoDialog (this, "Cabinet", "Cabinet stage shapes bandwidth to sound more like a guitar speaker."); };
    delayInfoButton.onClick = [this] { showInfoDialog (this, "Delay", "Delay creates repeating echoes behind your dry signal."); };
    reverbInfoButton.onClick = [this] { showInfoDialog (this, "Reverb", "Reverb adds space and room ambience."); };
    outputInfoButton.onClick = [this] { showInfoDialog (this, "Output Stage", "Output Stage sets final plugin level sent to the host."); };
    eqInfoButton.onClick = [this] { showInfoDialog (this, "Ten Band Equalizer", "Waveform shows equalizer input.\nSliders control ten EQ bands."); };

    setupKnob (inputGainKnob, inputGainLabel, "Input Gain");
    setupKnob (inputHpKnob, inputHpLabel, "Input High Pass");
    setupKnob (gateThresholdKnob, gateThresholdLabel, "Gate Threshold");
    setupKnob (gateRangeKnob, gateRangeLabel, "Gate Range");
    setupKnob (compThresholdKnob, compThresholdLabel, "Compressor Threshold");
    setupKnob (compRatioKnob, compRatioLabel, "Compressor Ratio");
    setupKnob (driveKnob, driveLabel, "Drive Amount");
    setupKnob (driveToneKnob, driveToneLabel, "Drive Tone");
    setupKnob (ampGainKnob, ampGainLabel, "Amplifier Gain");
    setupKnob (ampPresenceKnob, ampPresenceLabel, "Amplifier Presence");
    setupKnob (cabHighPassKnob, cabHighPassLabel, "Cabinet High Pass");
    setupKnob (cabLowPassKnob, cabLowPassLabel, "Cabinet Low Pass");
    setupKnob (delayTimeKnob, delayTimeLabel, "Delay Time");
    setupKnob (delayFeedbackKnob, delayFeedbackLabel, "Delay Feedback");
    setupKnob (reverbRoomKnob, reverbRoomLabel, "Reverb Room");
    setupKnob (reverbWetKnob, reverbWetLabel, "Reverb Wet");
    setupKnob (outputGainKnob, outputGainLabel, "Output Gain");

    for (int i = 0; i < GuitarFXAudioProcessor::eqNumBands; ++i)
    {
        eqBandSliders[static_cast<size_t> (i)] = std::make_unique<juce::Slider>();
        auto& s = *eqBandSliders[static_cast<size_t> (i)];
        s.setSliderStyle (juce::Slider::LinearVertical);
        s.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible (s);

        eqBandLabels[static_cast<size_t> (i)] = std::make_unique<juce::Label>();
        auto& l = *eqBandLabels[static_cast<size_t> (i)];
        const auto f = GuitarFXAudioProcessor::getEqBandFrequency (i);
        l.setText (f >= 1000.0f ? juce::String (f / 1000.0f, 1) + "k" : juce::String (f, 0), juce::dontSendNotification);
        l.setJustificationType (juce::Justification::centred);
        l.setFont (juce::Font (10.0f));
        addAndMakeVisible (l);
    }

    auto& apvts = processor.apvts;
    gateBypassAttachment = std::make_unique<ButtonAttachment> (apvts, "gateBypass", gateBypassButton);
    compBypassAttachment = std::make_unique<ButtonAttachment> (apvts, "compBypass", compBypassButton);
    driveBypassAttachment = std::make_unique<ButtonAttachment> (apvts, "driveBypass", driveBypassButton);
    ampBypassAttachment = std::make_unique<ButtonAttachment> (apvts, "ampBypass", ampBypassButton);
    cabBypassAttachment = std::make_unique<ButtonAttachment> (apvts, "cabBypass", cabBypassButton);
    delayBypassAttachment = std::make_unique<ButtonAttachment> (apvts, "delayBypass", delayBypassButton);
    reverbBypassAttachment = std::make_unique<ButtonAttachment> (apvts, "reverbBypass", reverbBypassButton);
    eqBypassAttachment = std::make_unique<ButtonAttachment> (apvts, "eqBypass", eqBypassButton);

    inputGainAttachment = std::make_unique<SliderAttachment> (apvts, "inputGainDb", inputGainKnob);
    inputHpAttachment = std::make_unique<SliderAttachment> (apvts, "inputHighPassHz", inputHpKnob);
    gateThresholdAttachment = std::make_unique<SliderAttachment> (apvts, "gateThresholdDb", gateThresholdKnob);
    gateRangeAttachment = std::make_unique<SliderAttachment> (apvts, "gateRangeDb", gateRangeKnob);
    compThresholdAttachment = std::make_unique<SliderAttachment> (apvts, "compThresholdDb", compThresholdKnob);
    compRatioAttachment = std::make_unique<SliderAttachment> (apvts, "compRatio", compRatioKnob);
    driveAttachment = std::make_unique<SliderAttachment> (apvts, "drive", driveKnob);
    driveToneAttachment = std::make_unique<SliderAttachment> (apvts, "driveToneHz", driveToneKnob);
    ampGainAttachment = std::make_unique<SliderAttachment> (apvts, "ampGain", ampGainKnob);
    ampPresenceAttachment = std::make_unique<SliderAttachment> (apvts, "ampPresence", ampPresenceKnob);
    cabHighPassAttachment = std::make_unique<SliderAttachment> (apvts, "cabHighPassHz", cabHighPassKnob);
    cabLowPassAttachment = std::make_unique<SliderAttachment> (apvts, "cabLowPassHz", cabLowPassKnob);
    delayTimeAttachment = std::make_unique<SliderAttachment> (apvts, "delayTimeMs", delayTimeKnob);
    delayFeedbackAttachment = std::make_unique<SliderAttachment> (apvts, "delayFeedback", delayFeedbackKnob);
    reverbRoomAttachment = std::make_unique<SliderAttachment> (apvts, "reverbRoom", reverbRoomKnob);
    reverbWetAttachment = std::make_unique<SliderAttachment> (apvts, "reverbWet", reverbWetKnob);
    outputGainAttachment = std::make_unique<SliderAttachment> (apvts, "outputGainDb", outputGainKnob);
    for (int i = 0; i < GuitarFXAudioProcessor::eqNumBands; ++i)
        eqBandAttachments[static_cast<size_t> (i)] = std::make_unique<SliderAttachment> (apvts, "eqBand" + juce::String (i), *eqBandSliders[static_cast<size_t> (i)]);

    // setSize() can trigger resized() before EQ sliders are created; force a final layout pass now.
    resized();
    createBackgroundImage();
    startTimerHz (30);
}

GuitarFXAudioProcessorEditor::~GuitarFXAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void GuitarFXAudioProcessorEditor::setupKnob (juce::Slider& knob, juce::Label& label, const juce::String& text)
{
    knob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    knob.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 78, 16);
    addAndMakeVisible (knob);
    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setFont (juce::Font (12.0f, juce::Font::bold));
    addAndMakeVisible (label);
}

void GuitarFXAudioProcessorEditor::setupBypassButton (juce::ToggleButton& button)
{
    button.setButtonText ("Enabled");
    addAndMakeVisible (button);
}

void GuitarFXAudioProcessorEditor::createBackgroundImage()
{
    backgroundImage = juce::Image (juce::Image::RGB, juce::jmax (1, getWidth()), juce::jmax (1, getHeight()), true);
    juce::Graphics g (backgroundImage);
    juce::ColourGradient base (juce::Colour::fromRGB (9, 12, 18), 0.0f, 0.0f,
                               juce::Colour::fromRGB (32, 20, 14), static_cast<float> (getWidth()), static_cast<float> (getHeight()), false);
    base.addColour (0.5, juce::Colour::fromRGB (19, 26, 35));
    g.setGradientFill (base);
    g.fillAll();
}

void GuitarFXAudioProcessorEditor::drawMeter (juce::Graphics& g, const juce::Rectangle<int>& area, float db, bool outputMeter)
{
    g.setColour (juce::Colour::fromRGBA (8, 10, 13, 220));
    g.fillRoundedRectangle (area.toFloat(), 2.8f);
    auto fill = area.reduced (2);
    const float norm = juce::jlimit (0.0f, 1.0f, juce::jmap (db, -60.0f, 3.0f, 0.0f, 1.0f));
    const int h = static_cast<int> (fill.getHeight() * norm);
    auto lit = fill.removeFromBottom (h);
    g.setColour (outputMeter ? juce::Colour::fromRGB (58, 216, 140) : juce::Colour::fromRGB (72, 192, 255));
    g.fillRoundedRectangle (lit.toFloat(), 2.0f);
}

void GuitarFXAudioProcessorEditor::drawMeterPair (juce::Graphics& g, const juce::Rectangle<int>& inArea, const juce::Rectangle<int>& outArea, int stage)
{
    drawMeter (g, inArea, uiInDb[static_cast<size_t> (stage)], false);
    drawMeter (g, outArea, uiOutDb[static_cast<size_t> (stage)], true);
}

void GuitarFXAudioProcessorEditor::drawWaveform (juce::Graphics& g, const juce::Rectangle<int>& area)
{
    g.setColour (juce::Colour::fromRGBA (8, 10, 13, 220));
    g.fillRoundedRectangle (area.toFloat(), 5.0f);
    g.setColour (juce::Colour::fromRGBA (224, 162, 95, 140));
    g.drawRoundedRectangle (area.toFloat(), 5.0f, 1.0f);

    auto inner = area.reduced (4);
    g.setColour (juce::Colour::fromRGBA (215, 220, 230, 35));
    for (int i = 1; i < 8; ++i)
    {
        const int x = inner.getX() + (inner.getWidth() * i) / 8;
        g.drawVerticalLine (x, static_cast<float> (inner.getY()), static_cast<float> (inner.getBottom()));
    }
    for (int i = 1; i < 4; ++i)
    {
        const int y = inner.getY() + (inner.getHeight() * i) / 4;
        g.drawHorizontalLine (y, static_cast<float> (inner.getX()), static_cast<float> (inner.getRight()));
    }

    const float midY = static_cast<float> (area.getCentreY());
    juce::Path wave;
    for (int i = 0; i < GuitarFXAudioProcessor::waveformSize; ++i)
    {
        const float x = static_cast<float> (area.getX() + 4) + (static_cast<float> (i) / static_cast<float> (GuitarFXAudioProcessor::waveformSize - 1)) * static_cast<float> (area.getWidth() - 8);
        const float y = midY - uiWaveform[static_cast<size_t> (i)] * static_cast<float> (area.getHeight() * 0.42f);
        if (i == 0) wave.startNewSubPath (x, y); else wave.lineTo (x, y);
    }

    juce::Path fill;
    fill.addPath (wave);
    fill.lineTo (static_cast<float> (area.getRight() - 4), midY);
    fill.lineTo (static_cast<float> (area.getX() + 4), midY);
    fill.closeSubPath();
    g.setColour (juce::Colour::fromRGBA (90, 180, 255, 55));
    g.fillPath (fill);

    g.setColour (juce::Colour::fromRGBA (110, 216, 255, 70));
    g.strokePath (wave, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    g.setColour (juce::Colour::fromRGB (145, 228, 255));
    g.strokePath (wave, juce::PathStrokeType (1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void GuitarFXAudioProcessorEditor::timerCallback()
{
    const auto setBypassText = [] (juce::ToggleButton& b) { b.setButtonText (b.getToggleState() ? "Bypassed" : "Enabled"); };
    setBypassText (gateBypassButton); setBypassText (compBypassButton); setBypassText (driveBypassButton);
    setBypassText (ampBypassButton); setBypassText (cabBypassButton); setBypassText (delayBypassButton);
    setBypassText (reverbBypassButton); setBypassText (eqBypassButton);

    for (int stage = 0; stage < GuitarFXAudioProcessor::stageCount; ++stage)
    {
        uiInDb[static_cast<size_t> (stage)] = 0.82f * uiInDb[static_cast<size_t> (stage)] + 0.18f * processor.getStageInputMeterDb (stage);
        uiOutDb[static_cast<size_t> (stage)] = 0.82f * uiOutDb[static_cast<size_t> (stage)] + 0.18f * processor.getStageOutputMeterDb (stage);
    }

    std::array<float, GuitarFXAudioProcessor::waveformSize> wave {};
    processor.copyEqInputWaveform (wave);
    for (int i = 0; i < GuitarFXAudioProcessor::waveformSize; ++i)
        uiWaveform[static_cast<size_t> (i)] = 0.35f * uiWaveform[static_cast<size_t> (i)] + 0.65f * wave[static_cast<size_t> (i)];

    repaint();
}

void GuitarFXAudioProcessorEditor::paint (juce::Graphics& g)
{
    if (backgroundImage.getWidth() != getWidth() || backgroundImage.getHeight() != getHeight())
        createBackgroundImage();
    g.drawImageAt (backgroundImage, 0, 0);

    const auto drawCard = [&g] (const juce::Rectangle<int>& r, const juce::String& title, bool bypassed)
    {
        auto card = r.toFloat();
        const auto bgTop = bypassed ? juce::Colour::fromRGB (44, 46, 52) : juce::Colour::fromRGB (30, 36, 44);
        const auto bgBottom = bypassed ? juce::Colour::fromRGB (22, 24, 28) : juce::Colour::fromRGB (13, 16, 20);
        g.setColour (juce::Colour::fromRGBA (0, 0, 0, 70));
        g.fillRoundedRectangle (card.translated (0.0f, 2.5f), 10.0f);
        juce::ColourGradient bg (bgTop, card.getX(), card.getY(), bgBottom, card.getX(), card.getBottom(), false);
        g.setGradientFill (bg);
        g.fillRoundedRectangle (card, 10.0f);
        g.setColour (juce::Colour::fromRGBA (234, 167, 98, 178));
        g.drawRoundedRectangle (card, 10.0f, 1.15f);
        g.setColour (juce::Colour::fromRGB (246, 225, 194));
        g.setFont (juce::Font (13.0f, juce::Font::bold));
        g.drawText (title, r.withTrimmedTop (7).withHeight (18), juce::Justification::centredTop, false);
    };

    drawCard (inputCard, "Input Stage", false);
    drawCard (gateCard, "Noise Gate", gateBypassButton.getToggleState());
    drawCard (compCard, "Compressor", compBypassButton.getToggleState());
    drawCard (driveCard, "Drive", driveBypassButton.getToggleState());
    drawCard (ampCard, "Amplifier", ampBypassButton.getToggleState());
    drawCard (cabCard, "Cabinet", cabBypassButton.getToggleState());
    drawCard (delayCard, "Delay", delayBypassButton.getToggleState());
    drawCard (reverbCard, "Reverb", reverbBypassButton.getToggleState());
    drawCard (outputCard, "Output Stage", false);
    drawCard (eqSection, "Ten Band Equalizer", eqBypassButton.getToggleState());

    for (int s = 0; s < GuitarFXAudioProcessor::stageCount; ++s)
        drawMeterPair (g, meterInAreas[static_cast<size_t> (s)], meterOutAreas[static_cast<size_t> (s)], s);

    drawWaveform (g, eqWaveformArea);
}

void GuitarFXAudioProcessorEditor::resized()
{
    titleLabel.setBounds (16, 10, 620, 24);
    subtitleLabel.setBounds (640, 12, getWidth() - 656, 20);

    auto content = getLocalBounds().reduced (14, 54);
    eqSection = content.removeFromBottom (250);
    content.removeFromBottom (12);

    constexpr int gap = 12;
    constexpr int rowGap = 12;
    auto top = content.removeFromTop ((content.getHeight() - rowGap) / 2);
    content.removeFromTop (rowGap);
    auto bottom = content;

    const int topW = (top.getWidth() - gap * 4) / 5;
    inputCard = top.removeFromLeft (topW); top.removeFromLeft (gap);
    gateCard = top.removeFromLeft (topW); top.removeFromLeft (gap);
    compCard = top.removeFromLeft (topW); top.removeFromLeft (gap);
    driveCard = top.removeFromLeft (topW); top.removeFromLeft (gap);
    ampCard = top;

    const int bottomW = (bottom.getWidth() - gap * 3) / 4;
    cabCard = bottom.removeFromLeft (bottomW); bottom.removeFromLeft (gap);
    delayCard = bottom.removeFromLeft (bottomW); bottom.removeFromLeft (gap);
    reverbCard = bottom.removeFromLeft (bottomW); bottom.removeFromLeft (gap);
    outputCard = bottom;

    const auto placeTwoKnobCard = [] (juce::Rectangle<int> r, juce::ToggleButton* bypass, juce::TextButton& info,
                                      juce::Slider& a, juce::Label& al, juce::Slider& b, juce::Label& bl,
                                      juce::Rectangle<int>& inM, juce::Rectangle<int>& outM)
    {
        auto inner = r.reduced (8);
        inner.removeFromTop (22);
        auto topBar = inner.removeFromTop (24);
        if (bypass != nullptr)
            bypass->setBounds (topBar.withSizeKeepingCentre (140, topBar.getHeight()));
        info.setBounds (r.getRight() - 28, r.getY() + 7, 18, 18);

        inM = inner.removeFromLeft (9).withTrimmedTop (4).withTrimmedBottom (19);
        inner.removeFromLeft (4);
        outM = inner.removeFromRight (9).withTrimmedTop (4).withTrimmedBottom (19);
        inner.removeFromRight (4);

        auto left = inner.removeFromLeft ((inner.getWidth() - 6) / 2);
        inner.removeFromLeft (6);
        auto right = inner;

        auto k1 = left.withSizeKeepingCentre (76, 94);
        k1.setY (left.getY() + 6);
        a.setBounds (k1);
        al.setBounds (k1.getX() - 4, k1.getBottom() + 1, k1.getWidth() + 8, 16);

        auto k2 = right.withSizeKeepingCentre (76, 94);
        k2.setY (right.getY() + 6);
        b.setBounds (k2);
        bl.setBounds (k2.getX() - 4, k2.getBottom() + 1, k2.getWidth() + 8, 16);
    };

    placeTwoKnobCard (inputCard, nullptr, inputInfoButton, inputGainKnob, inputGainLabel, inputHpKnob, inputHpLabel,
                      meterInAreas[GuitarFXAudioProcessor::stageInput], meterOutAreas[GuitarFXAudioProcessor::stageInput]);
    placeTwoKnobCard (gateCard, &gateBypassButton, gateInfoButton, gateThresholdKnob, gateThresholdLabel, gateRangeKnob, gateRangeLabel,
                      meterInAreas[GuitarFXAudioProcessor::stageGate], meterOutAreas[GuitarFXAudioProcessor::stageGate]);
    placeTwoKnobCard (compCard, &compBypassButton, compInfoButton, compThresholdKnob, compThresholdLabel, compRatioKnob, compRatioLabel,
                      meterInAreas[GuitarFXAudioProcessor::stageComp], meterOutAreas[GuitarFXAudioProcessor::stageComp]);
    placeTwoKnobCard (driveCard, &driveBypassButton, driveInfoButton, driveKnob, driveLabel, driveToneKnob, driveToneLabel,
                      meterInAreas[GuitarFXAudioProcessor::stageDrive], meterOutAreas[GuitarFXAudioProcessor::stageDrive]);
    placeTwoKnobCard (ampCard, &ampBypassButton, ampInfoButton, ampGainKnob, ampGainLabel, ampPresenceKnob, ampPresenceLabel,
                      meterInAreas[GuitarFXAudioProcessor::stageAmp], meterOutAreas[GuitarFXAudioProcessor::stageAmp]);
    placeTwoKnobCard (cabCard, &cabBypassButton, cabInfoButton, cabHighPassKnob, cabHighPassLabel, cabLowPassKnob, cabLowPassLabel,
                      meterInAreas[GuitarFXAudioProcessor::stageCab], meterOutAreas[GuitarFXAudioProcessor::stageCab]);
    placeTwoKnobCard (delayCard, &delayBypassButton, delayInfoButton, delayTimeKnob, delayTimeLabel, delayFeedbackKnob, delayFeedbackLabel,
                      meterInAreas[GuitarFXAudioProcessor::stageDelay], meterOutAreas[GuitarFXAudioProcessor::stageDelay]);
    placeTwoKnobCard (reverbCard, &reverbBypassButton, reverbInfoButton, reverbRoomKnob, reverbRoomLabel, reverbWetKnob, reverbWetLabel,
                      meterInAreas[GuitarFXAudioProcessor::stageReverb], meterOutAreas[GuitarFXAudioProcessor::stageReverb]);

    {
        auto inner = outputCard.reduced (8);
        inner.removeFromTop (22);
        inner.removeFromTop (24);
        outputInfoButton.setBounds (outputCard.getRight() - 28, outputCard.getY() + 7, 18, 18);
        meterInAreas[GuitarFXAudioProcessor::stageOutput] = inner.removeFromLeft (9).withTrimmedTop (4).withTrimmedBottom (19);
        inner.removeFromLeft (4);
        meterOutAreas[GuitarFXAudioProcessor::stageOutput] = inner.removeFromRight (9).withTrimmedTop (4).withTrimmedBottom (19);
        inner.removeFromRight (10);
        auto k = inner.withSizeKeepingCentre (78, 96);
        k.setY (inner.getY() + 8);
        outputGainKnob.setBounds (k);
        outputGainLabel.setBounds (k.getX() - 4, k.getBottom() + 1, k.getWidth() + 8, 16);
    }

    {
        auto inner = eqSection.reduced (10);
        inner.removeFromTop (22);
        auto topBar = inner.removeFromTop (24);
        eqBypassButton.setBounds (topBar.withSizeKeepingCentre (140, topBar.getHeight()));
        eqInfoButton.setBounds (eqSection.getRight() - 28, eqSection.getY() + 7, 18, 18);

        eqWaveformArea = inner.removeFromTop (88).reduced (4, 2);
        inner.removeFromTop (6);
        eqSlidersArea = inner;

        const int count = GuitarFXAudioProcessor::eqNumBands;
        const int gapPx = 4;
        const int colW = juce::jmax (20, (eqSlidersArea.getWidth() - (count - 1) * gapPx) / count);
        int x = eqSlidersArea.getX();
        for (int i = 0; i < count; ++i)
        {
            auto col = juce::Rectangle<int> (x, eqSlidersArea.getY(), colW, eqSlidersArea.getHeight());
            auto label = col.removeFromBottom (16);
            if (eqBandSliders[static_cast<size_t> (i)] != nullptr)
                eqBandSliders[static_cast<size_t> (i)]->setBounds (col);
            if (eqBandLabels[static_cast<size_t> (i)] != nullptr)
                eqBandLabels[static_cast<size_t> (i)]->setBounds (label);
            x += colW + gapPx;
        }
    }
}
