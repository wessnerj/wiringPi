/*
 * readall.c:
 *	The readall functions - getting a bit big, so split them out.
 *	Copyright (c) 2012-2017 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <wiringPi.h>
/*----------------------------------------------------------------------------*/

extern int wpMode ;

/*----------------------------------------------------------------------------*/
#ifndef TRUE
#  define 	TRUE	(1==1)
#  define	FALSE	(1==2)
#endif

/*----------------------------------------------------------------------------*/
/*
 * doReadallExternal:
 *	A relatively crude way to read the pins on an external device.
 *	We don't know the input/output mode of pins, but we can tell
 *	if it's an analog pin or a digital one...
 */
/*----------------------------------------------------------------------------*/
static void doReadallExternal (void)
{
	int pin ;

	printf ("+------+---------+--------+\n") ;
	printf ("|  Pin | Digital | Analog |\n") ;
	printf ("+------+---------+--------+\n") ;

	for (pin = wiringPiNodes->pinBase ; pin <= wiringPiNodes->pinMax ; ++pin)
		printf ("| %4d |  %4d   |  %4d  |\n", pin, digitalRead (pin), analogRead (pin)) ;

	printf ("+------+---------+--------+\n") ;
}

/*----------------------------------------------------------------------------*/
static const char *alts [] =
{
	"IN", "OUT", "ALT1", "ALT2", "ALT3", "ALT4", "ALT5", "ALT6", "ALT7"
} ;

static const char *pupd [] =
{
	"DSBLD", "P/U", "P/D"
} ;

/*----------------------------------------------------------------------------*/
static const int physToWpi [64] =
{
	-1,	// 0
	-1, -1,	// 1, 2
	 8, -1,
	 9, -1,
	 7, 15,
	-1, 16,
	 0,  1,
	 2, -1,
	 3,  4,
	-1,  5,
	12, -1,
	13,  6,
	14, 10,
	-1, 11,	// 25, 26
	30, 31,	// Actually I2C, but not used
	21, -1,
	22, 26,
	23, -1,
	24, 27,
	25, 28,
	-1, 29,
	-1, -1,
	-1, -1,
	-1, -1,
	-1, -1,
	-1, -1,
	17, 18,
	19, 20,
	-1, -1, -1, -1, -1, -1, -1, -1, -1
} ;

/*----------------------------------------------------------------------------*/
static const char *physNamesOdroidC2All_Rev2 [64] =
{
	NULL,

	"    3.3V", "5V      ",
	"   SDA.1", "5V      ",
	"   SCL.1", "GND(0V) ",
	"GPIO.249", "TxD1    ",
	" GND(0V)", "RxD1    ",
	"GPIO.247", "GPIO.238",
	"GPIO.239", "GND(0V) ",
	"GPIO.237", "GPIO.236",
	"    3.3V", "GPIO.233",
	"GPIO.235", "GND(0V) ",
	"GPIO.232", "GPIO.231",
	"GPIO.230", "GPIO.229",
	" GND(0V)", "GPIO.225",
	"   SDA.2", "SCL.2   ",
	"GPIO.228", "GND(0V) ",
	"GPIO.219", "GPIO.224",
	"GPIO.234", "GND(0V) ",
	"GPIO.214", "GPIO.218",
	"   AIN.1", "1V8     ",
	" GND(0V)", "AIN.0   ",

	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,
};

/*----------------------------------------------------------------------------*/
static const char *physNamesOdroidC2_Rev2 [64] =
{
	NULL,

	"   3.3V", "5V     ",
	"  SDA.1", "5V     ",
	"  SCL.1", "0V     ",
	" IO.249", "TxD1   ",
	"     0V", "RxD1   ",
	" IO.247", "IO.238 ",
	" IO.239", "0V     ",
	" IO.237", "IO.236 ",
	"   3.3V", "IO.233 ",
	" IO.235", "0V     ",
	" IO.232", "IO.231 ",
	" IO.230", "IO.229 ",
	"     0V", "IO.225 ",
	"  SDA.2", "SCL.2  ",
	" IO.228", "0V     ",
	" IO.219", "IO.224 ",
	" IO.234", "0V     ",
	" IO.214", "IO.218 ",
	"  AIN.1", "1V8    ",
	"     0V", "AIN.0  ",

	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,
} ;

/*----------------------------------------------------------------------------*/
static const char *physNamesOdroidC2All_Rev1 [64] =
{
	NULL,

	"    3.3V", "5V      ",
	"   SDA.1", "5V      ",
	"   SCL.1", "GND(0V) ",
	"GPIO.214", "--------",
	" GND(0V)", "--------",
	"GPIO.219", "GPIO.218",
	"GPIO.247", "GND(0V) ",
	"--------", "GPIO.235",
	"    3.3V", "GPIO.233",
	"GPIO.238", "GND(0V) ",
	"GPIO.237", "GPIO.234",
	"GPIO.236", "GPIO.248",
	" GND(0V)", "GPIO.249",
	"   SDA.2", "SCL.2   ",
	"GPIO.232", "GND(0V) ",
	"GPIO.231", "GPIO.230",
	"GPIO.239", "GND(0V) ",
	"GPIO.228", "GPIO.229",
	"   AIN.1", "1V8     ",
	" GND(0V)", "AIN.0   ",

	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,
};

/*----------------------------------------------------------------------------*/
static const char *physNamesOdroidC2_Rev1 [64] =
{
	NULL,

	"   3.3V", "5V     ",
	"  SDA.1", "5V     ",
	"  SCL.1", "0V     ",
	" IO.214", "-------",
	"     0V", "-------",
	" IO.219", "IO.218 ",
	" IO.247", "0V     ",
	"-------", "IO.235 ",
	"   3.3V", "IO.233 ",
	" IO.238", "0V     ",
	" IO.237", "IO.234 ",
	" IO.236", "IO.248 ",
	"     0V", "IO.249 ",
	"  SDA.2", "SCL.2  ",
	" IO.232", "0V     ",
	" IO.231", "IO.230 ",
	" IO.239", "0V     ",
	" IO.228", "IO.229 ",
	"  AIN.1", "1V8    ",
	"     0V", "AIN.0  ",

	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,
};

/*----------------------------------------------------------------------------*/
static void readallPhys(int model, int UNU rev, int physPin, const char *physNames[], int isAll) {
	int pin ;

	// GPIO, wPi pin number
	if (isAll == TRUE) {
		if ((physPinToGpio (physPin) == -1) && (physToWpi [physPin] == -1))
			printf(" |      |    ");
		else if (physPinToGpio (physPin) != -1) {
			printf(" |  %3d | %3d", physPinToGpio(physPin), physToWpi[physPin]);
		} else
			printf(" |      | %3d", physToWpi [physPin]);
	} else {
		if ((physPinToGpio (physPin) == -1) && (physToWpi [physPin] == -1))
			printf(" |     |    ");
		else if (physPinToGpio (physPin) != -1) {
			printf(" | %3d | %3d", physPinToGpio(physPin), physToWpi[physPin]);
		} else
			printf(" |     | %3d", physToWpi [physPin]);
	}

	// GPIO pin name
	printf (" | %s", physNames [physPin]) ;

	// GPIO pin mode, value
	if ((physToWpi [physPin] == -1) || (physPinToGpio (physPin) == -1)) {
		printf(" |      |  ");
		if (isAll == TRUE)
			printf(" |    |      ");
	} else {
		if (wpMode == MODE_GPIO)
			pin = physPinToGpio (physPin);
		else if (wpMode == MODE_PHYS)
			pin = physPin ;
		else
			pin = physToWpi [physPin];

		printf (" | %4s", alts [getAlt (pin)]) ;
		printf (" | %d", digitalRead (pin)) ;

		// GPIO pin drive strength, pu/pd
		if (isAll == TRUE) {
			switch (model) {
			case MODEL_ODROID_C2:
				printf (" |    | %5s", pupd[getPUPD(pin)]);
				break;
			default:
				break;
			}
		}
	}

	// Physical pin number
	printf (" | %2d", physPin) ;
	++physPin ;
	printf (" || %-2d", physPin) ;

	// GPIO pin mode, value
	if ((physToWpi [physPin] == -1) || (physPinToGpio (physPin) == -1)) {
		printf(" |");
		if (isAll == TRUE)
			printf("       |    |");
		printf("   |     ");
	} else {
		if (wpMode == MODE_GPIO)
			pin = physPinToGpio (physPin);
		else if (wpMode == MODE_PHYS)
			pin = physPin ;
		else
			pin = physToWpi [physPin];


		// GPIO pin drive strength, pu/pd
		if (isAll == TRUE) {
			switch (model) {
			case MODEL_ODROID_C2:
				printf (" | %-5s |   ", pupd[getPUPD(pin)]);
				break;
			default:
				break;
			}
		}
		printf(" | %d", digitalRead (pin));
		printf(" | %-4s", alts [getAlt (pin)]);
	}

	// GPIO pin name
	printf (" | %-6s", physNames [physPin]);

	// GPIO, wPi pin number
	if (isAll == TRUE) {
		if ((physPinToGpio (physPin) == -1) && (physToWpi [physPin] == -1))
			printf(" |     |     ");
		else if (physPinToGpio (physPin) != -1)
			printf(" | %-3d | %-3d ", physToWpi [physPin], physPinToGpio (physPin));
		else
			printf(" | %-3d |     ", physToWpi [physPin]);
	} else {
		if ((physPinToGpio (physPin) == -1) && (physToWpi [physPin] == -1))
			printf(" |     |    ");
		else if (physPinToGpio (physPin) != -1)
			printf(" | %-3d | %-3d", physToWpi [physPin], physPinToGpio (physPin));
		else
			printf(" | %-3d |    ", physToWpi [physPin]);
	}

	printf (" |\n") ;
}

static void printHeader(const char *headerName, int isAll) {
	const char *headerLeft = " +-----+-----+---------+------+---+";
	const char *headerRight = "+---+------+---------+-----+-----+\n";
	const char *headerLeftAll = " +------+-----+----------+------+---+----+";
	const char *headerRightAll = "+----+---+------+----------+-----+------+\n";

	(isAll == FALSE) ? printf("%s", headerLeft) : printf("%s", headerLeftAll);
	printf("%s", headerName);
	(isAll == FALSE) ? printf("%s", headerRight) : printf("%s", headerRightAll);
}

/*----------------------------------------------------------------------------*/
static void printBody(int model, int rev, const char *physNames[], int isAll) {
	(isAll == FALSE)
		? printf(
			" | I/O | wPi |   Name  | Mode | V | Physical | V | Mode |  Name   | wPi | I/O |\n"
			" +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+\n")
		: printf(
			" | GPIO | wPi |   Name   | Mode | V | DS | PU/PD | Physical | PU/PD | DS | V | Mode |   Name   | wPi | GPIO |\n"
			" +------+-----+----------+------+---+----+-------+----++----+-------+----+---+------+----------+-----+------+\n");
	for (int pin = 1; pin <= 40; pin += 2)
		readallPhys(model, rev, pin, physNames, isAll);
	(isAll == FALSE)
		? printf(
			" +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+\n"
			" | I/O | wPi |   Name  | Mode | V | Physical | V | Mode |  Name   | wPi | I/O |\n")
		: printf(
			" +------+-----+----------+------+---+----+-------+----++----+-------+----+---+------+----------+-----+------+\n"
			" | GPIO | wPi |   Name   | Mode | V | DS | PU/PD | Physical | PU/PD | DS | V | Mode |   Name   | wPi | GPIO |\n");
}

/*----------------------------------------------------------------------------*/
/*
 * doReadall:
 *	Read all the GPIO pins
 *	We also want to use this to read the state of pins on an externally
 *	connected device, so we need to do some fiddling with the internal
 *	wiringPi node structures - since the gpio command can only use
 *	one external device at a time, we'll use that to our advantage...
 */
/*----------------------------------------------------------------------------*/
void doReadall(int argc, char *argv[]) {
	int model, rev, mem, maker, overVolted, isAll;
	char *headerName, *physNames;

	// External readall
	if (wiringPiNodes != NULL) {
		doReadallExternal();
		return;
	}

	if (argc <= 2) {
		isAll = FALSE;
	} else if (argc == 3 && (strcasecmp(argv[2], "-a") == 0 || strcasecmp(argv[2], "--all") == 0)) {
		isAll = TRUE;
	} else {
		printf("Oops - unknown readall option:\n");
		for (int i = 3; i < argc + 1; i++)
			printf("\targv[%d]: %s\n", i, argv[i - 1]);

		return;
	}

	piBoardId (&model, &rev, &mem, &maker, &overVolted);

	switch (model) {
		case MODEL_ODROID_C2:
			headerName = (isAll == FALSE) ? "--- C2 ---" : "---- Model  ODROID-C2 ----";
			if (rev == 1)
				physNames = (char *) ((isAll == FALSE) ? physNamesOdroidC2_Rev1 : physNamesOdroidC2All_Rev1);
			else
				physNames = (char *) ((isAll == FALSE) ? physNamesOdroidC2_Rev2 : physNamesOdroidC2All_Rev2);
			break;
		default:
			printf("Oops - unknown model: %d\n", model);
			return;
	}

	switch (model) {
		default:
			printHeader((const char *) headerName, isAll);
			printBody(model, rev, (const char **) physNames, isAll);
			printHeader((const char *) headerName, isAll);
			break;
	}
}

/*----------------------------------------------------------------------------*/
/*
 * doAllReadall:
 *	Force reading of all pins regardless of Pi model
 */
/*----------------------------------------------------------------------------*/
void doAllReadall(void) {
	char *fakeArgv[3] = { "", "", "--all" };

	doReadall(3, fakeArgv);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
