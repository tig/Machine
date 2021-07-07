#include <Arduino.h>
#include <ArduinoLog.h>
#include <assert.h>
#include <FlashStringTable.h>
#include <Fsm.h>
#include <FlashStringTable.h>

#include "Machine.h"

Machine _machine;

Machine::Machine()
    : _stateStrings(_progmem_MachineStates),
      _triggerStrings(_progmem_MachineTriggers),
      _state(States::error),
      _trigger(Triggers::None) {
  Log.traceln("Machine::Machine()");

  // Don't do anything in here because these objects are static
  // and thus constructors are called before Serial or other
  // stuff is available, making debugging hard. Use begin() instead.
}

bool Machine::begin() {
  Log.traceln("Machine::begin()");

  assert(States::error + 1 == _stateStrings.getNumStrings());

  _rgpStates = nullptr;
  _pFsm = nullptr;

  // _rgpStates = (State **)new State[(Machine::States::error + 1)];
  // _rgpStates[Machine::States::error] = new State(
  //     []() {
  //       Log.traceln("on_enter");
  //       _machine.setCurrentState(Machine::States::error);
  //     },
  //     []() {
  //       Log.traceln("on_state");
  //     },
  //     []() {
  //       Log.traceln("on_exit");
  //     });

  // _pFsm = new Fsm(_rgpStates[_state]);
  // _pFsm->add_transition(_rgpStates[Machine::States::error],
  //     _rgpStates[Machine::States::error], Machine::Triggers::None, nullptr);

  return true;
}

void Machine::runStateMachines() {
  // trigger() ends up callilng old->on_exit, new->on_enter

  // We should never run
  assert (_rgpStates == nullptr);
  assert (_pFsm == nullptr);

  _pFsm->trigger(_trigger);

  // Reset the trigger
  setTrigger(None);

  // runs the current state's on_state then checks for
  // timed transitions
  _pFsm->run_machine();

  // the current state's on_state has returned
  // returning from here puts us back in loop()...
}

void Machine::setTrigger(TriggerType trigger) {
  Log.traceln(F("Machine::setTrigger(%S)"), _triggerStrings.getString(trigger));
  // Trigger a state transition (asynchronously)
  // TODO: Redraw dispay?

  trigger = trigger;
}

void Machine::setCurrentState(StateType state) {
  // For diagnostics

  // TODO: Redraw dispay
}

TriggerType Machine::process(StateType currentState) {
  Log.traceln(F("Machine::process(%S)"), _stateStrings.getString(currentState));
  // Handle work (from on_state). Return new state transition trigger (or None)
  // DO NOT set trigger or call setTrigger(); the calling on_state will do so
  TriggerType trigger = Triggers::None;

  return trigger;
}

size_t Machine::printTo(Print &p) const {
  return p.print(_stateStrings.getString((int)_state));
};
