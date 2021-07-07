#pragma once

BEGIN_FLASH_STRING_TABLE_CLASS(MachineStates)
ADD_FLASH_STRING("error")
END_FLASH_STRING_TABLE()

BEGIN_FLASH_STRING_TABLE_CLASS(MachineTriggers)
ADD_FLASH_STRING("None")
END_FLASH_STRING_TABLE()

// Because c/c++ does not support polymorhpic enums we can't really pass our nice
// enums around strongly typed. So we just an int. And silly defines to make the code
// more readable
#define StateType   uint16_t
#define TriggerType uint16_t

/**
 * @brief Abstract (pure v) base class for subystems that are run by a state machine.
 *
 */
class Machine : public Printable {
 public:
  /**
 * @brief The states the Machine's FSM can be in. Must have the same
 * number of items as _stateStrings[]
 * 
 */
  enum States
  { error };

  /**
 * @brief The events that can trigger the Machine FSM to transition to another state.
 *  Must have the same number of items as _stateStrings[]
 * 
 */
  enum Triggers
  { None };

  Machine();
  Machine(StateType initialState)
      : Machine() { _state = initialState; };

  void on_enter(StateType state);
  void on_state(StateType state);
  void on_exit(StateType state);

  /**
   * @brief Flash-memory based strings for the States
   * 
   */
  StringTable _stateStrings;

  /**
   * @brief Flash-memory based strings for the States
   * 
   */
  StringTable _triggerStrings;

 public:
  /**
   *
   * @brief Initialize object. Called at system startup.
   *
   * @return true if setup worked
   * @return false if setup failed
   */
  bool begin();

  /**
   * @brief Runs the sub-system's state machine. Called from the holding
   * object's `runStateMachines()`; the top level machine (`System`)'s
   * `runMachine` is called from `loop()`.
   *
   */
  virtual void runStateMachines();

  /**
   * @brief Set the Trigger objectSets the trigger of the next state to transition to. 
   * The trigger will happen async. Trigger::None means stay in current state.
   * 
   * @param trigger 
   */
  virtual void setTrigger(TriggerType trigger);

  /**
   * @brief Set the Current State object - Used for diagnostics; called from
   * on_enter of each state in statemachine
   *
   * @param state
   */
  virtual void setCurrentState(StateType state);

  /**
   * @brief Do machine work while in a state (to be called from `on_state` handlers).
   * 
   * @param currentState - passed as a convenience so on_state doesn't have to call getState().
   * @return Trigger - a trigger for a state transition. The `on_state` will then use this to
   * initate a state transition (by calling the appropriate `setTrigger()`)
   */
  virtual TriggerType process(StateType currentState);

  /**
   * @brief `Printable::printTo` - prints the current State & Trigger
   * 'State(Trigger)'
   *
   * @param p
   * @return size_t
   */
  virtual size_t printTo(Print& p) const;

  /**
   * @brief Current state of the machine
   *
   */
  StateType _state;

  /**
   * @brief Set by a state machine function to trigger next state
   *
   */
  TriggerType _trigger;

  State** _rgpStates;  
  Fsm* _pFsm;
  
};

