#pragma once

#include <functional>
#include <vector>
#include <map>
#include <string>
using namespace std;

#include <FlashStringTable.h>
#include <Fsm.h>

// BEGIN_FLASH_STRING_TABLE_CLASS(MachineStates)
// ADD_FLASH_STRING("error")
// END_FLASH_STRING_TABLE()

BEGIN_FLASH_STRING_TABLE_CLASS(MachineTriggers)
ADD_FLASH_STRING("None")
END_FLASH_STRING_TABLE()

#pragma GCC diagnostic ignored "-Wunused-variable"

/**
 * @brief Shortcut for ensuring a on_enter function can be setup/traced easily. 
 * e.g. STATE_ONENTER_FN(MainMachine);
 * 
 * These local vars are defined 
 *  machine.- instance of the Machine based class
 *  machine_state - current state
 * 
 * @param classname classname of the `Machine` subclass.
 */
#define STATE_ONENTER_FN(classname)                        \
  classname& machine = classname::getInstance();           \
  MachineState* machine_state = machine.getCurrentState(); \
  machine.setDirty(true);                                                   \
  machine.notifyStateChange();

/**
 * @brief Shortcut for ensuring a on_state function can be setup/traced easily. 
 * e.g. STATE_ONSTATE_FN(MainMachine, true);
 * 
 * These local vars are defined 
 *  machine.- instance of the Machine based class
 *  machine_state - current state
 *  machine_trigger - trigger suggested by calling machine.on_state()
 * 
 * @param classname classname of the `Machine` subclass.
 * @param trace if `true` Log.traceln will be called 
 */
#define STATE_ONSTATE_FN(classname, trace)                                   \
  classname& machine = classname::getInstance();                             \
  MachineState* machine_state = machine.getCurrentState();                   \
  if (trace) {                                                               \
    Log.traceln(F("  " #classname " state: %p  - on_state"), machine_state); \
  }                                                                          \
  TriggerType machine##_trigger = machine.on_state();

/**
 * @brief Shortcut for ensuring a on_enter/state/exit function can be setup/traced easily
 * e.g. STATE_ONEXIT_FN(MainMachine, on_enter, true);
 * 
 * These local vars are defined 
 *  machine.- instance of the Machine based class
 *  machine_state - current state
 * 
 * @param classname classname of the `Machine` subclass.
 * @param fn on_enter, on_state, or on_exit
 * @param trace if `true` Log.traceln will be called 
 */
#define STATE_ONEXIT_FN(classname, trace)                                   \
  classname& machine = classname::getInstance();                            \
  MachineState* machine_state = machine.getCurrentState();                  \
  if (trace) {                                                              \
    Log.traceln(F("  " #classname " state: %p  - on_exit"), machine_state); \
  }

/**
 * @brief shortcut for calling `defineState()`; puts `state` in 
 * flash memory.
 * 
 */
#define DEFINE_STATE(state) defineState(&state, F("" #state "")

// Because c/c++ does not support polymorhpic enums we can't really pass our nice
// enums around strongly typed. So we just an int. And silly defines to make the code
// more readable
// using StateType = uint16_t;
using TriggerType = uint16_t;

// template<typename T>
// class EnumWrapper
// {
//     T Object;
//         public:
//             operator T&(){return Object;}
// };

// class StateType :public EnumWrapper<uint16_t> {};
// class TriggerType :public EnumWrapper<uint16_t> {};

class MachineState : public State, public Printable {
 public:
  // The states this machine can be in
  MachineState(uint16_t index, const __FlashStringHelper* name,
      void (*on_enter)(), void (*on_state)(), void (*on_exit)()) : MachineState() {
    index = index;
    name = name;
    on_enter = on_enter;
    on_state = on_state;
    on_exit = on_exit;
  };
  // MachineState(const __FlashStringHelper* name){
  //   name = name;
  // };
  MachineState(){};

  uint16_t index = 0;
  const __FlashStringHelper* name = nullptr;

  /**
   * @brief `Printable::printTo` - prints the current State
   *
   * @param p
   * @return size_t
   */
  virtual size_t printTo(Print& p) const {
    return p.print(name);
  };
};

/**
 * @brief Abstract (pure v) base class for subystems that are run by a state machine.
 *
 */
class Machine : public Printable {
 public:
  /** 
   * States
   */
  MachineState error;

  /**
   * @brief The events that can trigger the Machine FSM to transition to another state.
   *  Must have the same number of items as _stateStrings[]
   * 
   */
  enum Triggers
  { None };

  /**
   * @brief Flash-memory based strings for the States
   * 
   */
  StringTable _triggerStrings = _progmem_MachineTriggers;

  /**************** BEGIN Fsm Wrappers *****/

  /**
   * @brief Enables testing if defineState has been called to allocate states.
   * 
   * @return true yes
   * @return false no
   */
  bool isFsmInitialized() const {
    return _pFsm != nullptr;
  }

  MachineState* defineState(MachineState* state, const __FlashStringHelper* name,
      void (*on_enter)(),
      void (*on_state)(),
      void (*on_exit)());
  /**
   * @brief Allocates the state machine implementation. Call from the end of 
   * implementaitons of `begin()` before using `_rgpStates`
   * 
   */
  void setStartState(MachineState* state);

  /**
   * trigger transition with the event 
   * @param trigger enum that defines the trigger
   * @param bool true (default) causes state transition to happen during the call to trigger (synchronously)
   * @param bool false sets a flag such that the state transition will happen in the next call to run_machine()
   */
  void trigger(TriggerType trigger, bool immediate = true);

  /**
   * @brief checks whether an trigger will cause a transition from the current state.
   * 
   * @param trigger enum that defines the trigger
   * @return `true` if `trigger` is valid for the current state; `false` otherwise.
   */
  bool isTriggerValid(TriggerType trigger);

  /**
   * @brief Runs the state machine. Called from the holding
   * object's `runSubMachines()`; the top level machine (`System`)'s
   * `runMachine` is called from `loop()`.
   *
   */
  virtual void runMachine();

  /**
   * @brief Adds a transition
   * 
   * @param stateFrom
   * @param stateTo
   * @param trigger 
   * @param on_transition 
   */
  void addTransition(MachineState* stateFrom, MachineState* stateTo, TriggerType trigger, void (*on_transition)() = nullptr);

  /**
   * @brief Adds a timed transition
   * 
   * @param stateFrom
   * @param stateTo
   * @param interval 
   * @param on_transition 
   */
  void addTimedTransition(MachineState* stateFrom, MachineState* stateTo, unsigned long interval, void (*on_transition)() = nullptr);

  // /**
  //  * looks for the current state's timed transitions to the target state and resets the timer
  //  * @param stateTo target state to reset the timed transition for. If None reset all current state timers
  //  */
  // void resetTimedTransition(StateType stateTo);

  /**
   * returns current state (helpful if the same handler is used to drive many similar states)
   * @return index of current state
   */
  MachineState* getState(uint16_t i) { return _rgpMachineStates[i]; }

  MachineState* getState(const char* name) {
    for (uint16_t i = 0; i < _numStates; i++) {
      if (0 == strcmp_P(name, (const char*)(_rgpMachineStates[i]->name))) {
        return _rgpMachineStates[i];
      }
    }
    return nullptr;
  }

  uint16_t getNumStates() { return _numStates; };

  /**
   * returns current state (helpful if the same handler is used to drive many similar states)
   * @return Current state
   */
  MachineState* getCurrentState() const;

  /**************** END Fsm Wrappers *****/

  /**
   *
   * @brief Initialize state machine. Should be called at system startup.
   * Subclasses should override this and call Machine::begin() before doing their
   * own begin() processing.
   *
   * @return true if setup worked
   * @return false if setup failed
   */
  virtual bool begin();

  /**
   * @brief Set the Trigger objectSets the trigger of the next state to transition to. 
   * The trigger will happen async. Trigger::None means stay in current state.
   * 
   * @param trigger 
   */
  virtual void setTrigger(TriggerType trigger);

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
  virtual TriggerType on_state();

  /**
   * @brief `Printable::printTo` - prints the current State & Trigger
   * 'State(Trigger)'
   *
   * @param p
   * @return size_t
   */
  virtual size_t printTo(Print& p) const;

  /**
   * @brief Set by a state machine function to trigger next state
   *
   */
  TriggerType _trigger = 0;

  /**
   * @brief name of the statemachine.
   * 
   */
  const __FlashStringHelper* name() const { return (_name != nullptr) ? _name : F(""); }
  void setName(const __FlashStringHelper* n) { _name = n; }

  /**
   * @brief registers a callback function that will be called whenever a new state is
   * entered.
   * 
   * @param func 
   */
  void registerStateChange(void (*const func)(Machine& machine)) {
    _notifyCallbacks.emplace_back(func);
  }

  void notifyStateChange() {
    for (const auto& cb : _notifyCallbacks)
      cb(*this);
  }

  /**
   * @brief Set to true when there is a reason for the state machine to 
   * run.
   * 
   * @param dirty 
   */
  void setDirty(bool dirty) { _dirty = dirty; }

  /**
   * @brief Indicates that the state machine has processing that needs to be done.
   * 
   */
  bool dirty() { return _dirty; }

 private:
  /**
   * @brief Dynamically allocated array of pointers to MachineStates
   *
   */
  MachineState** _rgpMachineStates = nullptr;

  /**
   * @brief Current state of the machine
   *
   */
  MachineState* _currentState = nullptr;

  /**
   * @brief pointer to arduino-fsm `Fsm` instance
   * 
   */
  Fsm* _pFsm = nullptr;
  std::vector<std::function<void(Machine& machine)>> _notifyCallbacks;
  const __FlashStringHelper* _name;
  uint16_t _numStates = 0;
  uint16_t _numTriggers = Triggers::None + 1;
  bool _dirty = true;

  // =======================================================
  // Singleton support
  // https://stackoverflow.com/a/1008289/297526
 public:
  /**
   * @brief Returns the single instance object
   *
   */
  static Machine& getInstance() {
    static Machine instance;  // Guaranteed to be destroyed.
                              // Instantiated on first use.
    return instance;
  }

  // NOTE: Normally for a singleton the constructor would be private
  // Prohibiting External Constructions. But Machine is
  // actually abstract and we only ...
  // TODO: Fix this bs.
 public:
  Machine() : _name(nullptr) {
    // Don't do anything in here because these objects are static
    // and thus constructors are called before Serial or other
    // stuff is available, making debugging hard. Use begin() instead.
  }

 private:
  // C++ 11
  // =======
  // We can use the better technique of deleting the methods
  // we don't want.
 public:
  // This breaks printTo
  //Machine(Machine const&) = delete;
  void operator=(Machine const&) = delete;
  // =======================================================
};
