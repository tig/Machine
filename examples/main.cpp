#include <Arduino.h>
#include <ArduinoLog.h>
#include <assert.h>
#include <FlashStringTable.h>
#include <Fsm.h>
#include <FlashStringTable.h>

#include <Machine.h>
#include "Beaker.h"
#include "MainMachine.h"

void waitForSerial(unsigned long timeout_millis) {
  // Start Serial
  Serial.begin(57600);
  unsigned long start = millis();
  while (!Serial && !Serial.available()) {
    if (millis() - start > timeout_millis) break;
  }
}

void setup() {
  // put your setup code here, to run once:
  waitForSerial(8000);
  delay(250);

  // TODO: For release builds, change to LOG_LEVEL_WARNING
#ifndef DRV_DEBUG_H_
  Log.begin(LOG_LEVEL_VERBOSE, &Serial, false);
#else
  Log.begin(LOG_LEVEL_VERBOSE, &_Debug, false);
#endif

  // Log.setPrefix(printTimestamp); // Uncomment to get timestamps as prefix
  Log.noticeln(F("Machine Test App"));

  Log.traceln(F("Machine!"));
  Machine &machine = Machine::getInstance();
  machine.begin();
  Log.traceln("Machine printTo: %p ", machine);
  Log.traceln("Machine State printTo: %p ", machine._stateStrings);
  for (uint16_t i = 0; i < machine._stateStrings.getNumStrings(); i++) {
    Log.trace("%d = %S, ", i, machine._stateStrings.getString(i));
  }
  Log.traceln(F("\n"));
  Log.traceln("Machine Trigger printTo: %p", machine._triggerStrings);
  for (uint16_t i = 0; i < machine._triggerStrings.getNumStrings(); i++) {
    Log.trace("%d = %S, ", i, machine._triggerStrings.getString(i));
  }
  Log.traceln(F("\n"));

  Log.traceln(F("Beaker!"));
  Beaker& beaker = Beaker::getInstance();
  beaker.begin();
  Log.traceln("Beaker printTo: %p ", beaker);
  Log.traceln("Beaker State printTo: %p ", beaker._stateStrings);
  for (uint16_t i = 0; i < beaker._stateStrings.getNumStrings(); i++) {
    Log.trace("%d = %S, ", i, beaker._stateStrings.getString(i));
  }
  Log.traceln(F("\n"));
  Log.traceln("Beaker Trigger printTo: %p", beaker._triggerStrings);
  for (uint16_t i = 0; i < beaker._triggerStrings.getNumStrings(); i++) {
    Log.trace("%d = %S, ", i, beaker._triggerStrings.getString(i));
  }
  Log.traceln(F("\n"));

  Log.traceln(F("MainMachine!"));

  MainMachine& system = MainMachine::getInstance();
  system.begin();

  Log.traceln("MainMachine printTo: %p ", system);
  Log.traceln("MainMachine State printTo: %p ", system._stateStrings);
  for (uint16_t i = 0; i < system._stateStrings.getNumStrings(); i++) {
    Log.trace("%d = %S, ", i, system._stateStrings.getString(i));
  }
  Log.traceln(F("\n"));
  Log.traceln("MainMachine Trigger printTo: %p", system._triggerStrings);
  for (uint16_t i = 0; i < system._triggerStrings.getNumStrings(); i++) {
    Log.trace("%d = %S, ", i, system._triggerStrings.getString(i));
  }
  Log.traceln(F("\n"));
  Log.traceln(F("Done!"));

  system.runSubMachines();
  system.runSubMachines();
  system.trigger(MainMachine::Triggers::Deploy);
  system.runSubMachines();
  system.runSubMachines();
}

void loop() {
    //  MainMachine& system = MainMachine::getInstance();
    //  system.runSubMachines();
}

// handle diagnostic informations given by assertion and abort program
// execution:
void __assert(const char* __func, const char* __file, int __lineno,
    const char* __sexp) {
  // transmit diagnostic informations through serial link.
  char out[255];
  sprintf_P(out, PSTR("FATAL: %s:%d: assert: '%s' in %s()"), __file, __lineno,
      __sexp, __func);
  Log.fatalln(out);

#ifdef HALT_ON_ASSERT
  Log.fatalln(F("program execution halted"));
#endif

  // abort program execution.
  Serial.flush();

#ifdef HALT_ON_ASSERT
  abort();
#endif
}
