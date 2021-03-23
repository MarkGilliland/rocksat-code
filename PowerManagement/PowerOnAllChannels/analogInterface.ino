float countsToVolts(int counts){//converts to volts
  return counts*0.0032226;//assumes 3.3V, 10bit
}
float countsToAmps(int counts){//converts to amps
  float volts = countsToVolts(counts);
  return volts*2.0;
}
float countsToDegrees(int counts){//converts to degrees C
  float volts = countsToVolts(counts);
  return (volts-0.5)*100;
}
