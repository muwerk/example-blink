#define USE_SERIAL_DBG 1

#include "ustd_platform.h"
#include "scheduler.h"

#if USTD_FEATURE_MEMORY < USTD_FEATURE_MEM_2K
#include "mup_digital_out.h"
#define LIGHT_CLASS DigitalOut
#else
#include "mup_light.h"
#define LIGHT_CLASS Light
#define LIGHT_CONTROLLER
#endif

#include "mupplet_core.h"

void appLoop();

#if USTD_FEATURE_MEMORY < USTD_FEATURE_MEM_2K
ustd::Scheduler sched(2, 2, 2);
#else
#if USTD_FEATURE_MEMORY < USTD_FEATURE_MEM_8K
ustd::Scheduler sched(4, 4, 8);
#else
ustd::Scheduler sched(10, 16, 32);
#endif
#endif

ustd::LIGHT_CLASS led1("myLed1", MB_LED1, true);
ustd::LIGHT_CLASS led2("myLed2", MB_LED2, true);

void setup() {
#ifdef USE_SERIAL_DBG
    Serial.begin(115200);
    Serial.println("Starting...");
#endif

    led1.begin(&sched);
    led2.begin(&sched);

#ifdef LIGHT_CONTROLLER  // All but the tiniest AVRs can use LightController, which can do all sorts of effects.
    led1.setMode(ustd::LightController::Mode::Wave, 700, 0.0);
    led2.setMode(ustd::LightController::Mode::Wave, 1300, 0.5);
#endif

    // App loop is called every 100ms, but does nothing, as the LightController does all the work in the background
    // (exception: tiny AVR, which can't use LightController and uses the appLoop() instead )
    sched.add(appLoop, "1", 100000L);
}

bool bl1 = false;
bool bl2 = false;
int c1 = 0;
int c2 = 0;
int cms1 = 7;
int cms2 = 13;
void appLoop() {
#ifndef LIGHT_CONTROLLER  // LightController does this automatically, so only tiny AVR need this.
    c1++;
    if (c1 >= cms1) {
        c1 = 0;
        bl1 = !bl1;
        led1.set(bl1);
    }
    c2++;
    if (c2 >= cms2) {
        c2 = 0;
        bl2 = !bl2;
        led2.set(bl2);
    }
#endif
}

// Never add code to this loop, use appLoop() instead.
void loop() {
    sched.loop();
}