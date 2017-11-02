//final project
//teensynth

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform2;      //xy=115,219
AudioSynthWaveform       waveform1;      //xy=119,180
AudioSynthNoiseWhite     noise1;         //xy=119,263
AudioMixer4              mixer1;         //xy=286,208
AudioEffectEnvelope      filterEnv;      //xy=402,271
AudioFilterStateVariable filter1;        //xy=491,215
AudioEffectEnvelope      ampEnv;      //xy=612,214
AudioMixer4              mainOutMixer;         //xy=736,284
AudioOutputAnalog        dac1;           //xy=825,183
AudioConnection          patchCord1(waveform2, 0, mixer1, 1);
AudioConnection          patchCord2(waveform1, 0, mixer1, 0);
AudioConnection          patchCord3(noise1, 0, mixer1, 2);
AudioConnection          patchCord4(mixer1, filterEnv);
AudioConnection          patchCord5(mixer1, 0, filter1, 0);
AudioConnection          patchCord6(filterEnv, 0, filter1, 1);
AudioConnection          patchCord7(filter1, 0, ampEnv, 0);
AudioConnection          patchCord8(ampEnv, 0, mainOutMixer, 0);
AudioConnection          patchCord9(mainOutMixer, dac1);
// GUItool: end automatically generated code


int button1Pin = 12;
int switchPin = 24;
int pot1Pin = A0;
int pot2Pin = A1;
int pot3Pin = A4;
boolean switchState;
boolean button1 = LOW;
boolean lastState1 = LOW;

float pitches[2] = {440, 440};
float fineTune, fineTuneState = 0;
float masterVolume;


void setup() {
  pinMode(button1Pin, INPUT);
  pinMode(switchPin, INPUT);
  
  pinMode(pot1Pin, INPUT);
  pinMode(pot2Pin, INPUT);
  pinMode(pot3Pin, INPUT);
  
  AudioMemory(12); //12 bit dac output
  for (int i = 0; i > 4; i++) {
    mixer1.gain(i, 1);
  }
  mainOutMixer.gain(0, 1);
  waveform1.begin(0.5, pitches[0], WAVEFORM_SINE);
  waveform2.begin(0.5, pitches[1], WAVEFORM_SQUARE);
  noise1.amplitude(0.1);
  filter1.frequency(5000);
  Serial.begin(9600);

}

void loop() {
  checkButton1();
  checkVolume();
  checkAttack();
  checkSwitch();
  checkTune();

}

//think about LFO (also targets)
void checkTune(){
  fineTune = (float)((getSmooth(pot3Pin)/1023.0)*16.0)-8;
  //this is a problem because the analog pots are very fickle so it hard to keep steady
  //figure out a psuedo rotary encoder/digipot way to implement this
  if (fineTune != fineTuneState){ 
    if (switchState == HIGH){
      waveform1.frequency(pitches[0]+fineTune);
    }
    else if (switchState == LOW){
      waveform2.frequency(pitches[1]+fineTune);
    }
    Serial.println(fineTune);
  }
  fineTuneState = fineTune;
}

boolean checkSwitch() {
  //to mute noise and OSC2
  switchState = digitalRead(switchPin);
  if (switchState == HIGH) { //turn off noise and OSC2
    waveform2.amplitude(0);
    noise1.amplitude(0);
  }
  else if (switchState == LOW) {
    waveform2.amplitude(0.5);
    noise1.amplitude(0.5);
  }
  return switchState;
}

void checkVolume() {
  masterVolume = map(getSmooth(pot1Pin), 0, 1023, 0, 100);
  mainOutMixer.gain(0, masterVolume / 100);
}

void checkAttack() {
  float ampA = (float) (getSmooth(A1) / 1023.0) * 200;
  ampEnv.attack(ampA);
  //Serial.println(ampA);
}

void checkButton1() {
  //read button1 from button1Pin and make ref. lastState
  lastState1 = button1;
  button1 = digitalRead(button1Pin);

  //read button on off for midi note sending
  if (button1 == HIGH && lastState1 == LOW) {
    Serial.println("attack!");
    ampEnv.noteOn();
    filterEnv.noteOn();
  }
  else if (button1 == LOW && lastState1 == HIGH) {
    Serial.println("release!");
    ampEnv.noteOff();
    filterEnv.noteOff();
  }
}


int getSmooth(int pin) {
  int vals[5]; //store 5 readings
  for (int i = 0; i < 5; i++) {
    vals[i] = analogRead(pin); //get 5 readings
  }
  float smooth = (vals[0] + vals[1] + vals[2] + vals[3] + vals[4]) / 5; //average those readings
  return smooth;
}
