float calibrateReference(Adafruit_FXOS8700 accelmag){
  /* Get a new sensor event */
  //sensors_event_t aevent, mevent;
  accelmag.getEvent(&aevent, &mevent);
  float y_ref = mevent.magnetic.y;
  Serial.print("y_ref: ");
  Serial.println(y_ref);
  return y_ref;
}