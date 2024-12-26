/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class Waylochorus2AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Waylochorus2AudioProcessor();
    ~Waylochorus2AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;
    
    float linInterp(float sample_x, float sample_x1, float inPhase);

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Waylochorus2AudioProcessor)
    float mDelayTimeInSamples;
        float mDelayReadHead;
        float feedback;
        float mLFOphase;
        float mLFOrate;
        float mPanOne;
        
        float mDelayTwoTimeInSamples;
        float mDelayTwoReadHead;
        float feedbackTwo;
        float mLFOphaseTwo;
        float mLFOrateTwo;
        float mPanTwo;
        
        float mDelayThreeTimeInSamples;
        float mDelayThreeReadHead;
        float feedbackThree;
        float mLFOphaseThree;
        float mLFOrateThree;
        float mPanThree;
        
        float mDelayFourTimeInSamples;
        float mDelayFourReadHead;
        float feedbackFour;
        float mLFOphaseFour;
        float mLFOrateFour;
        float mPanFour;
        
        
        float mCircularBufferLeft[96000] = {  };
        float mCircularBufferRight[96000] = {  };
        float mCircularBufferLeftTwo[96000] = {  };
        float mCircularBufferRightTwo[96000] = {  };
        float mCircularBufferLeftThree[96000] = {  };
        float mCircularBufferRightThree[96000] = {  };
        float mCircularBufferLeftFour[96000] = {  };
        float mCircularBufferRightFour[96000] = {  };

        float mLastInputGain  = 0.0f;
        float mDelayTimeSmoothed;
        
        float arr2[10] = { 0 };
        
        int mCircularBufferWriteHead;
        int mCircularBufferLength;
        
        float mLastInputGainTwo  = 0.0f;
        float mDelayTimeSmoothedTwo;
        
        int mCircularBufferWriteHeadTwo;
        
        float mLastInputGainThree  = 0.0f;
        float mDelayTimeSmoothedThree;
        
        int mCircularBufferWriteHeadThree;
        
        
        float mLastInputGainFour  = 0.0f;
        float mDelayTimeSmoothedFour;

        int mCircularBufferWriteHeadFour;
        
        float panLevelOne;
        float panLevelTwo;
        float panLevelThree;
        float panLevelFour;
        float panOneL;
        float panOneR;
        float panTwoL;
        float panTwoR;
        float panThreeL;
        float panThreeR;
        float panFourL;
        float panFourR;
    


};
