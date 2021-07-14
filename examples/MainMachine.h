#pragma once

BEGIN_FLASH_STRING_TABLE_CLASS(MainMachineStates)
ADD_FLASH_STRING("error")
ADD_FLASH_STRING("startup")
ADD_FLASH_STRING("transitioning")
ADD_FLASH_STRING("retracted")
ADD_FLASH_STRING("deployed")
ADD_FLASH_STRING("deploy_opening")
ADD_FLASH_STRING("deploy_closing")
ADD_FLASH_STRING("retract_opening")
ADD_FLASH_STRING("retract_closing")
ADD_FLASH_STRING("slider_deploying")
ADD_FLASH_STRING("slider_retracting")
END_FLASH_STRING_TABLE()

BEGIN_FLASH_STRING_TABLE_CLASS(MainMachineTriggers)
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

class MainMachine : public Machine {
 public:
  enum States
  {
    error,
    startup,
    transitioning,
    retracted,
    deployed,
    deploy_opening,
    deploy_closing,
    retract_opening,
    retract_closing,
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

  /***************** BEGIN From Machine() ****************/
  virtual bool begin();
  virtual void runSubMachines();
  //virtual void setTrigger(TriggerType trigger);
  //virtual void stateChanged(StateType state);
  virtual TriggerType process(StateType stateCalling = Machine::States::error);
  //virtual size_t printTo(Print& p) const;
  /***************** END From Machine() ****************/

  //// Added APIS

  /**
   * @brief For synchronous commands like 'status'
   * 
   */
  void runCommand();

  // this flag tells us the system came up alive
  bool systemFunctional = true;

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
      Log.errorln(F("MainMachine is not functional"));
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
  /**
   * @brief Returns the single instance object
   *
   */
  static MainMachine& getInstance() {
    static MainMachine instance;  // Guaranteed to be destroyed.
                                  // Instantiated on first use.
    return instance;
  }

 private:
  // Prohibiting External Constructions
  MainMachine() {
    _stateStrings = _progmem_MainMachineStates;
    _triggerStrings = _progmem_MainMachineTriggers;
  }

  // C++ 11
  // =======
  // We can use the better technique of deleting the methods
  // we don't want.
 public:
  // This breaks printTo
  //MainMachine(MainMachine const&) = delete;
  void operator=(MainMachine const&) = delete;
};

//extern MainMachine _system;
