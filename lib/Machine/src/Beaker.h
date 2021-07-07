
#pragma once

BEGIN_FLASH_STRING_TABLE_CLASS(BeakerStates)
ADD_FLASH_STRING("error")
ADD_FLASH_STRING("zero")
ADD_FLASH_STRING("one")
ADD_FLASH_STRING("two")
ADD_FLASH_STRING("three")
END_FLASH_STRING_TABLE()

BEGIN_FLASH_STRING_TABLE_CLASS(BeakerTriggers)
ADD_FLASH_STRING("None")
ADD_FLASH_STRING("Oil")
ADD_FLASH_STRING("Water")
ADD_FLASH_STRING("Juice")
ADD_FLASH_STRING("Beer")
END_FLASH_STRING_TABLE()

class Beaker : public Machine {
 public:
  enum States
  {
    error,
    zero,
    one,
    two,
    three
  };

  enum Triggers
  {
    None,
    Oil,
    Water,
    Juice,
    Beer
  };

  Beaker();

  // void on_enter(StateType  _state);
  // void on_state(StateType  _state);
  // void on_exit(StateType  _state);

  // /**
  //  * @brief Flash-memory based strings for the States
  //  * 
  //  */
  // StringTable _stateStrings;

  // /**
  //  * @brief Flash-memory based strings for the States
  //  * 
  //  */
  // StringTable _triggerStrings;

 public:
  /**
   *
   * @brief Initiazlie object. Called at system startup.
   *
   * @return true if setup worked
   * @return false if setup failed
   */
  bool begin();

  /**
   * @brief Runs the sub-system's _state machine. Called from the holding
   * object's `runStateMachines()`; the top level machine (`System`)'s
   * `runMachine` is called from `loop()`.
   *
   */
  virtual void runStateMachines();

  /**
   * @brief Set the Trigger objectSets the _trigger of the next _state to transition to. 
   * The _trigger will happen async. Trigger::None means stay in current _state.
   * 
   * @param _trigger 
   */
  virtual void setTrigger(TriggerType _trigger);

  /**
   * @brief Set the Current State object - Used for diagnostics; called from
   * on_enter of each _state in statemachine
   *
   * @param _state
   */
  virtual void setCurrentState(StateType  _state);

  /**
   * @brief Do machine work while in a _state (to be called from `on_state` handlers).
   * 
   * @param currentState - passed as a convenience so on_state doesn't have to call getState().
   * @return Trigger - a _trigger for a _state transition. The `on_state` will then use this to
   * initate a _state transition (by calling the appropriate `setTrigger()`)
   */
  virtual TriggerType process(StateType  currentState);

  /**
   * @brief Returns current (cached) _state of the machine
   *
   * @return States
   */
  //virtual Beaker::States getState();

  /**
   * @brief `Printable::printTo` - prints the current State & Trigger
   * 'State(Trigger)'
   *
   * @param p
   * @return size_t
   */
  virtual size_t printTo(Print& p) const;

 private:
};