#include <Arduino.h>
#include <ArduinoLog.h>
#include <assert.h>

#include "Machine.h"

void Machine::reset() {
  // if (_rgpMachineStates != nullptr) {
  //   free(_rgpMachineStates);
  //   _rgpMachineStates = nullptr;
  // }
  //_numStates = 0;

  if (_pFsm != nullptr) {
    delete _pFsm;
    _pFsm = nullptr;
  }
  //_notifyCallbacks.clear();
  //uint16_t _numTriggers = Triggers::None + 1;
  _dirty = true;  
}

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
  setDirty(true);
};

void Machine::trigger(TriggerType trigger, bool immediate) {
  Log.traceln(F("[%S]::trigger(%S, %T) - dirty = %T"), name(), _triggerStrings.getString(trigger), immediate, dirty());
  assert(_rgpMachineStates != nullptr);
  assert(_pFsm != nullptr);
  //assert(isTriggerValid(trigger));
  if (!immediate) {
    setDirty(true);
  }
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
  //Log.traceln(F("[%S]::runMachine() - dirty = %T, trigger = %S"), name(), dirty(), _triggerStrings.getString(_trigger));
  assert(_rgpMachineStates != nullptr);
  assert(_pFsm != nullptr);

  // Trigger an outstanding trigger if there is one
  if (_trigger != Machine::Triggers::None) {
    // trigger() ends up callilng old->on_exit, new->on_enter
    //Log.traceln(F("[%S]::runMachine - _trigger = %S"), name(), _triggerStrings.getString(_trigger));
    trigger(_trigger, true);
    // Reset the trigger
    //Log.traceln(F("Machine::runMachine - trigger(%S) completed, resetting trigger and exiting..."), _triggerStrings.getString(_trigger));
    setTrigger(Machine::Triggers::None);
    return;
  }

  if (dirty()) {
    // runs the current state's on_state then checks for
    // timed transitions
    _pFsm->run_machine();
    // the current state's on_state has returned
    setDirty(false);

    // if, during run_machine() setTrigger was called with a valid trigger, ensure dirty is set
    if (_pFsm->is_valid_event(_trigger)) {
      setDirty(true);
    }
  }
  // returning from here puts us back in loop()...
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
  setDirty(true);
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
  setDirty(true);
}

void Machine::resetTimedTransition(MachineState* stateTo) {
  //Log.traceln(F("Machine::addTimedTransition(%p, %p, %d)"), stateFrom, stateTo, interval);
  assert(_pFsm != nullptr);
  assert(_rgpMachineStates != nullptr);
  assert(stateTo != nullptr);
  assert(stateTo->index < _numStates);

  _pFsm->reset_timed_transition(stateTo);
  setDirty(true);
}

MachineState* Machine::getCurrentState() const {
  assert(_pFsm != nullptr);
  if (_pFsm == nullptr) {
    return nullptr;
  }
  assert(_rgpMachineStates != nullptr);

  MachineState* state = nullptr;
  MachineState* pCurState = (MachineState*)_pFsm->get_current_state();
  for (uint16_t i = 0; i < _numStates; i++) {
    if (_rgpMachineStates[i] == pCurState) {
      state = _rgpMachineStates[i];
      break;
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
        STATE_ONENTER_FN(Machine);
      },
      []() {
        STATE_ONSTATE_FN(Machine, false);
      },
      []() {
        STATE_ONEXIT_FN(Machine, false);
      });

  setStartState(startState);

  return true;
}

void Machine::setTrigger(TriggerType trigger) {
  assert(_rgpMachineStates != nullptr);
  assert(_pFsm != nullptr);
  if (trigger != None) {
    Log.traceln(F("[%S]::setTrigger(%S) current state: %p"), name(), _triggerStrings.getString(trigger), getCurrentState());
  }
  if (isTriggerValid(trigger)) {
    setDirty(true);
  }
  _trigger = trigger;
}

/**
   * @brief on_state gets called from the active state when loop() happens. It is called
   * before any code in the on_state lambda is run and is used to do any generic analysis
   * that would apply to any state. 
   * 
   * Return new state transition trigger (or None).
   * 
   * State specific logic should not go here; put that in the on_state lambda instead.
   * 
   * Avoid calling setTrigger(); the calling on_state lambda will do that.
   * 
   * @return TriggerType - the suggested Trigger based on whatever analysis was performed.
   * The `on_state` will then use this to initate a state transition (by calling the appropriate `setTrigger()`
   */
TriggerType Machine::on_state() {
  assert(_rgpMachineStates != nullptr);
  assert(_pFsm != nullptr);

  //Log.traceln(F("Machine::on_state(%S)"), _stateStrings.getString(stateCalling));
  // Handle work (from on_state). Return new state transition trigger (or None)
  // DO NOT set trigger or call setTrigger(); the calling on_state will do so
  TriggerType trigger = Triggers::None;
}

size_t Machine::printTo(Print& p) const {
  size_t n = p.print(name());
  n += p.print(F(" = "));
  return n + p.print(isFsmInitialized() ? getCurrentState()->name : F("<err>"));
};
