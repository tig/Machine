#include <Arduino.h>
#include <ArduinoLog.h>
#include <assert.h>

#include <Machine.h>
#include "Beaker.h"

bool Beaker::begin() {
  Log.traceln(F("Beaker::begin()"));

  // Allocate
  allocateFsm(Beaker::States::three, Beaker::Triggers::Beer);

  for (StateType i = Beaker::States::error; i <= Beaker::States::three; i++) {
    _rgpStates[i] = new State(
        []() {
          TRACE_STATE_FN(Beaker, on_enter, true);
        },
        []() {
          TRACE_STATE_STATE_FN(Beaker, false);
        },
        []() {
          TRACE_STATE_FN(Beaker, on_exit, true);
        });
  }
  // Set initial state
  setStartState(three);

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

TriggerType Beaker::process(StateType stateCalling) {
  // Handle work (from on_state). Return new state transition trigger (or None)
  // DO NOT set trigger or call setTrigger(); the calling on_state will do so
  TriggerType trigger = Triggers::None;

  return trigger;
}
