#include <Arduino.h>
#include <ArduinoLog.h>
#include <assert.h>
#include <FlashStringTable.h>
#include <Fsm.h>
#include <FlashStringTable.h>

#include "Machine.h"
#include "Beaker.h"

//size_t Beaker::printTo(Print &p) const { return p.print(getString(state)); };

Beaker::Beaker() {
  Log.traceln("Beaker::Beaker()");
  _stateStrings.init(_progmem_BeakerStates);
  _triggerStrings.init(_progmem_BeakerTriggers);

  // not really needed as Machine's constructor inits; but if a distinct intial state
  // or trigger were needed.
  _state = (StateType)Beaker::States::error;
  _trigger = (TriggerType)Beaker::Triggers::None;

  assert(Beaker::States::three + 1 == _stateStrings.getNumStrings());
}

// void Beaker::on_enter(StateType state) {
//   switch (state) {
//     case Beaker::States::error:

//       break;

//     default:
//       break;
//   }
// }
// void Beaker::on_state(StateType state) {
// }
// void Beaker::on_exit(StateType state) {
// }

bool Beaker::begin() {
  Log.traceln("Beaker::begin()");

  return true;
}

void Beaker::runStateMachines() {
  // trigger() ends up callilng old->on_exit, new->on_enter
  //logStatus(F("Trigger: %S"), _triggerStrings.getString(trigger));
  _pFsm->trigger(_trigger);

  // Reset the trigger
  setTrigger(None);

  // runs the current state's on_state then checks for
  // timed transitions
  _pFsm->run_machine();

  // the current state's on_state has returned
  // returning from here puts us back in loop()...
}

void Beaker::setTrigger(TriggerType trigger) {
  // Trigger a state transition (asynchronously)
  // TODO: Redraw dispay?

  trigger = trigger;
  _pFsm->trigger(trigger);
}

void Beaker::setCurrentState(StateType state) {
  // For diagnostics

  // TODO: Redraw dispay
}

TriggerType Beaker::process(StateType currentState) {
  // Handle work (from on_state). Return new state transition trigger (or None)
  // DO NOT set trigger or call setTrigger(); the calling on_state will do so
  TriggerType trigger = Triggers::None;

  return trigger;
}

size_t Beaker::printTo(Print &p) const {
  return p.print(_stateStrings.getString((int)_state));
};