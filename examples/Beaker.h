
#pragma once

BEGIN_FLASH_STRING_TABLE_CLASS(BeakerTriggers)
ADD_FLASH_STRING("None")
ADD_FLASH_STRING("Oil")
ADD_FLASH_STRING("Water")
ADD_FLASH_STRING("Juice")
ADD_FLASH_STRING("Beer")
END_FLASH_STRING_TABLE()

class Beaker : public Machine {
 public:
  MachineState error;
  MachineState zero;
  MachineState one;
  MachineState two;
  MachineState three;

  enum Triggers
  {
    None,
    Oil,
    Water,
    Juice,
    Beer
  };

  /***************** BEGIN From Machine() ****************/
  virtual bool begin();
  virtual void runSubMachines();
  //virtual void setTrigger(TriggerType trigger);
  //virtual void stateChanged(MachineState* state);
  virtual TriggerType process(MachineState* stateCalling = nullptr);
  //virtual size_t printTo(Print& p) const;
  /***************** END From Machine() ****************/

 private:
  // =======================================================
  // Singleton support
  // https://stackoverflow.com/a/1008289/297526
 public:
  /**
   * @brief Returns the single instance object
   *
   */
  static Beaker& getInstance() {
    static Beaker instance;  // Guaranteed to be destroyed.
                             // Instantiated on first use.
    return instance;
  }
  Beaker() {
    // Don't do anything in here because these objects are static
    // and thus constructors are called before Serial or other
    // stuff is available, making debugging hard. Use begin() instead.
    _triggerStrings = _progmem_BeakerTriggers;
  }

 private:
  // C++ 11
  // =======
  // We can use the better technique of deleting the methods
  // we don't want.
 public:
  // This breaks printTo
  //Beaker(Beaker const&) = delete;
  void operator=(Beaker const&) = delete;
  // =======================================================
};