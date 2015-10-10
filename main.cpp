/*
 *  Author: Nicanor Pagsanjan
 *          UC Berkeley, Space Sciences Lab
 *
 *  Copyright (c)2015. The Regents of the University of California (Regents).
 *  All Rights Reserved. Permission to use, copy, modify, and distribute this
 *  software and its documentation for educational, research, and not-for-profit  
 *  purposes, without fee and without a signed licensing agreement, is hereby
 *  granted, provided that the above copyright notice, this paragraph and the
 *  following two paragraphs appear in all copies, modifications, and
 *  distributions. Contact The Office of Technology Licensing, UC Berkeley, 2150
 *  Shattuck Avenue, Suite 510, Berkeley, CA 94720-1620, (510) 643-7201, for
 *  commercial licensing opportunities.
 *
 *  IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
 *  SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS,
 *  ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
 *  REGENTS HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED
 *  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
 *  HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
 *  MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 *  +--------------------+
 *  | Flashbang detector |
 *  +--------------------+
 *
 *  Device is now independently triggered by light
 *  and sound.
 *
 *  This device runs a thread that attaches an interrupt
 *  to the RTC where the timer in microseconds is reset
 *  every second.
 *
 *  +----------------+
 *  | Sensor logging |
 *  +----------------+
 *
 *  Format for sensor data goes as such:
 *  YYYY MM DD HH:MM:SS.xxxxxx (L or S) T
 *
 *  time in UTC, where L or S is a light or sound trigger,
 *  T is temperature in degrees Celsius respectively.
 *
 *  +----------+
 *  | Hardware |
 *  +----------+
 *
 *  SparkFun MicroSD Breakout Board
 *
 *  MicroSD Breakout    mbed
 *
 *  CS  --------------- 8    (DigitalOut cs)
 *  DI  --------------- 5    (SPI mosi)
 *  VCC --------------- VOUT
 *  SCK --------------- 7    (SPI sck)
 *  GND --------------- GND
 *  DO  --------------- 6    (SPI miso)
 *  CD  -
 *
 *  +-----------+
 *  | LIBRARIES |
 *  +-----------+
 *
 *  mBed Standard Library
 *      (https://developer.mbed.org/users/mbed_official/code/mbed/)
 *  mBed Real-Time Operating System
 *      (https://developer.mbed.org/handbook/RTOS)
 *  EthernetInterface
 *      (https://developer.mbed.org/handbook/Ethernet-Interface)
 *  NTPClient
 *      (https://developer.mbed.org/cookbook/NTP-Client)
 *  SDFileSystem
 *      (https://developer.mbed.org/users/neilt6/code/SDFileSystem/)
 *  MODGPS
        (https://developer.mbed.org/users/AjK/code/MODGPS/docs/tip/GPS_8h_source.html)
 *  RTC
 *      (https://developer.mbed.org/users/Sissors/code/RTC/)
 */

#include "mbed.h"               // basic MBED libraries
#include "EthernetInterface.h"  // Ethernet Drivers
#include "SDFileSystem.h"       // SD card Drivers
#include "NTPClient.h"          // NTP Client
#include "RTC.h"                // RTC interrupts

// color definitions for prettier serial output
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// calibration values
#define LTHRESH  5.00
#define MTHRESH1 1.20
#define CSIZE    64

// Inputs
AnalogIn TMP(p18);
AnalogIn PHT(p19);
AnalogIn MIC(p20);

// Networking
EthernetInterface eth;
NTPClient ntp_client;

// Parameters for networking
char* domain_name = "0.uk.pool.ntp.org";

// SD card (SPI Pins)
SDFileSystem sd(p5, p6, p7, p8, "sd");

// Time
time_t ct_time;
char time_buffer[80];

// USB serial (tx, rx)
Serial pc(USBTX, USBRX);

// Global Timer
Timer timer;

void timer_reset(void) {
    timer.reset();
}

int main() {
	// timer reset interrupt
	RTC::attach(&timer_reset, RTC::Second);

    //SD Card File
    FILE* file;

    // Input Thresholds
    float pThresh;  // Light threshold
    float mThresh1; // Upper Sound threshold

    // start timer
    timer.start();

    // data to be polled
    float pht;
    float mic;

    // initialize connection and update time
    pc.printf("Initializing connection...\n\r");
    eth.init();
    if (eth.connect() < 0) {
        error(ANSI_COLOR_RED "Connection Failed!\n\rExiting program.\n\r" ANSI_COLOR_RESET);
    }
    else {
        pc.printf("Connection success! IP is: %s\n\r", eth.getIPAddress());
    }
    ntp_client.setTime(domain_name, 123);
    eth.disconnect();

    // Print initialized time
    ct_time = time(NULL);
    strftime(time_buffer, 80, "%F %H:%M:%S", localtime(&ct_time));
    pc.printf("Time is: %s.%d\n\r", time_buffer, timer.read_us());

    // calibrate sensors
    pc.printf("Calibrating sensors...\n\rPlease wait.\n\r");
    float sound = 0, light = 0;
    for (int i=0; i < CSIZE; ++i) {
        sound += MIC;
        light += PHT;
        wait(0.2);
    }
    pThresh  = light * LTHRESH  / CSIZE;
    mThresh1 = sound * MTHRESH1 / CSIZE;
    pc.printf(ANSI_COLOR_MAGENTA "Calibration complete.\n\r" ANSI_COLOR_RESET);
    pc.printf("Initializing File...\n\r");

    // initialize file
    file = fopen("/sd/DATA.txt", "a");
    if (file == NULL) {
        error("Unable to initialize file.\n\rExiting program.\n\r");
    }
    fclose(file);
// with microlib and ARM compiler
#if defined(__MICROLIB) && defined(__ARMCC_VERSION)
    free(fp);
#endif
    pc.printf(ANSI_COLOR_BLUE "File opened!\n\r" ANSI_COLOR_RESET);

    while(1) {
        // polling data
        pht = PHT;
        mic = MIC;
		float tmp = TMP;
		tmp = ((tmp * 3.3)- 0.5) * 100.0;
        // Photodiode trigger
        if (pht >= pThresh) {
            // update time
            ct_time = time(NULL);
            strftime(time_buffer, 80, "%F %H:%M:%S", localtime(&ct_time));

            // open file, then write to sd
            file = fopen("/sd/DATA.txt", "a");
            if (file == NULL) {
                error("Unable to initialize file.\n\rExiting program.\n\r");
            }
            fprintf(file, "%s.%d L %1.2f\n", time_buffer, timer.read_us(), tmp);
            fclose(file);
// with microlib and ARM compiler
#if defined(__MICROLIB) && defined(__ARMCC_VERSION)
            free(fp);
#endif

            // print to screen
            pc.printf("%s.%06d L %1.2f\n\r", time_buffer, timer.read_us(), tmp);
			wait_us(150000);
        }

        // Microphone Trigger
        if (mic >= mThresh1) {

            // update time
            ct_time = time(NULL);
            strftime(time_buffer, 80, "%F %H:%M:%S", localtime(&ct_time));

            // open file, then write to sd
            file = fopen("/sd/DATA.txt", "a");
            if (file == NULL) {
                error("Unable to initialize file.\n\rExiting program.\n\r");
            }
            fprintf(file, "%s.%d S %1.2f\n", time_buffer, timer.read_us(), tmp);
            fclose(file);
// with microlib and ARM compiler
#if defined(__MICROLIB) && defined(__ARMCC_VERSION)
            free(fp);
#endif

            // print to screen
            pc.printf("%s.%06d S %1.2f\n\r", time_buffer, timer.read_us(), tmp);
    		wait_us(150000);
	    }
    }
}
