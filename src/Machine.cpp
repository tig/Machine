#include <Arduino.h>
#include <ArduinoLog.h>
#include <assert.h>

#include "Machine.h"

MachineState* Machine::defineState(MachineState* pms, const __FlashStringHelper* name,
    void (*on_enter)(),
    void (*on_state)(),
    void (*on_exit)()) {
  //MachineState* pms = new MachineState(_numStates, name, on_enter, on_state, on_exit);
  pms->index = _numStates;
  pms->name = name;
  pms->on_enter = on_enter;
  pms->on_state = on_state;
  pms->on_exit = on_exit;
  _rgpMachineStates = (MachineState**)realloc(_rgpMachineStates, sizeof(MachineState*) * (_numStates + 1));
  _rgpMachineStates[_numStates] = pms;
  _numStates++;
  return pms;
}

void Machine::setStartState(MachineState* state) {
  //Log.traceln(F("Machine::setStartState(%p)"), state);
  assert(_rgpMachineStates != nullptr);
  assert(_pFsm == nullptr);
  assert(state->index < _numStates);

  for (uint16_t i = 0; i < _numStates; i++) {
    assert(_rgpMachineStates[i] != nullptr);
  }
  _pFsm = new Fsm(state);
};

void Machine::trigger(TriggerType trigger, bool immediate) {
  //Log.traceln(F("Machine::trigger(%d, %d)"), trigger, immediate);
  assert(_rgpMachineStates != nullptr);
  assert(_pFsm != nullptr);
  //assert(isTriggerValid(trigger));
  _pFsm->trigger(trigger, immediate);
}

bool Machine::isTriggerValid(TriggerType trigger) {
  if (_pFsm == nullptr || _rgpMachineStates == nullptr) return false;
  // assert(_rgpMachineStates != nullptr);
  // assert(_pFsm != nullptr);
  if (trigger == Triggers::None) {
    return true;
  }
  return _pFsm->is_valid_event(trigger);
}

void Machine::runMachine() {
  //Log.traceln(F("Machine::runMachine()"));
  assert(_rgpMachineStates != nullptr);
  assert(_pFsm != nullptr);
  _pFsm->run_machine();
}

void Machine::addTransition(MachineState* stateFrom, MachineState* stateTo, TriggerType trigger, void (*on_transition)()) {
  //Log.traceln(F("Machine::addTransition(%p, %p, %d)"), stateFrom, stateTo, trigger);
  assert(_pFsm != nullptr);
  assert(_rgpMachineStates != nullptr);
  assert(stateFrom != nullptr);
  assert(stateTo != nullptr);
  assert(stateFrom->index < _numStates);
  assert(stateTo->index < _numStates);
  //assert(trigger < _numTriggers);

  _pFsm->add_transition(stateFrom, stateTo, trigger, on_transition);
}

void Machine::addTimedTransition(MachineState* stateFrom, MachineState* stateTo, unsigned long interval, void (*on_transition)()) {
  //Log.traceln(F("Machine::addTimedTransition(%p, %p, %d)"), stateFrom, stateTo, interval);
  assert(_pFsm != nullptr);
  assert(_rgpMachineStates != nullptr);
  assert(stateFrom != nullptr);
  assert(stateTo != nullptr);
  assert(stateFrom->index < _numStates);
  assert(stateTo->index < _numStates);

  _pFsm->add_timed_transition(stateFrom, stateTo, interval, on_transition);
}

MachineState* Machine::getCurrentState() const {
  if (_pFsm == nullptr) {
    return nullptr;
  }
  assert(_rgpMachineStates != nullptr);

  MachineState* state = nullptr;
  MachineState* pCurState = (MachineState*)_pFsm->get_current_state();
  if (pCurState == nullptr) {
    state = nullptr;
  } else {
    for (uint16_t i = 0; i < _numStates; i++) {
      if (_rgpMachineStates[i] == pCurState) {
        state = _rgpMachineStates[i];
        break;
      }
    }
  }
  assert(state->index < _numStates);
  return state;
}

bool Machine::begin() {
  //Log.traceln(F("Machine::begin()"));
  assert(_pFsm == nullptr);
  assert(_rgpMachineStates == nullptr);

  // this code block illustrates a typical `begin()` implementation
  MachineState* startState = defineState(
      &error, F("error"),
      []() {
        TRACE_STATE_FN(Machine, on_enter, true);
  },
      []() {
        TRACE_STATE_STATE_FN(Machine, false);
  },
      []() {
        TRACE_STATE_FN(Machine, on_exit, true);
  });

  setStartState(startState);

  return true;
}

void Machine::runSubMachines() {
  //Log.traceln(F("Machine::runSubMachines()"));
  assert(_rgpMachineStates != nullptr);
  assert(_pFsm != nullptr);

  // Trigger an outstanding trigger if there is one
  if (_trigger != Machine::Triggers::None) {
    // trigger() ends up callilng old->on_exit, new->on_enter
    //Log.traceln(F("Machine::runSubMachines - trigger(%S)"), _triggerStrings.getString(_trigger));
    trigger(_trigger, true);
    // Reset the trigger
    //Log.traceln(F("Machine::runSubMachines - trigger(%S) completed, resetting trigger and exiting..."), _triggerStrings.getString(_trigger));
    setTrigger(Machine::Triggers::None);
    return;
  }

  // runs the current state's on_state then checks for
  // timed transitions
  runMachine();

  // the current state's on_state has returned
  // returning from here puts us back in loop()...
}

void Machine::setTrigger(TriggerType trigger) {
  assert(_rgpMachineStates != nullptr);
  assert(_pFsm != nullptr);
  // if (trigger != None) {
  //   Log.traceln(F("Machine::setTrigger(%S) - invalid trigger (current state: %p)"), _triggerStrings.getString(trigger), getCurrentState()); 
  // }

  _trigger = trigger;
}

TriggerType Machine::on_state() {
  assert(_rgpMachineStates != nullptr);
  assert(_pFsm != nullptr);

  //Log.traceln(F("Machine::on_state(%S)"), _stateStrings.getString(stateCalling));
  // Handle work (from on_state). Return new state transition trigger (or None)
  // DO NOT set trigger or call setTrigger(); the calling on_state will do so
  TriggerType trigger = Triggers::None;

  return trigger;
}

size_t Machine::printTo(Print& p) const {
  return p.print(_pFsm != nullptr ? getCurrentState()->name : F("<errr>"));
};
