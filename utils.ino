/* Same as delay(d) (or delay(d-1) if d%2 == 1
 * Make the builtin led blink on half the time of the delay
 */
void delay_blink(unsigned long d)
{
  unsigned long d_2 = d / 2;
  
  digitalWrite(LED_BUILTIN, HIGH);
  delay(d_2);
  digitalWrite(LED_BUILTIN, LOW);
  delay(d_2);
}
