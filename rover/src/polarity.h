String readPolarity(Adafruit_LSM9DS1 lsm){
  // Get new sensor events
  lsm.read();
  float z = lsm.magData.z;

  // Determine the magnet's polarity
  if (z > 0) {
    return "North Up";
  } else if (z < 0) {
    return "South Up";
  } else {
    return "Neutral";
  }
}