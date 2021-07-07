#pragma once

BEGIN_FLASH_STRING_TABLE_CLASS(SystemStates)
ADD_FLASH_STRING("error")
ADD_FLASH_STRING("startup")
ADD_FLASH_STRING("transitioning")
ADD_FLASH_STRING("retracted")
ADD_FLASH_STRING("deployed")
ADD_FLASH_STRING("deploy_door_opening")
ADD_FLASH_STRING("deploy_door_closing")
ADD_FLASH_STRING("retract_door_opening")
ADD_FLASH_STRING("retract_door_closing")
ADD_FLASH_STRING("slider_deploying")
ADD_FLASH_STRING("slider_retracting")
END_FLASH_STRING_TABLE()

BEGIN_FLASH_STRING_TABLE_CLASS(SystemTriggers)
ADD_FLASH_STRING("None")
ADD_FLASH_STRING("Retracted")
ADD_FLASH_STRING("Deployed")
ADD_FLASH_STRING("Transitioning")
ADD_FLASH_STRING("DoorOpened")
ADD_FLASH_STRING("DoorClosed")
ADD_FLASH_STRING("Deploy")
ADD_FLASH_STRING("Retract")
ADD_FLASH_STRING("Abort")
ADD_FLASH_STRING("Reset")
ADD_FLASH_STRING("DoorTimeout")
ADD_FLASH_STRING("DoorNotOpen")
ADD_FLASH_STRING("DoorError")
ADD_FLASH_STRING("SliderStuck")
ADD_FLASH_STRING("SliderError")
END_FLASH_STRING_TABLE()

const char _logStatusFmt[] = "Door: %s | Slider: %s, Fwd: %dmm, Rwd: %dmm, Speed: %d, Sp Fract: %d";
static const char _s_logStatusFmt[] = "Door: %s | Slider: %s, Fwd: %dmm, Rwd: %dmm, Speed: %d, Sp Fract: %d";
const char _f_logStatusFmt[] PROGMEM = "Door: %s | Slider: %s, Fwd: %dmm, Rwd: %dmm, Speed: %d, Sp Fract: %d";
// make static so it's not dynamically allocated
static char _logStatus[255];

class System : public Machine {
 public:
  enum States
  {
    error,
    startup,
    transitioning,
    retracted,
    deployed,
    deploy_door_opening,
    deploy_door_closing,
    retract_door_opening,
    retract_door_closing,
    slider_deploying,
    slider_retracting,
  };

  enum Triggers
  {
    None,
    // Status
    Retracted,
    Deployed,
    Transitioning,
    DoorOpened,
    DoorClosed,
    // Commands
    Deploy,
    Retract,
    Abort,
    Reset,
    // Errors
    DoorTimeout,
    DoorNotOpen,
    DoorError,
    SliderStuck,
    SliderError
  };

  //System();
  //System() : api(), slider(), door(), _status(Triggers::Error), _trigger(Triggers::None) {}

  // void on_enter(StateType  _state);
  // void on_state(StateType  _state);
  // void on_exit(StateType  _state);

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
   * The _trigger will happen async. Triggers::None means stay in current _state.
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
  virtual void setCurrentState(StateType _state);

  /**
   * @brief Do machine work while in a _state (to be called from `on_state` handlers).
   * 
   * @param currentState - passed as a convenience so on_state doesn't have to call getState().
   * @return Trigger - a _trigger for a _state transition. The `on_state` will then use this to
   * initate a _state transition (by calling the appropriate `setTrigger()`)
   */
  virtual TriggerType process(StateType currentState);

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

  //// Added APIS

  /**
 * @brief For synchronous commands like 'status'
 * 
 */
  void runCommand();

  // this flag tells us the system came up alive
  bool systemFunctional = true;

  // Api api;
  // Slider slider;
  // Door door;

  template <class T, typename... Args>
  /**
   * @brief Logs the status of the system
   * 
   * @param msg 
   * @param args 
   */
  void logStatus(T msg, Args... args) {
    _logStatus[0] = 0;
    if (!systemFunctional) {
      Log.errorln(F("System is not functional"));
    }
    Log.noticeln(msg, args...);

#if 0
    sprintf_P(_logStatus, PSTR("Door: %s | Slider: %s, Fwd: %dmm, Rwd: %dmm, Speed: %d, Sp Fract: %d"),
        // sprintf(_logStatus, _f_logStatusFmt,
        door.getStatusString(),
        slider.getStatusString(),
        slider.getForwardDistance(),
        slider.getRearwardDistance(),
        slider.getCurrentSpeed(),
        slider.getSpeedFraction());

    Log.notice(F(" State: %s | "), getStatusString());
    // Log.notice(F("Door: %s | "), door.getStatusString());
    // Log.notice(F("Slider: %s, "), slider.getStatusString());
    // Log.notice(F("Fwd: %dmm, Rwd: %dmm, "), slider.getForwardDistance(), slider.getRearwardDistance());
    // Log.noticeln(F("Speed: %d, Sp Fract: %d"), slider.getCurrentSpeed(), slider.getSpeedFraction());
#endif
    Log.noticeln(_logStatus);
  }

 private:
  bool configStateMachine();

  // ---------- Singleton support
  // https://stackoverflow.com/a/1008289/297526
 public:
  // Getting a single instance object
  static System& getInstance() {
    static System instance;  // Guaranteed to be destroyed.
                             // Instantiated on first use.
    return instance;
  }

 private:
  // Prohibiting External Constructions
  System() {}

  // C++ 11
  // =======
  // We can use the better technique of deleting the methods
  // we don't want.
 public:
  // This breaks printTo
  //System(System const&) = delete;
  void operator=(System const&) = delete;
};

//extern System _system;
