#define FULLPOWER = 127
void Drive(float power_modifier, int time) {
  motor[rightMotor] = FULLPOWER * power_modifier;
  motor[rightMotor] = FULLPOWER * power_modifier;
  wait1Msec(time * 1000)
}

task main() {
  wait1Msec(2000);
  
  for (float i=1; i < 4; i++) {
    Drive(1, i);
    Drive(-1, i);
  }
}
