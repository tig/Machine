#include <Arduino.h>
#include <ArduinoLog.h>
#include <assert.h>

#include "Machine.h"
#include "MainMachine.h"

bool MainMachine::configStateMachine() {
  
  allocateFsm(States::slider_retracting, Triggers::SliderError);

  // =============================================
  _rgpStates[MainMachine::States::error] = new State(
      []() {  // on_enter
        TRACE_STATE_FN(MainMachine, on_enter, true);
        MainMachine_inst.stateChanged(MainMachine::States::error);
#if 0
      MainMachine::getInstance().door.reset();
      MainMachine::getInstance().slider.stop();
      //MainMachine::getInstance().logStatus("In Error State");
#endif
      },
      []() {  // on_state
        TRACE_STATE_FN(MainMachine, on_exit, false);      

#if 0
        if (MainMachine::getInstance().api.currentCommand == Api::Commands::Reset) {
          MainMachine::getInstance().setTrigger(MainMachine::Triggers::Reset);
        }
      MainMachine::getInstance().api.resetCurrentCommand();
#endif
      },
      []() {  // on_exit
        TRACE_STATE_FN(MainMachine, on_exit, true);      
        MainMachine::getInstance().logStatus("Error State Cleared");
      });

  // =============================================
  _rgpStates[MainMachine::States::startup] = new State(
      []() {                               // on_enter
        TRACE_STATE_FN(MainMachine, on_enter, true);
        MainMachine::getInstance().stateChanged(MainMachine::States::startup);
      },
      []() {                               // on_state
        TRACE_STATE_FN(MainMachine, on_exit, true);      
        MainMachine system = MainMachine::getInstance();
        Log.traceln("on_state: startup - currentState = %S, _trigger = %S", system._stateStrings.getString(system.getCurrentState()),
         system._triggerStrings.getString(system._trigger));  // TODO: PROGMEM string
        if (system.systemFunctional) {
          switch (system.process((StateType)MainMachine::States::startup)) {
            //system.logStatus("on_state: startup");
            case MainMachine::Triggers::Transitioning:
              //system.logStatus("on_state: Transitioning");
              system.setTrigger(MainMachine::Triggers::Transitioning);
              break;

            case MainMachine::Triggers::Retracted:
              //system.logStatus("on_state: Retracted");
              system.setTrigger(MainMachine::Triggers::Retracted);
              break;

            case MainMachine::Triggers::Deployed:
              //system.logStatus("on_state: Deployed");
              system.setTrigger(MainMachine::Triggers::Deployed);
              break;

            default:
              break;
          }
        }
      },
      []() {                              // on_exit
        TRACE_STATE_FN(MainMachine, on_exit, true);      
      });

  // =============================================
  // A startup state
  _rgpStates[MainMachine::States::transitioning] = new State(
      []() {  // on_enter
        Log.traceln(F("on_enter: transitioning"));
        MainMachine::getInstance().stateChanged(MainMachine::States::transitioning);
      },
      []() {  // on_state
#if 0
      switch (MainMachine::getInstance().api.currentCommand) {
        case Api::Commands::Deploy:
          MainMachine::getInstance().setTrigger(MainMachine::Triggers::Deploy);
          break;

        case Api::Commands::Retract:
          MainMachine::getInstance().setTrigger(MainMachine::Triggers::Retract);
          break;

        default:
          break;
      }
      MainMachine::getInstance().api.resetCurrentCommand();
#endif
      },
      []() {  // on_exit
        Log.traceln(F("on_exit: transitioning"));
      });

  // =============================================
  _rgpStates[MainMachine::States::retracted] = new State(
      []() {  // on_enter
        Log.traceln(F("on_enter: retracted"));
        MainMachine::getInstance().stateChanged(MainMachine::States::retracted);
      },
      []() {  // on_state
#if 0
      switch (MainMachine::getInstance().api.currentCommand) {
        case Api::Commands::Deploy:
          MainMachine::getInstance().setTrigger(MainMachine::Triggers::Deploy);
          break;

        default:
          break;
      }
      MainMachine::getInstance().api.resetCurrentCommand();
#endif
      },
      []() {  // on_exit
        Log.traceln(F("on_exit: retracted"));
      });

  // =============================================
  _rgpStates[MainMachine::States::deployed] = new State(
      []() {  // on_enter
        Log.traceln(F("on_enter: deployed"));
        MainMachine::getInstance().stateChanged(MainMachine::States::deployed);
      },
      []() {  // on_state
#if 0
      switch (MainMachine::getInstance().api.currentCommand) {
        case Api::Commands::Retract:
          MainMachine::getInstance().setTrigger(MainMachine::Triggers::Retract);
          break;

        default:
          break;
      }
      MainMachine::getInstance().api.resetCurrentCommand();
#endif
      },
      []() {  // on_exit
        Log.traceln(F("on_exit: deployed"));
      });

  // =============================================
  _rgpStates[MainMachine::States::deploy_opening] = new State(
      []() {  // on_enter
        Log.traceln(F("on_enter: deploy_opening"));
        MainMachine::getInstance().stateChanged(MainMachine::States::deploy_opening);
#if 0
      MainMachine::getInstance().door.open();
#endif
      },
      []() {  // on_state
#if 0
     //Log.traceln(F("on_state: deploy_opening"));
      if (MainMachine::getInstance().api.currentCommand == Api::Commands::Abort) {
        MainMachine::getInstance().setTrigger(MainMachine::Triggers::Abort);
      } else {

        switch (MainMachine::getInstance().door.process((StateType)MainMachine::States::deploy_opening)) {
          case Door::States::opened:
            // Door Open
            Log.traceln(F("   state_deploy_opening - MainMachine::Triggers::DoorOpened"));
            MainMachine::getInstance().setTrigger(MainMachine::Triggers::DoorOpened);
            break;

          case Door::States::timeout:
            Log.traceln(F("   state_deploy_opening - MainMachine::Triggers::DoorTimeout"));
            MainMachine::getInstance().setTrigger(MainMachine::Triggers::DoorTimeout);
            break;

          case Door::States::error:
            Log.traceln(F("   state_deploy_opening - MainMachine::Triggers::DoorError"));
            MainMachine::getInstance().setTrigger(MainMachine::Triggers::DoorError);
            break;

          default:
            break;
        }
      }
      MainMachine::getInstance().api.resetCurrentCommand();
#endif
      },
      []() {  // on_exit
        Log.traceln(F("on_exit: deploy_opening"));
#if 0
        MainMachine::getInstance().door.reset();
#endif
      });

  // =============================================
  _rgpStates[MainMachine::States::deploy_closing] = new State(
      []() {  // on_enter
        Log.traceln(F("on_enter: deploy_closing"));
        MainMachine::getInstance().stateChanged(MainMachine::States::deploy_closing);
#if 0

      MainMachine::getInstance().door.close();
#endif
      },
      []() {  // on_state
#if 0
      if (MainMachine::getInstance().api.currentCommand == Api::Commands::Abort) {
        MainMachine::getInstance().setTrigger(MainMachine::Triggers::Abort);
      } else {
        switch (MainMachine::getInstance().door.process()) {
          case Door::States::Closed:
            // Door Open
            Log.traceln(F("   state_deploy_closing - MainMachine::Triggers::DoorClosed"));
            MainMachine::getInstance().setTrigger(MainMachine::Triggers::DoorClosed);
            break;

          case Door::States::timeout:
            Log.traceln(F("   state_deploy_closing - MainMachine::Triggers::DoorTimeout"));
            MainMachine::getInstance().setTrigger(MainMachine::Triggers::DoorTimeout);
            break;

          case Door::States::error:
            Log.traceln(F("   state_deploy_closing - MainMachine::Triggers::DoorError"));
            MainMachine::getInstance().setTrigger(MainMachine::Triggers::DoorError);
            break;

          default:
            break;
        }
      }
      MainMachine::getInstance().api.resetCurrentCommand();
#endif
      },
      []() {  // on_exit
        Log.traceln(F("on_exit: deploy_closing"));
#if 0
      MainMachine::getInstance().door.reset();
#endif
      });

  // =============================================
  _rgpStates[MainMachine::States::retract_opening] = new State(
      []() {  // on_enter
        Log.traceln(F("on_enter: retract_opening"));
        MainMachine::getInstance().stateChanged(MainMachine::States::retract_opening);
#if 0

      MainMachine::getInstance().door.open();
#endif
      },
      []() {  // on_state
#if 0

      if (MainMachine::getInstance().api.currentCommand == Api::Commands::Abort) {
        MainMachine::getInstance().setTrigger(MainMachine::Triggers::Abort);
      } else {
        switch (MainMachine::getInstance().door.process()) {
          case Door::States::opened:
            // Door Open
            Log.traceln(F("   state_retract_opening - MainMachine::Triggers::DoorOpened"));
            MainMachine::getInstance().setTrigger(MainMachine::Triggers::DoorOpened);
            break;

          case Door::States::timeout:
            Log.traceln(F("   state_retract_opening - MainMachine::Triggers::DoorTimeout"));
            MainMachine::getInstance().setTrigger(MainMachine::Triggers::DoorTimeout);
            break;

          case Door::States::error:
            Log.traceln(F("   state_retract_opening - MainMachine::Triggers::DoorError"));
            MainMachine::getInstance().setTrigger(MainMachine::Triggers::DoorError);
            break;

          default:
            break;
        }
      }
      MainMachine::getInstance().api.resetCurrentCommand();
#endif
      },
      []() {  // on_exit
        Log.traceln(F("on_exit: retract_opening"));
#if 0
      
      MainMachine::getInstance().door.reset();
#endif
      });

  // =============================================
  _rgpStates[MainMachine::States::retract_closing] = new State(
      []() {  // on_enter
        Log.traceln(F("on_enter: retract_closing"));
        MainMachine::getInstance().stateChanged(MainMachine::States::retract_closing);
#if 0

      MainMachine::getInstance().door.close();
#endif
      },
      []() {  // on_state
#if 0

      if (MainMachine::getInstance().api.currentCommand == Api::Commands::Abort) {
        MainMachine::getInstance().setTrigger(MainMachine::Triggers::Abort);
      } else {
        switch (MainMachine::getInstance().door.process()) {
          case Door::States::Closed:
            // Door Open
            Log.traceln(F("   state_retract_closing - MainMachine::Triggers::DoorClosed"));
            MainMachine::getInstance().setTrigger(MainMachine::Triggers::DoorClosed);
            break;

          case Door::States::timeout:
            Log.traceln(F("   state_retract_closing - MainMachine::Triggers::DoorTimeout"));
            MainMachine::getInstance().setTrigger(MainMachine::Triggers::DoorTimeout);
            break;

          case Door::States::error:
            Log.traceln(F("   state_retract_closing - MainMachine::Triggers::DoorError"));
            MainMachine::getInstance().setTrigger(MainMachine::Triggers::DoorError);
            break;

          default:
            break;
        }
      }
      MainMachine::getInstance().api.resetCurrentCommand();
#endif
      },
      []() {  // on_exit
        Log.traceln(F("on_exit: retract_closing"));
#if 0
MainMachine::getInstance().door.reset();
#endif
      });

  // =============================================
  _rgpStates[MainMachine::States::slider_deploying] = new State(
      []() {  // on_enter
        Log.traceln(F("on_enter: slider_deploying"));
        MainMachine::getInstance().stateChanged(MainMachine::States::slider_deploying);
#if 0
      MainMachine::getInstance().slider.deploy();
#endif
      },
      []() {  // on_state
              // slider.get_Status does all the speed up/slow down stuff
#if 0

      if (MainMachine::getInstance().api.isCurrentCommand((char*)"abort")) {  // Abort
        MainMachine::getInstance().setTrigger(MainMachine::Triggers::Abort);
      } else if (MainMachine::getInstance().door.process() != Door::States::opened) {  // Error
        MainMachine::getInstance().setTrigger(MainMachine::Triggers::DoorNotOpen);
      } else {
        switch (MainMachine::getInstance().slider.process()) {
          case Slider::SliderStatus::Stuck:
            // Error
            MainMachine::getInstance().setTrigger(MainMachine::Triggers::SliderStuck);
            break;

          case Slider::SliderStatus::Deployed:
            MainMachine::getInstance().setTrigger(MainMachine::Triggers::Deployed);
            break;

          case Slider::SliderStatus::Error:
            MainMachine::getInstance().setTrigger(MainMachine::Triggers::SliderError);
            break;

          default:
            break;
        }
      }
      MainMachine::getInstance().api.resetCurrentCommand();
#endif
      },
      []() {  // on_exit
        Log.traceln(F("on_exit: slider_deploying"));
#if 0
      MainMachine::getInstance().slider.stop();
#endif
      });

  // =============================================
  _rgpStates[MainMachine::States::slider_retracting] = new State(
      []() {  // on_enter
        Log.traceln(F("on_enter: slider_retracting"));
        MainMachine::getInstance().stateChanged(MainMachine::States::slider_retracting);
#if 0

      MainMachine::getInstance().slider.retract();
#endif
      },
      []() {
  // slider.get_Status does all the speed up/slow down stuff
#if 0

      if (MainMachine::getInstance().api.isCurrentCommand((char*)"abort")) {  // Abort
        MainMachine::getInstance().setTrigger(MainMachine::Triggers::Abort);
      } else if (MainMachine::getInstance().door.process() != Door::States::opened) {  // Error
        MainMachine::getInstance().setTrigger(MainMachine::Triggers::DoorNotOpen);
      } else {
        switch (MainMachine::getInstance().slider.process()) {
          case Slider::SliderStatus::Stuck:
            // Error
            MainMachine::getInstance().setTrigger(MainMachine::Triggers::SliderStuck);
            break;

          case Slider::SliderStatus::Retracted:
            MainMachine::getInstance().setTrigger(MainMachine::Triggers::Retracted);
            break;

          case Slider::SliderStatus::Error:
            MainMachine::getInstance().setTrigger(MainMachine::Triggers::SliderError);
            break;

          default:
            break;
        }
      }
      MainMachine::getInstance().api.resetCurrentCommand();
#endif
      },
      []() {  // on_exit
        Log.traceln(F("on_exit: slider_retracting"));
#if 0
      MainMachine::getInstance().slider.stop();
#endif
      });

  for (StateType i = 0; i < (MainMachine::States::slider_retracting + 1); i++) {
    assert(_rgpStates[i]->on_enter != nullptr);
    assert(_rgpStates[i]->on_state != nullptr);
    assert(_rgpStates[i]->on_exit != nullptr);
  }

   for (StateType i = 0; i < (MainMachine::States::slider_retracting + 1); i++) {
     Log.traceln("[%d] @%X - on_enter = %X, on_state = %X, on_exit = %X", i, _rgpStates[i],
     _rgpStates[i]->on_enter, _rgpStates[i]->on_state, _rgpStates[i]->on_exit);
   }
  
  setStartState(startup);

  // Static States
  addTransition(startup, transitioning, MainMachine::Triggers::Transitioning);
  addTransition(startup, retracted, MainMachine::Triggers::Retracted);
  addTransition(startup, deployed, MainMachine::Triggers::Deployed);

  addTransition(transitioning, deploy_opening, MainMachine::Triggers::Deploy);
  addTransition(transitioning, retract_opening, MainMachine::Triggers::Retract);

  addTransition(retracted, deploy_opening, MainMachine::Triggers::Deploy);

  addTransition(deployed, retract_opening, MainMachine::Triggers::Retract);

  // Moving States
  addTransition(deploy_opening, error, MainMachine::Triggers::DoorTimeout);
  addTransition(deploy_opening, error, MainMachine::Triggers::DoorError);
  addTransition(deploy_opening, retract_closing, MainMachine::Triggers::Abort);
  addTransition(deploy_opening, slider_deploying, MainMachine::Triggers::DoorOpened);

  addTransition(slider_deploying, error, MainMachine::Triggers::DoorNotOpen);
  addTransition(slider_deploying, error, MainMachine::Triggers::SliderStuck);
  addTransition(slider_deploying, error, MainMachine::Triggers::SliderError);
  addTransition(slider_deploying, slider_retracting, MainMachine::Triggers::Abort);
  addTransition(slider_deploying, deploy_closing, MainMachine::Triggers::Deployed);

  addTransition(deploy_closing, error, MainMachine::Triggers::DoorTimeout);
  addTransition(deploy_closing, error, MainMachine::Triggers::DoorError);
  addTransition(deploy_closing, retract_opening, MainMachine::Triggers::Abort);
  addTransition(deploy_closing, deployed, MainMachine::Triggers::DoorClosed);

  addTransition(retract_opening, error, MainMachine::Triggers::DoorTimeout);
  addTransition(retract_opening, error, MainMachine::Triggers::DoorError);
  addTransition(retract_opening, deploy_closing, MainMachine::Triggers::Abort);
  addTransition(retract_opening, slider_retracting, MainMachine::Triggers::DoorOpened);

  addTransition(slider_retracting, error, MainMachine::Triggers::DoorNotOpen);
  addTransition(slider_retracting, error, MainMachine::Triggers::SliderStuck);
  addTransition(slider_retracting, error, MainMachine::Triggers::SliderError);
  addTransition(slider_retracting, slider_deploying, MainMachine::Triggers::Abort);
  addTransition(slider_retracting, retracted, MainMachine::Triggers::DoorClosed);

  addTransition(retract_closing, error, MainMachine::Triggers::DoorTimeout);
  addTransition(retract_closing, error, MainMachine::Triggers::DoorError);
  addTransition(retract_closing, deploy_opening, MainMachine::Triggers::Abort);
  addTransition(retract_closing, deployed, MainMachine::Triggers::DoorClosed);

  addTransition(error, retract_opening, MainMachine::Triggers::Reset);
  return true;
}

bool MainMachine::begin() {
  // No need to call Machine::begin()
  Log.traceln(F("MainMachine::begin()"));
  _stateStrings.init(_progmem_MainMachineStates);
  _triggerStrings.init(_progmem_MainMachineTriggers);

  // not really needed as Machine's constructor inits; but if a distinct intial state
  // or trigger were needed.
  assert(States::slider_retracting + 1 == _stateStrings.getNumStrings());
  assert(Triggers::SliderError + 1 == _triggerStrings.getNumStrings());
  bool result = true;

  result = configStateMachine();

  // TODO: Fix this
  _trigger = MainMachine::Triggers::Retracted;

#if 0
  // Setup REST interface
  api.begin();

  // Setup the door
  if (door.begin() == false) {
    systemFunctional = false;
  }

  // Setup the slider
  assert(0 == slider.getCurrentSpeed());
  if (slider.begin() == false) {
    systemFunctional = false;
  }
#endif

  if (systemFunctional) {
#if 0    
    switch (slider.getStatus()) {
      case Slider::SliderStatus::Deployed:
        _status = Triggers::Deployed;
        break;

      case Slider::SliderStatus::Retracted:
        _status = Triggers::Retracted;
        break;

      case Slider::SliderStatus::Deploying:
      case Slider::SliderStatus::Retracting:
        _status = Triggers::Transitioning;
        break;

      case Slider::SliderStatus::Stuck:
        Log.errorln(F("ERROR: The slider is in the stuck state at system startup."));
        _status = Triggers::Error;
        break;

      case Slider::SliderStatus::Error:
        Log.errorln(F("ERROR: The slider is in an error state at system startup."));
        _status = Triggers::Error;
        break;

      default:
        Log.errorln(F("ERROR: The slider is in an unknown state at system startup."));
        _status = Triggers::Error;
    }

    if (door.getStatus() == Door::States::error) {
      Log.errorln(F("ERROR: The door is in an error state at system startup."));
      _status = Triggers::Error;
    }
#endif
    Log.noticeln(F("Setup complete : awaiting your command"));

    // Start the statemachine
    //begin_state_machine();

  } else {
    Log.errorln(F("ERROR: MainMachine failed to configure hardware"));
  }

  // Enable the watchdog timer for 2 seconds to ensure Ethernet operations don't
  // lock the system
  // TODO: FIx
  // Watchdog.enable(2000);

  return result;
}

void MainMachine::runSubMachines() {
  Log.traceln(F("MainMachine::runSubMachines() - currentState: %S, _trigger: %S"), _stateStrings.getString(getCurrentState()), _triggerStrings.getString(_trigger));
  // trigger() ends up callilng old->on_exit, new->on_enter

  trigger(_trigger);

  trigger(Triggers::Deployed);

  // runs the current state's on_state then checks for
  // timed transitions
  Log.traceln(F("calling runMachine()"));
  runMachine();

  // Reset the trigger
  //setTrigger(None);

  // the current state's on_state has returned
  // returning from here puts us back in loop()...
}

TriggerType MainMachine::process(StateType stateCalling) {
  Log.traceln(F("MainMachine::process(%S)"), _stateStrings.getString(stateCalling));
  // Handle work (from on_state). Return new state transition trigger (or None)
  // DO NOT set _trigger or call setTrigger(); the calling on_state will do so
  //TriggerType trigger = (TriggerType)Triggers::None;

#if 0
  //    Log.traceln(F("MainMachine::process()"));
  switch (slider.getStatus()) {
    case Slider::SliderStatus::Deployed:
      trigger = Triggers::Deployed;
      break;

    case Slider::SliderStatus::Retracted:
      trigger = Triggers::Retracted;
      break;

    case Slider::SliderStatus::Deploying:
    case Slider::SliderStatus::Retracting:
      trigger = Triggers::Transitioning;
      break;

    case Slider::SliderStatus::Stuck:
      Log.errorln(F("ERROR: The slider is in the stuck state."));
      trigger = Triggers::Error;
      break;

    case Slider::SliderStatus::Error:
      Log.errorln(F("ERROR: The slider is in an error state."));
      trigger = Triggers::Error;
      break;

    default:
      Log.errorln(F("ERROR: The slider is in an unknown state."));
      trigger = Triggers::Error;
  }

  if (door.getStatus() == Door::States::error) {
    Log.errorln(F("ERROR: The door is in an error state."));
    trigger = Triggers::Error;
  }
#endif
  //logStatus("MainMachine::process()");

  return Triggers::Retracted;
  //return trigger;
}
