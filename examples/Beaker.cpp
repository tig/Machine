#include <Arduino.h>
#include <ArduinoLog.h>
#include <assert.h>

#include <Machine.h>
#include "Beaker.h"

bool Beaker::begin() {
  Log.traceln(F("Beaker::begin()"));

  DEFINE_STATE(error), 
  []() {
    STATE_ONEXIT_FN(Beaker, on_enter, true);
    Log.traceln(F("Hello %S"), machine_state);
  },
  []() {
    STATE_ONSTATE_FN(Beaker, false);
  },
  []() {
    STATE_ONEXIT_FN(Beaker, on_exit, false);
  });

  DEFINE_STATE(zero),
  []() {
    STATE_ONEXIT_FN(Beaker, on_enter, true);
  },
  []() {
    STATE_ONSTATE_FN(Beaker, false);
  },
  []() {
    STATE_ONEXIT_FN(Beaker, on_exit, false);
  });

  setStartState(&error);

  addTransition(&error, &zero, Triggers::Juice);
  addTransition(&zero, &error, Triggers::Beer);

  return true;
}

void Beaker::runSubMachines() {
  // trigger() ends up callilng old->on_exit, new->on_enter
  //logStatus(F("Trigger: %S"), _triggerStrings.getString(trigger));
  trigger(_trigger);

  // Reset the trigger
  setTrigger(None);

  // runs the current state's on_state then checks for
  // timed transitions
  runMachine();

  // the current state's on_state has returned
  // returning from here puts us back in loop()...
}

TriggerType Beaker::on_state() {
  // Handle work (from on_state). Return new state transition trigger (or None)
  // DO NOT set trigger or call setTrigger(); the calling on_state will do so
  TriggerType trigger = Triggers::None;

  return trigger;
}
