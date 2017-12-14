//TEENSYNTH BUILD 1.0
//CODY MARTINEZ

//for SYNTH
#include <Audio.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Encoder.h>
#include <i2c_t3.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define NOTE_ON B10000000
#define NOTE_OFF B00000000

//begin OLED
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000
};

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
//end OLED


// teensy audio library
AudioMixer4              preMixer1;
AudioMixer4              preMixer2;
AudioMixer4              masterMixer;
AudioOutputAnalogStereo  dacs1;

AudioSynthWaveform       voice1a;
AudioSynthWaveform       voice1b;
AudioMixer4              voice1Mixer;
AudioEffectEnvelope      ampEnv1;
AudioEffectEnvelope      filterEnv1;
AudioFilterStateVariable filter1;
AudioConnection          voice1Patch1(voice1a, 0, voice1Mixer, 0);
AudioConnection          voice1Patch2(voice1b, 0, voice1Mixer, 1);
AudioConnection          voice1Patch3(voice1Mixer, 0, ampEnv1, 0);
AudioConnection          voice1Patch4(voice1Mixer, 0, filterEnv1, 0);
AudioConnection          voice1Patch5(ampEnv1, 0, filter1, 0);
AudioConnection          voice1Patch6(filterEnv1, 0, filter1, 1);
AudioConnection          voice1Patch7(filter1, 0, preMixer1, 0);


AudioSynthWaveform       voice2a;
AudioSynthWaveform       voice2b;
AudioMixer4              voice2Mixer;
AudioEffectEnvelope      ampEnv2;
AudioEffectEnvelope      filterEnv2;
AudioFilterStateVariable filter2;
AudioConnection          voice2Patch1(voice2a, 0, voice2Mixer, 0);
AudioConnection          voice2Patch2(voice2b, 0, voice2Mixer, 1);
AudioConnection          voice2Patch3(voice2Mixer, 0,  ampEnv2, 0);
AudioConnection          voice2Patch4(voice2Mixer, 0, filterEnv2, 0);
AudioConnection          voice2Patch5(ampEnv2, 0, filter2, 0);
AudioConnection          voice2Patch6(filterEnv2, 0, filter2, 1);
AudioConnection          voice2Patch7(filter2, 0, preMixer1, 1);

AudioSynthWaveform       voice3a;
AudioSynthWaveform       voice3b;
AudioMixer4              voice3Mixer;
AudioEffectEnvelope      ampEnv3;
AudioEffectEnvelope      filterEnv3;
AudioFilterStateVariable filter3;
AudioConnection          voice3Patch1(voice3a, 0, voice3Mixer, 0);
AudioConnection          voice3Patch2(voice3b, 0, voice3Mixer, 1);
AudioConnection          voice3Patch3(voice3Mixer, ampEnv3);
AudioConnection          voice3Patch4(voice3Mixer, filterEnv3);
AudioConnection          voice3Patch5(ampEnv3, 0, filter3, 0);
AudioConnection          voice3Patch6(filterEnv3, 0, filter3, 1);
AudioConnection          voice3Patch7(filter3, 0, preMixer1, 2);

AudioSynthWaveform       voice4a;
AudioSynthWaveform       voice4b;
AudioMixer4              voice4Mixer;
AudioEffectEnvelope      ampEnv4;
AudioEffectEnvelope      filterEnv4;
AudioFilterStateVariable filter4;
AudioConnection          voice4Patch1(voice4a, 0, voice4Mixer, 0);
AudioConnection          voice4Patch2(voice4b, 0, voice4Mixer, 1);
AudioConnection          voice4Patch3(voice4Mixer, ampEnv4);
AudioConnection          voice4Patch4(voice4Mixer, filterEnv4);
AudioConnection          voice4Patch5(ampEnv4, 0, filter4, 0);
AudioConnection          voice4Patch6(filterEnv4, 0, filter4, 1);
AudioConnection          voice4Patch7(filter4, 0, preMixer1, 3);

AudioSynthWaveform       voice5a;
AudioSynthWaveform       voice5b;
AudioMixer4              voice5Mixer;
AudioEffectEnvelope      ampEnv5;
AudioEffectEnvelope      filterEnv5;
AudioFilterStateVariable filter5;
AudioConnection          voice5Patch1(voice5a, 0, voice5Mixer, 0);
AudioConnection          voice5Patch2(voice5b, 0, voice5Mixer, 1);
AudioConnection          voice5Patch3(voice5Mixer, ampEnv5);
AudioConnection          voice5Patch4(voice5Mixer, filterEnv5);
AudioConnection          voice5Patch5(ampEnv5, 0, filter5, 0);
AudioConnection          voice5Patch6(filterEnv5, 0, filter5, 1);
AudioConnection          voice5Patch7(filter5, 0, preMixer2, 0);

AudioConnection          preMix1Patch(preMixer1, 0, masterMixer, 0);
AudioConnection          preMix2Patch(preMixer2, 0, masterMixer, 1);
AudioConnection          masterOutPatchL(masterMixer, 0, dacs1, 0);


//LFO (NOT USED IN THIS BUILD)
//AudioSynthWaveform       lfoPitch;
//AudioSynthWaveform       lfoCutoff;
//AudioSynthWaveform       lfoFrequency;
//AudioSynthWaveform       lfoVolume;
//AudioMixer4              lfoMixer;
//AudioAnalyzePeak         lfoPeak;
//AudioConnection          patchCord1(lfo, lfoMixer);
//AudioConnection          patchCord3(lfoMixer, lfoPeak);

//all voices (polyphony handler)
AudioSynthWaveform * v1a = &voice1a;
AudioSynthWaveform * v1b = &voice1b;
AudioSynthWaveform * v2a = &voice2a;
AudioSynthWaveform * v2b = &voice2b;
AudioSynthWaveform * v3a = &voice3a;
AudioSynthWaveform * v3b = &voice3b;
AudioSynthWaveform * v4a = &voice4a;
AudioSynthWaveform * v4b = &voice4b;
AudioSynthWaveform * v5a = &voice5a;
AudioSynthWaveform * v5b = &voice5b;

AudioSynthWaveform * voices[5][2] = {{v1a, v1b}, {v2a, v2b}, {v3a, v3b}, {v4a, v4b}, {v5a, v5b}};

//voice mixers
AudioMixer4 * v1Mix = &voice1Mixer;
AudioMixer4 * v2Mix = &voice2Mixer;
AudioMixer4 * v3Mix = &voice3Mixer;
AudioMixer4 * v4Mix = &voice4Mixer;
AudioMixer4 * v5Mix = &voice5Mixer;

AudioMixer4 * voiceMixers[5] = {v1Mix, v2Mix, v3Mix, v4Mix, v5Mix};

//all the filters and amps
AudioEffectEnvelope * envAmp1 = &ampEnv1;
AudioEffectEnvelope * envFilter1 = &filterEnv1;
AudioEffectEnvelope * envAmp2 = &ampEnv2;
AudioEffectEnvelope * envFilter2 = &filterEnv2;
AudioEffectEnvelope * envAmp3 = &ampEnv3;
AudioEffectEnvelope * envFilter3 = &filterEnv3;
AudioEffectEnvelope * envAmp4 = &ampEnv4;
AudioEffectEnvelope * envFilter4 = &filterEnv4;
AudioEffectEnvelope * envAmp5 = &ampEnv5;
AudioEffectEnvelope * envFilter5 = &filterEnv5;
AudioEffectEnvelope * envelopes[5][2] = {
  {envAmp1, envFilter1},
  {envAmp2, envFilter2}, {envAmp3, envFilter3},
  {envAmp4, envFilter4}, {envAmp5, envFilter5}
};

AudioFilterStateVariable * f1 = &filter1;
AudioFilterStateVariable * f2 = &filter2;
AudioFilterStateVariable * f3 = &filter3;
AudioFilterStateVariable * f4 = &filter4;
AudioFilterStateVariable * f5 = &filter5;
AudioFilterStateVariable * filters[5] = {f1, f2, f3, f4, f5};

boolean firstRun;

//osc selects
int oscSelectPin = 26; //blue flip switch
int oscSelect = 0; //default to osc1 controls

//waveshapes
int waveShapeSelectPin = 27;
int waveShapeButtonState;
int waveShapeSelectIndex = 0; //default to WAVEFORM_SINE

short waveShapes[7] {WAVEFORM_SINE, WAVEFORM_SAWTOOTH,
        WAVEFORM_SQUARE, WAVEFORM_TRIANGLE, WAVEFORM_PULSE,
        WAVEFORM_SAWTOOTH_REVERSE, WAVEFORM_SAMPLE_HOLD
};

//env switch
int envSwitchPin = 25;
int envSwitchState, lastEnvSwitchState; //this is a switch
int envSelect = 0;

//envelope adsr
int envPins[4] = {A3, A2, A1, A0};
float ampEnvVals[4] = {1.5, 15, 15, 30};
float filterEnvVals[4] = {1.5, 15, 15, 30};
float envHoldVals[4] = {1.5, 15, 15, 30};
boolean envHolding[4] = {false, false, false, false};


//LFO (NOT USED IN THIS BUILD)
//int lfoIntensity = 0;
//int lfoRate = 10;
//int lfoWaveSelectPin = 24; //pushbutton
//
//int lfoIntensityPin = A12; //intensity is amplitude
//int lfoRatePin = A13; //rate is frequency
//
//short lfoWaveShapes[5] {WAVEFORM_SINE, WAVEFORM_SAWTOOTH, WAVEFORM_SAWTOOTH_REVERSE,
//        WAVEFORM_SQUARE, WAVEFORM_SAMPLE_HOLD
//};
////correspond to: lfoPitch, lfoCutoff, lfoFrequency, lfoVolume
//int lfoDestinations[4] {0, 1, 2, 3}; 


//notes and pitches
int octave = 1; //octave not implemented
int notesOn[5] = { -1, -1, -1, -1, -1};
float deTune;
boolean bufferFull = false; 

//encoder vals
Encoder enc1(11, 12);
int newPos1 = 0, oldPos1 = 0, encVal1 = 0;
int switchModes; //currently to turn osc2 off


//gains (not used in this build)
//int masterVolPin = A9;
//int oscVolPin = A8;
//int noiseVolPin = A7;
//float osc1Volume, osc2Volume, noiseVolume, masterVolume;


void setup() {
  Serial.begin(9600);
  
  // init OLED and show welcome buffer on display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // init with addr 0x3C
  displayString("welcome!");
  display.startscrollright(0x00, 0x0F);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.dim(true);
  //display scrolling logo
  displayLogo();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.display();
  delay(2000);
  display.clearDisplay();

  //pinmode setups
  pinMode(envSwitchPin, INPUT);
  pinMode(oscSelectPin, INPUT);
  pinMode(18, OUTPUT);
  pinMode(19, OUTPUT);
  pinMode(27, INPUT_PULLUP);
  pinMode(29, INPUT_PULLUP);

  //set levels for teensy audio library
  AudioMemory(256);
  for (int i = 0; i < 5; i++) {
    voices[i][0]->begin(.6, 440, WAVEFORM_SINE);
    voices[i][1]->begin(.5, 440, WAVEFORM_SQUARE);
    for (int j = 0; j < 5; j++) {
      voiceMixers[i]->gain(j, 1.0);
    }
  }

  //set the 4 channels of mixer gain
  for (int i = 0; i < 4; i++) {
    preMixer1.gain(i, .25);
    preMixer2.gain(i, .25);
  }
  
  dacs1.analogReference(INTERNAL);
  //set masterMixers at split half
  masterMixer.gain(1, .5);
  masterMixer.gain(0, .5);

  //hardware serial
  Serial1.begin(9600);
  Serial1.setRX(0);
  
  //firstrun is true
  firstRun = true;

}


void loop() {
  //uncomment to set up ADSR knobs
  checkEnvelopeState();
  checkUpdate();

  if (Serial1.available() > 0) {
    byte b = Serial1.read();
    if ((b & NOTE_ON) == NOTE_ON) {
      keyDown(1 + (int)b - 128);

    }
    if ((b < 128)) {
      keyUp(1 + (int)b);
    }
  }
  else {
    //do nothing
  }


}

//call this to check on button modes
void checkUpdate() {
  //waveshapeselect
  int lastWaveShapeButtonState = waveShapeButtonState;
  waveShapeButtonState = digitalRead(waveShapeSelectPin);
  if (waveShapeButtonState == LOW && lastWaveShapeButtonState == HIGH) {
    setWaveshape(waveShapeSelectIndex++);
    Serial.println(waveShapeSelectIndex);
  }

  //select osc1 or osc2 for wave shape
  int lastOscSelect = oscSelect;
  oscSelect = digitalRead(oscSelectPin);
  if (oscSelect != lastOscSelect) {
    oscSelect = !oscSelect;
  }

  //select filter vs amp envelope
  int lastEnvSelect = envSelect;
  envSelect = digitalRead(envSwitchPin);
  if (envSelect != lastEnvSelect) {
    envSelect = !envSelect;
  }

  int lastSwitchModesState = switchModes;
  switchModes = digitalRead(29);
  if (switchModes == LOW && lastSwitchModesState == HIGH) {
    //only turn it off when pressed down for effect
    osc2Off();
    Serial.println(switchModes);
  }
  if (switchModes == HIGH && lastSwitchModesState == LOW) {
    osc2On();
    Serial.println(switchModes);
  }

  checkDetune();

}

void checkDetune() {
  deTune = checkEncoder1();
  deTune = map(deTune, -32, 32, .875, 1.125);
}

void osc2Off() {
  for (int i = 0; i < 5; i++) {
    voices[i][1]->amplitude(0);
  }
}

void osc2On() {
  for (int i = 0; i < 5; i++) {
    voices[i][1]->amplitude(.5);
  }
}


int checkEncoder1() {
  newPos1 = enc1.read();
  if (newPos1 >= oldPos1 + 4) {
    oldPos1 = newPos1;
    encVal1 = oldPos1 / 4;
    Serial.println("forward");
    Serial.println(encVal1);
  } else if (newPos1 <= oldPos1 - 4) {
    oldPos1 = newPos1;
    encVal1 = oldPos1 / 4;
    Serial.println("backwards");
    Serial.println(encVal1);
  }
  return encVal1;
}

void checkEnvelopeState() { //used to check which envelope i'm controlling

  //trigger when switched states
  lastEnvSwitchState = envSwitchState;
  envSwitchState = digitalRead(envSwitchPin);

  if ((envSwitchState == HIGH && lastEnvSwitchState == LOW) ||
      (envSwitchState == LOW && lastEnvSwitchState == HIGH) || (firstRun)) {
    if (firstRun) { 
      firstRun = false; //hold vals on default from start
    }
    for (int i = 0; i < 4; i++) {
      envHolding[i] = true; //do not change values until things are moved
      envHoldVals[i] = analogRead(envPins[i]); //this is what the knobs are at
    }
  }

  //check to see if we have moved them
  for (int i = 0; i < 4; i++) {
    if (envHoldVals[i] + 5 <= analogRead(envPins[i]) || analogRead(envPins[i]) <= envHoldVals[i] - 5) {
      envHolding[i] = false;
    }
    //if we are not holding, operate as usual
    if (envHolding[i] == false) {
      //code that calls setEnvelope() based on certain knobs at [i]
      setEnvelope(i);
    }
  }
}

void setEnvelope(int param) {
  for (int i = 0; i < 5; i++) {
    switch (param) {
      case 0:
        envelopes[i][envSelect]->attack(mapA());
        break;
      case 1:
        envelopes[i][envSelect]->decay(mapD());
        break;
      case 2:
        envelopes[i][envSelect]->sustain(mapS());
        break;
      case 3:
        envelopes[i][envSelect]->release(mapR());
        break;
    }
  }
}


void setWaveshape(int waveShapeSelect) {
  AudioNoInterrupts();
  for (int i = 0; i < 5; i++) {
    voices[i][oscSelect]->begin(.5, 440, waveShapes[waveShapeSelect % 6]);
  AudioInterrupts();
}


//smooth out data by taking average of 5
int getSmooth(int pin) {
  int vals[5]; //array that stores 5 readings.
  for (int i = 0; i < 5; i++) {
    vals[i] = analogRead(pin); //takes 5 readings.
  }
  float smooth = (vals[0] + vals[1] + vals[2] + vals[3] + vals[4]) / 5;
  return smooth;
}


float midiNoteToFreq(int midiNote) {
  return 440.0 * pow(2.0, ((midiNote - 69) / 12.0));
}

float mapA() {
  return map(getSmooth(envPins[0]), 0, 1023, 1.0, 200.0);
}
float mapD() {
  return map(getSmooth(envPins[1]), 0, 1023, 0.0, 200.0);
}
float mapS() {
  return map(getSmooth(envPins[2]), 0, 1023, 0.0, 1.0);
}
float mapR() {
  return map(getSmooth(envPins[3]), 0, 1023, 0.0, 200.0);
}


void keyDown(int key) {
  //play the note (trigger envelope).

  //allocate the note to a voice
  int vox = voiceBuffer(key);
  if (vox == -1) {
    Serial.println("voice buffer full");
    bufferFull = true;
    return;
  }
  float freq = midiNoteToFreq(key);
  Serial.println(vox);
  AudioNoInterrupts();
  voices[vox][0]->frequency(freq);
  voices[vox][1]->frequency(freq * deTune);
  envelopes[vox][0]->noteOn();
  envelopes[vox][1]->noteOn();
  Serial.println(AudioProcessorUsage());
  AudioInterrupts();
  Serial.print("on! ");
  Serial.println(key);
}

void keyUp(int key) {
  //let go of the note (untrigger envelope).
  int vox = findVoice(key);
  if (vox == -1) {
    Serial.println("voice not in buffer");
    return;
  }
  AudioNoInterrupts();
  envelopes[vox][0]->noteOff();
  envelopes[vox][1]->noteOff();
  AudioInterrupts();
  notesOn[vox] = -1;
  Serial.print("off! ");
  Serial.println(key);
}

//find the first empty space in the voice buffer
int voiceBuffer(int key) {
  int v = 0;
  while (v < 5) {
    //find first -1
    if (notesOn[v] == -1) {
      notesOn[v] = key;
      return v;
    }
    v++;
  }
  //if could not find

  return -1;

}

//find which voice is playing a certain note
int findVoice(int key) {
  int v = 0;
  while (v < 5) {
    //find first matching key
    if (notesOn[v] == key) {
      //so that we know which to turn off;
      return v;
    }
    v++;
  }
  //this voice was non existent (should not reach)
  Serial.println("voice not in buffer");
  return -1;
}

//OLED
//this will crash when not in setup
void displayString(String param) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.clearDisplay();
  display.println(param);
  display.display();
  delay(1);
}

void displayLogo(void) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.clearDisplay();
  display.println("Teensynth 1.0");
  display.display();
  delay(1);
}



