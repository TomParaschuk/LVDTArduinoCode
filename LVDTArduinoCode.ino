//the analog pin where we plug in the DC input
//we probably also have to connect a common ground
#define ANALOGPIN 5

//the number of values used for the average
//should take around 0.5 seconds to sample 5000 times from ADC
#define NUMSAMPLES 5000

//scaling factor to transform raw ADC to a real voltage
//5 for vRef (this might change)
//1023 for the 10 bit size of the input
#define VOLTAGECONVERSIONFACTOR 5/1023

//running total of input voltages
unsigned long inputVoltageSum;

//average voltage converted to the right units
//final voltage
double averageVoltage;

//displacement, linear function (currently unknown) of voltage
double displacement;

void setup() {
  // put your setup code here, to run once:

}

void loop() {

  //reset the input voltage sum
  inputVoltageSum = 0;

  //gather as many samples as running average size
  for(int i = 0; i < NUMSAMPLES; i++)
  {
    //read one voltage
    inputVoltageSum += analogRead(ANALOGPIN);
  }

  //divide by the number of samples then apply the scaling factor
  averageVoltage = inputVoltageSum / NUMSAMPLES * VOLTAGECONVERSIONFACTOR;

  // displacement =  unknown function of average voltage

  //display with the LCD

}
