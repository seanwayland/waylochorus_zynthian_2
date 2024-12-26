/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Waylochorus2AudioProcessor::Waylochorus2AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    
    // initialize buffer values
    mCircularBufferWriteHead = mCircularBufferWriteHeadTwo =  mCircularBufferWriteHeadThree = mCircularBufferWriteHeadFour = 0;
    
    mDelayTimeInSamples = mDelayTwoTimeInSamples = mDelayThreeTimeInSamples = mDelayFourTimeInSamples = 0.0;
    
    mDelayReadHead = mDelayTwoReadHead = mDelayThreeReadHead = mDelayFourReadHead =  0.0;
    
    mLFOphase = mLFOphaseTwo = mLFOphaseThree = mLFOphaseFour = 0;
    
    mLFOrate = mLFOrateTwo= mLFOrateThree = mLFOrateFour  = 0.3f;
}

Waylochorus2AudioProcessor::~Waylochorus2AudioProcessor()
{
}

//==============================================================================
const juce::String Waylochorus2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

float Waylochorus2AudioProcessor::linInterp(float sample_x, float sample_x1, float inPhase)
{
    return (1 - inPhase) * sample_x + inPhase * sample_x1;
    
}

bool Waylochorus2AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Waylochorus2AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Waylochorus2AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Waylochorus2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Waylochorus2AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Waylochorus2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Waylochorus2AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Waylochorus2AudioProcessor::getProgramName (int index)
{
    return {};
}

void Waylochorus2AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
//==============================================================================
void Waylochorus2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    
    
    mLFOphase = mLFOphaseTwo = mLFOphaseThree = mLFOphaseFour  = 0;
    
    mLFOrateTwo = mLFOrateTwo = mLFOrateThree = mLFOrateFour =  0.3f  ;
    
    
    
    
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    // mCircularBufferLength = sampleRate*MAX_DELAY_TIME;
    mCircularBufferLength = 96000;
    mCircularBufferWriteHead = 0;
    mCircularBufferWriteHeadTwo = 0;
    mCircularBufferWriteHeadThree = 0;
    mCircularBufferWriteHeadFour = 0;

    
    
    mDelayReadHead = 0.0;
    mDelayTwoReadHead = 0.0;
    mDelayThreeReadHead = 0.0;
    mDelayFourReadHead =0.0;

    
    
    
    
}

void Waylochorus2AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Waylochorus2AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Waylochorus2AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    float* LeftChannel;
    float* RightChannel;
    
    if (totalNumInputChannels >1){
        LeftChannel = buffer.getWritePointer(0);
        RightChannel = buffer.getWritePointer(0);
    }
    else
    {
        LeftChannel = buffer.getWritePointer(0);
        RightChannel = buffer.getWritePointer(1);
        
    }

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int i = 0; i < buffer.getNumSamples(); ++i)
       {

           float lfoOut = sin(2*M_PI * mLFOphase);
           float lfoOutTwo = sin(2*M_PI * mLFOphaseTwo);
           float lfoOutThree = sin(2*M_PI * mLFOphaseThree);
           float lfoOutFour = sin(2*M_PI * mLFOphaseFour);

        // ### NUMBERS HERE
           mLFOphase += 0.65/getSampleRate();
           mLFOphaseTwo += 0.57/getSampleRate();
           mLFOphaseThree += 0.48/getSampleRate();
           mLFOphaseFour += 0.44/getSampleRate();
           
           // LFO phase is moving between zero and one
            if ( mLFOphase > 1){
                mLFOphase -= 1;
            }
            if ( mLFOphaseTwo > 1){
                mLFOphaseTwo -= 1;
            }
            
            if ( mLFOphaseThree > 1){
                mLFOphaseThree -= 1;
            }
            
            if ( mLFOphaseFour > 1){
                mLFOphaseFour -= 1;
            }
           
           float lfoOutMapped = juce::jmap(lfoOut,-1.f,1.f,0.001f, 0.1f);
           float lfoOutMappedTwo = juce::jmap(lfoOutTwo,-1.f,1.f,0.001f, 0.1f);
           float lfoOutMappedThree = juce::jmap(lfoOutThree,-1.f,1.f,0.001f, 0.1f);
           float lfoOutMappedFour = juce::jmap(lfoOutFour,-1.f,1.f,0.001f,0.1f);
           
           int dtime = static_cast<int>(0.0236*getSampleRate());
           int dtimeTwo = static_cast<int>(0.03*getSampleRate());
           int dtimeThree = static_cast<int>(0.036*getSampleRate());
           int dtimeFour = static_cast<int>(0.028*getSampleRate());
           
           /// add the modulated delay time to the value the delay time is set to with the slider
           
           mDelayTimeInSamples = dtime*(1+ lfoOutMapped) ;
           mDelayTwoTimeInSamples = dtimeTwo*(1+ lfoOutMappedTwo);
           mDelayThreeTimeInSamples = dtimeThree*(1+ lfoOutMappedThree);
           mDelayFourTimeInSamples = dtimeFour*(1+ lfoOutMappedFour) ;
           
           // shove some of the input into the circular buffer also add some of the feedback
           mCircularBufferLeft[mCircularBufferWriteHead] = LeftChannel[i] ;
           mCircularBufferRight[mCircularBufferWriteHead] = RightChannel[i] ;
           mCircularBufferLeftTwo[mCircularBufferWriteHeadTwo] = LeftChannel[i] ;
           mCircularBufferRightTwo[mCircularBufferWriteHeadTwo] = RightChannel[i] ;
           mCircularBufferLeftThree[mCircularBufferWriteHeadThree] = LeftChannel[i] ;
           mCircularBufferRightThree[mCircularBufferWriteHeadThree] = RightChannel[i] ;
           mCircularBufferLeftFour[mCircularBufferWriteHeadFour] = LeftChannel[i] ;
           mCircularBufferRightFour[mCircularBufferWriteHeadFour] = RightChannel[i] ;
           
           // move the read head on the circular to the new delay position
           mDelayReadHead = mCircularBufferWriteHead - mDelayTimeInSamples;
           mDelayTwoReadHead = mCircularBufferWriteHeadTwo - mDelayTwoTimeInSamples;
           mDelayThreeReadHead = mCircularBufferWriteHeadThree - mDelayThreeTimeInSamples;
           mDelayFourReadHead = mCircularBufferWriteHeadFour - mDelayFourTimeInSamples;
           
           // if read head is below zero wrap around
           if (mDelayReadHead < 0){
               mDelayReadHead += mCircularBufferLength;
           }
           
           if (mDelayTwoReadHead < 0){
               mDelayTwoReadHead += mCircularBufferLength;
           }
           if (mDelayThreeReadHead < 0){
               mDelayThreeReadHead += mCircularBufferLength;
           }
           if (mDelayFourReadHead < 0){
               mDelayFourReadHead += mCircularBufferLength;
           }
           
           // get the integer part of the read head
            int readHeadX = (int)mDelayReadHead;
            // get the part of the readHead after the decimal point
            float readHeadFloat = mDelayReadHead - readHeadX;
            // next integer sample position
            int readHeadX1 = readHeadX + 1;
            
            // get the integer part of the read head
            int readHeadXTwo = (int)mDelayTwoReadHead;
            // get the part of the readHead after the decimal point
            float readHeadFloatTwo = mDelayTwoReadHead - readHeadXTwo;
            // next integer sample position
            int readHeadX1Two = readHeadXTwo + 1;
            
            // get the integer part of the read head
            int readHeadXThree = (int)mDelayThreeReadHead;
            // get the part of the readHead after the decimal point
            float readHeadFloatThree = mDelayThreeReadHead - readHeadXThree;
            // next integer sample position
            int readHeadX1Three = readHeadXThree + 1;
            
            // get the integer part of the read head
            int readHeadXFour = (int)mDelayFourReadHead;
            // get the part of the readHead after the decimal point
            float readHeadFloatFour = mDelayFourReadHead - readHeadXFour;
            // next integer sample position
            int readHeadX1Four = readHeadXFour + 1;
           
           
           
           // if next sample position is outside the buffer
           if ( readHeadX1 >= mCircularBufferLength){
               readHeadX1 -= mCircularBufferLength;
           }
           
           if ( readHeadX1Two >= mCircularBufferLength){
               readHeadX1Two -= mCircularBufferLength;
           }
           
           if ( readHeadX1Three >= mCircularBufferLength){
               readHeadX1Three -= mCircularBufferLength;
           }
           
           if ( readHeadX1Four >= mCircularBufferLength){
               readHeadX1Four -= mCircularBufferLength;
           }
           
           // get the interpolated value of the delayed sample from the circular buffer
           float delay_sample_Left = Waylochorus2AudioProcessor::linInterp(mCircularBufferLeft[readHeadX], mCircularBufferLeft[readHeadX1], readHeadFloat);
           float delay_sample_Right = Waylochorus2AudioProcessor::linInterp(mCircularBufferRight[readHeadX], mCircularBufferRight[readHeadX1], readHeadFloat);
           float delay_sample_LeftTwo = Waylochorus2AudioProcessor::linInterp(mCircularBufferLeftTwo[readHeadXTwo], mCircularBufferLeftTwo[readHeadX1Two], readHeadFloatTwo);
           float delay_sample_RightTwo = Waylochorus2AudioProcessor::linInterp(mCircularBufferRightTwo[readHeadXTwo], mCircularBufferRightTwo[readHeadX1Two], readHeadFloatTwo);
           float delay_sample_LeftThree = Waylochorus2AudioProcessor::linInterp(mCircularBufferLeftThree[readHeadXThree], mCircularBufferLeftThree[readHeadX1Three], readHeadFloatThree);
           float delay_sample_RightThree = Waylochorus2AudioProcessor::linInterp(mCircularBufferRightThree[readHeadXThree], mCircularBufferRightThree[readHeadX1Three], readHeadFloatThree);
           float delay_sample_LeftFour = Waylochorus2AudioProcessor::linInterp(mCircularBufferLeftFour[readHeadXFour], mCircularBufferLeftFour[readHeadX1Four], readHeadFloatFour);
           float delay_sample_RightFour = Waylochorus2AudioProcessor::linInterp(mCircularBufferRightFour[readHeadXFour], mCircularBufferRightFour[readHeadX1Four], readHeadFloatFour);
           
           
           panLevelOne = 0.0f;
           panOneL = 1 - panLevelOne;
           panOneR = panLevelOne;
           panLevelTwo = 1.0f;
           panTwoL = 1 - panLevelTwo;
           panTwoR = panLevelTwo;
           panLevelThree = 0.0f;
           panThreeL = 1 - panLevelThree;
           panThreeR = panLevelThree;
           panLevelFour = 1.0f;
           panFourL = 1 - panLevelFour;
           panFourR = panLevelFour;
           
           
           buffer.setSample(0, i,
                            
                            
                            //1.0, 1.0,0.7,0.57
                            
                            delay_sample_Left
                            +delay_sample_LeftTwo
                            +delay_sample_LeftThree*0.7
                            +delay_sample_LeftFour*0.57

                            
                            );
           
           buffer.setSample(1, i,
                            
                            

                            + delay_sample_Right
                            + delay_sample_RightThree*0.7
                            + delay_sample_RightTwo
                            + delay_sample_RightFour*0.57
                         
                        
            
                            );
        
           
           // increment the buffer write head
           mCircularBufferWriteHead ++;
           mCircularBufferWriteHeadTwo ++;
           mCircularBufferWriteHeadThree ++;
           mCircularBufferWriteHeadFour ++;

           
           
           // wrap around if needed
           if (mCircularBufferWriteHead == mCircularBufferLength){
               mCircularBufferWriteHead = 0;
           }
           if (mCircularBufferWriteHeadTwo == mCircularBufferLength){
               mCircularBufferWriteHeadTwo = 0;
           }
           if (mCircularBufferWriteHeadThree == mCircularBufferLength){
               mCircularBufferWriteHeadThree = 0;
           }
           if (mCircularBufferWriteHeadFour == mCircularBufferLength){
               mCircularBufferWriteHeadFour = 0;
           }

           

    }
}

//==============================================================================
bool Waylochorus2AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Waylochorus2AudioProcessor::createEditor()
{
    return new Waylochorus2AudioProcessorEditor (*this);
}

//==============================================================================
void Waylochorus2AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Waylochorus2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Waylochorus2AudioProcessor();
}
