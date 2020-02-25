#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, I2C_1,  rightIEM,       sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_2,  leftIEM,        sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Motor,  port1,           rightMotor,    tmotorVex393, openLoop, reversed, encoder, encoderPort, I2C_1, 1000)
#pragma config(Motor,  port10,          leftMotor,     tmotorVex393, openLoop, reversed, encoder, encoderPort, I2C_2, 1000)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*
Drive a certain pattern of straights and turns
ds = 31
dl = 10
*/

task display(){
	bLCDBacklight = true;
	string mainBattery, backupBattery;

	while(true)
	{
		clearLCDLine(0);
		clearLCDLine(1);

		//Display the Primary Robot battery voltage
		displayLCDString(0, 0, "Primary: ");
		sprintf(mainBattery, "%1.2f%c", nImmediateBatteryLevel/1000.0,'V'); //Build the value to be displayed
		displayNextLCDString(mainBattery);

		//Display the Backup battery voltage
		displayLCDString(1, 0, "Backup: ");
		sprintf(backupBattery, "%1.2f%c", BackupBatteryLevel/1000.0, 'V');	//Build the value to be displayed
		displayNextLCDString(backupBattery);

		//Short delay for the LCD refresh rate
		wait1Msec(100);
	}
}

#define BASEDISTANCE 572.76
#define STARTSLOWDOWN 360
#define BASETURN 3.1


int GetDirection(int value) {
	return value < 0 ? -1 : 1;
}

void CorrectPower(int baseSpeedLeft, int baseSpeedRight, int correctionStrength, float power_modifier) {

	int leftDirection = GetDirection(baseSpeedLeft);
	int rightDirection = GetDirection(baseSpeedRight);

	if(abs(nMotorEncoder[rightMotor]) == abs(nMotorEncoder[leftMotor]))
	{
		motor[rightMotor] = baseSpeedRight * power_modifier;
		motor[leftMotor]  = baseSpeedLeft * power_modifier;
	}
	else if(abs(nMotorEncoder[rightMotor]) > abs(nMotorEncoder[leftMotor]))
	{
		motor[rightMotor] = (abs(baseSpeedRight) - correctionStrength) * rightDirection * power_modifier;
		motor[leftMotor]  = (abs(baseSpeedLeft) + correctionStrength) * leftDirection * power_modifier;
	}
	else
	{
		motor[rightMotor] = (abs(baseSpeedRight) + correctionStrength) * rightDirection * power_modifier;
		motor[leftMotor]  = (abs(baseSpeedLeft) - correctionStrength) * leftDirection * power_modifier;
	}
}

void Drive(int distance, float power_modifier) {

	nMotorEncoder[rightMotor] = 0;
	nMotorEncoder[leftMotor] = 0;

	while(abs(nMotorEncoder[leftMotor]) < distance)
	{
		if (distance - STARTSLOWDOWN < abs(nMotorEncoder[leftMotor])) {
			// Slow down gently.

			// Our x needs to start at 0 for the formula to work
			float x = abs(nMotorEncoder[leftMotor]) - distance + STARTSLOWDOWN;

			// At 0 this formula returns 1
			// At 1 this formula returns 0,95
			// At 360 this formula returns 0,05
			float result = 1 * (-(x/(200+x))+1);

			// Make sure we don't slow down too much.
			result = result < 0.0001 ? 50 : result;

			// Convert it to the direction of the current modifier
			power_modifier = power_modifier < 0 ? -result : result;
		}
		if(abs(nMotorEncoder[rightMotor]) == abs(nMotorEncoder[leftMotor]))
		{
			motor[rightMotor] = 80 * power_modifier;
			motor[leftMotor]  = 80 * power_modifier;
		}
		else if(abs(nMotorEncoder[rightMotor]) > abs(nMotorEncoder[leftMotor]))
		{
			motor[rightMotor] = 50 * power_modifier;
			motor[leftMotor]  = 110 * power_modifier;
		}
		else
		{
			motor[rightMotor] = 110 * power_modifier;
			motor[leftMotor]  = 50 * power_modifier;
		}
	}
	motor[rightMotor] = 0;
	motor[leftMotor]  = 0;
}

void Turn(int degrees) {
	nMotorEncoder[rightMotor] = 0;
	nMotorEncoder[leftMotor] = 0;

	int direction;
	if (0 < degrees) {
		direction = 1;
		degrees += 7;
	} else {
		direction = -1;
		degrees -= 5;
	}
	float turn = degrees * BASETURN;

	while(abs(nMotorEncoder[rightMotor]) < abs(turn) && abs(nMotorEncoder[leftMotor]) < abs(turn)) {
		CorrectPower(50, -50, 10, direction);
	}

	motor[rightMotor] = 0;
	motor[leftMotor]  = 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++| MAIN |+++++++++++++++++++++++++++++++++++++++++++++++
task main()
{
	StartTask(display);


	int sequence[14] = {1, -1, -1, 1, 1, -1, 1, 1, -1, 1, 1, -1, -1, 1};

	wait1Msec(5000);
	for(int i = 0; i < sizeof(sequence) / sizeof(int); i++) {
		Drive(BASEDISTANCE, 1);
		wait1Msec(1000);
		Turn(90 * sequence[i]);
		wait1Msec(1000);
	}
	Drive(BASEDISTANCE, 1);

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
