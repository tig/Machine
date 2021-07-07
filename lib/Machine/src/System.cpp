#include <Arduino.h>
#include <ArduinoLog.h>
#include <assert.h>
#include <FlashStringTable.h>
#include <Fsm.h>
#include <FlashStringTable.h>

#include "Machine.h"
#include "System.h"

//System _system;

// System::System() {
//   // Don't do anything in here because these objects are static
//   // and thus constructors are called before Serial or other
//   // stuff is available, making debugging hard. Use begin() instead.
// }

bool System::configStateMachine() {
  _rgpStates = (State **)new State[(System::States::slider_retracting + 1)];

  // =============================================
  _rgpStates[System::States::error] = new State(
      []() {  // on_enter
        Log.traceln("on_enter: error");
        System::getInstance().setCurrentState(System::States::error);
#if 0
      System::getInstance().door.reset();
      System::getInstance().slider.stop();
      //System::getInstance().logStatus("In Error State");
#endif
      },
      []() {  // on_state
#if 0
        if (System::getInstance().api.currentCommand == Api::Commands::Reset) {
          System::getInstance().setTrigger(System::Triggers::Reset);
        }
      System::getInstance().api.resetCurrentCommand();
#endif
      },
      []() {  // on_exit
        Log.traceln("on_exit: error");
        System::getInstance().logStatus("Error State Cleared");
      });

  // =============================================
  _rgpStates[System::States::startup] = new State(
      []() {                               // on_enter
        Log.traceln("on_enter: startup");  // TODO: PROGMEM string
        System::getInstance().setCurrentState(System::States::startup);
      },
      []() {                               // on_state
        System system = System::getInstance();
        Log.traceln("on_state: startup - _state = %S, _trigger = %S", system._stateStrings.getString(system._state),
         system._triggerStrings.getString(system._trigger));  // TODO: PROGMEM string
        if (system.systemFunctional) {
          switch (system.process((StateType)System::States::startup)) {
            //system.logStatus("on_state: startup");
            case System::Triggers::Transitioning:
              //system.logStatus("on_state: Transitioning");
              system.setTrigger(System::Triggers::Transitioning);
              break;

            case System::Triggers::Retracted:
              //system.logStatus("on_state: Retracted");
              system.setTrigger(System::Triggers::Retracted);
              break;

            case System::Triggers::Deployed:
              //system.logStatus("on_state: Deployed");
              system.setTrigger(System::Triggers::Deployed);
              break;

            default:
              break;
          }
        }
      },
      []() {                              // on_exit
        Log.traceln("on_exit: startup");  // TODO: PROGMEM string
      });

  // =============================================
  // A startup state
  _rgpStates[System::States::transitioning] = new State(
      []() {  // on_enter
        Log.traceln("on_enter: transitioning");
        System::getInstance().setCurrentState(System::States::transitioning);
      },
      []() {  // on_state
#if 0
      switch (System::getInstance().api.currentCommand) {
        case Api::Commands::Deploy:
          System::getInstance().setTrigger(System::Triggers::Deploy);
          break;

        case Api::Commands::Retract:
          System::getInstance().setTrigger(System::Triggers::Retract);
          break;

        default:
          break;
      }
      System::getInstance().api.resetCurrentCommand();
#endif
      },
      []() {  // on_exit
        Log.traceln("on_exit: transitioning");
      });

  // =============================================
  _rgpStates[System::States::retracted] = new State(
      []() {  // on_enter
        Log.traceln("on_enter: retracted");
        System::getInstance().setCurrentState(System::States::retracted);
      },
      []() {  // on_state
#if 0
      switch (System::getInstance().api.currentCommand) {
        case Api::Commands::Deploy:
          System::getInstance().setTrigger(System::Triggers::Deploy);
          break;

        default:
          break;
      }
      System::getInstance().api.resetCurrentCommand();
#endif
      },
      []() {  // on_exit
        Log.traceln("on_exit: retracted");
      });

  // =============================================
  _rgpStates[System::States::deployed] = new State(
      []() {  // on_enter
        Log.traceln("on_enter: deployed");
        System::getInstance().setCurrentState(System::States::deployed);
      },
      []() {  // on_state
#if 0
      switch (System::getInstance().api.currentCommand) {
        case Api::Commands::Retract:
          System::getInstance().setTrigger(System::Triggers::Retract);
          break;

        default:
          break;
      }
      System::getInstance().api.resetCurrentCommand();
#endif
      },
      []() {  // on_exit
        Log.traceln("on_exit: deployed");
      });

  // =============================================
  _rgpStates[System::States::deploy_door_opening] = new State(
      []() {  // on_enter
        Log.traceln("on_enter: deploy_door_opening");
        System::getInstance().setCurrentState(System::States::deploy_door_opening);
#if 0
      System::getInstance().door.open();
#endif
      },
      []() {  // on_state
#if 0
     //Log.traceln("on_state: deploy_door_opening");
      if (System::getInstance().api.currentCommand == Api::Commands::Abort) {
        System::getInstance().setTrigger(System::Triggers::Abort);
      } else {

        switch (System::getInstance().door.process((StateType)System::States::deploy_door_opening)) {
          case Door::Status::Opened:
            // Door Open
            Log.traceln("   state_deploy_door_opening - System::Triggers::DoorOpened");
            System::getInstance().setTrigger(System::Triggers::DoorOpened);
            break;

          case Door::Status::Timeout:
            Log.traceln("   state_deploy_door_opening - System::Triggers::DoorTimeout");
            System::getInstance().setTrigger(System::Triggers::DoorTimeout);
            break;

          case Door::Status::Error:
            Log.traceln("   state_deploy_door_opening - System::Triggers::DoorError");
            System::getInstance().setTrigger(System::Triggers::DoorError);
            break;

          default:
            break;
        }
      }
      System::getInstance().api.resetCurrentCommand();
#endif
      },
      []() {  // on_exit
        Log.traceln("on_exit: deploy_door_opening");
#if 0
        System::getInstance().door.reset();
#endif
      });

  // =============================================
  _rgpStates[System::States::deploy_door_closing] = new State(
      []() {  // on_enter
        Log.traceln("on_enter: deploy_door_closing");
        System::getInstance().setCurrentState(System::States::deploy_door_closing);
#if 0

      System::getInstance().door.close();
#endif
      },
      []() {  // on_state
#if 0
      if (System::getInstance().api.currentCommand == Api::Commands::Abort) {
        System::getInstance().setTrigger(System::Triggers::Abort);
      } else {
        switch (System::getInstance().door.process()) {
          case Door::Status::Closed:
            // Door Open
            Log.traceln("   state_deploy_door_closing - System::Triggers::DoorClosed");
            System::getInstance().setTrigger(System::Triggers::DoorClosed);
            break;

          case Door::Status::Timeout:
            Log.traceln("   state_deploy_door_closing - System::Triggers::DoorTimeout");
            System::getInstance().setTrigger(System::Triggers::DoorTimeout);
            break;

          case Door::Status::Error:
            Log.traceln("   state_deploy_door_closing - System::Triggers::DoorError");
            System::getInstance().setTrigger(System::Triggers::DoorError);
            break;

          default:
            break;
        }
      }
      System::getInstance().api.resetCurrentCommand();
#endif
      },
      []() {  // on_exit
        Log.traceln("on_exit: deploy_door_closing");
#if 0
      System::getInstance().door.reset();
#endif
      });

  // =============================================
  _rgpStates[System::States::retract_door_opening] = new State(
      []() {  // on_enter
        Log.traceln("on_enter: retract_door_opening");
        System::getInstance().setCurrentState(System::States::retract_door_opening);
#if 0

      System::getInstance().door.open();
#endif
      },
      []() {  // on_state
#if 0

      if (System::getInstance().api.currentCommand == Api::Commands::Abort) {
        System::getInstance().setTrigger(System::Triggers::Abort);
      } else {
        switch (System::getInstance().door.process()) {
          case Door::Status::Opened:
            // Door Open
            Log.traceln("   state_retract_door_opening - System::Triggers::DoorOpened");
            System::getInstance().setTrigger(System::Triggers::DoorOpened);
            break;

          case Door::Status::Timeout:
            Log.traceln("   state_retract_door_opening - System::Triggers::DoorTimeout");
            System::getInstance().setTrigger(System::Triggers::DoorTimeout);
            break;

          case Door::Status::Error:
            Log.traceln("   state_retract_door_opening - System::Triggers::DoorError");
            System::getInstance().setTrigger(System::Triggers::DoorError);
            break;

          default:
            break;
        }
      }
      System::getInstance().api.resetCurrentCommand();
#endif
      },
      []() {  // on_exit
        Log.traceln("on_exit: retract_door_opening");
#if 0
      
      System::getInstance().door.reset();
#endif
      });

  // =============================================
  _rgpStates[System::States::retract_door_closing] = new State(
      []() {  // on_enter
        Log.traceln("on_enter: retract_door_closing");
        System::getInstance().setCurrentState(System::States::retract_door_closing);
#if 0

      System::getInstance().door.close();
#endif
      },
      []() {  // on_state
#if 0

      if (System::getInstance().api.currentCommand == Api::Commands::Abort) {
        System::getInstance().setTrigger(System::Triggers::Abort);
      } else {
        switch (System::getInstance().door.process()) {
          case Door::Status::Closed:
            // Door Open
            Log.traceln("   state_retract_door_closing - System::Triggers::DoorClosed");
            System::getInstance().setTrigger(System::Triggers::DoorClosed);
            break;

          case Door::Status::Timeout:
            Log.traceln("   state_retract_door_closing - System::Triggers::DoorTimeout");
            System::getInstance().setTrigger(System::Triggers::DoorTimeout);
            break;

          case Door::Status::Error:
            Log.traceln("   state_retract_door_closing - System::Triggers::DoorError");
            System::getInstance().setTrigger(System::Triggers::DoorError);
            break;

          default:
            break;
        }
      }
      System::getInstance().api.resetCurrentCommand();
#endif
      },
      []() {  // on_exit
        Log.traceln("on_exit: retract_door_closing");
#if 0
System::getInstance().door.reset();
#endif
      });

  // =============================================
  _rgpStates[System::States::slider_deploying] = new State(
      []() {  // on_enter
        Log.traceln("on_enter: slider_deploying");
        System::getInstance().setCurrentState(System::States::slider_deploying);
#if 0
      System::getInstance().slider.deploy();
#endif
      },
      []() {  // on_state
              // slider.get_Status does all the speed up/slow down stuff
#if 0

      if (System::getInstance().api.isCurrentCommand((char*)"abort")) {  // Abort
        System::getInstance().setTrigger(System::Triggers::Abort);
      } else if (System::getInstance().door.process() != Door::Status::Opened) {  // Error
        System::getInstance().setTrigger(System::Triggers::DoorNotOpen);
      } else {
        switch (System::getInstance().slider.process()) {
          case Slider::SliderStatus::Stuck:
            // Error
            System::getInstance().setTrigger(System::Triggers::SliderStuck);
            break;

          case Slider::SliderStatus::Deployed:
            System::getInstance().setTrigger(System::Triggers::Deployed);
            break;

          case Slider::SliderStatus::Error:
            System::getInstance().setTrigger(System::Triggers::SliderError);
            break;

          default:
            break;
        }
      }
      System::getInstance().api.resetCurrentCommand();
#endif
      },
      []() {  // on_exit
        Log.traceln("on_exit: slider_deploying");
#if 0
      System::getInstance().slider.stop();
#endif
      });

  // =============================================
  _rgpStates[System::States::slider_retracting] = new State(
      []() {  // on_enter
        Log.traceln("on_enter: slider_retracting");
        System::getInstance().setCurrentState(System::States::slider_retracting);
#if 0

      System::getInstance().slider.retract();
#endif
      },
      []() {
  // slider.get_Status does all the speed up/slow down stuff
#if 0

      if (System::getInstance().api.isCurrentCommand((char*)"abort")) {  // Abort
        System::getInstance().setTrigger(System::Triggers::Abort);
      } else if (System::getInstance().door.process() != Door::Status::Opened) {  // Error
        System::getInstance().setTrigger(System::Triggers::DoorNotOpen);
      } else {
        switch (System::getInstance().slider.process()) {
          case Slider::SliderStatus::Stuck:
            // Error
            System::getInstance().setTrigger(System::Triggers::SliderStuck);
            break;

          case Slider::SliderStatus::Retracted:
            System::getInstance().setTrigger(System::Triggers::Retracted);
            break;

          case Slider::SliderStatus::Error:
            System::getInstance().setTrigger(System::Triggers::SliderError);
            break;

          default:
            break;
        }
      }
      System::getInstance().api.resetCurrentCommand();
#endif
      },
      []() {  // on_exit
        Log.traceln("on_exit: slider_retracting");
#if 0
      System::getInstance().slider.stop();
#endif
      });

  for (StateType i = 0; i < (System::States::slider_retracting + 1); i++) {
    assert(_rgpStates[i]->on_enter != nullptr);
    assert(_rgpStates[i]->on_state != nullptr);
    assert(_rgpStates[i]->on_exit != nullptr);
  }

   for (StateType i = 0; i < (System::States::slider_retracting + 1); i++) {
     Log.traceln("[%d] on_enter = %X, on_state = %X, on_exit = %X", i,
     _rgpStates[i]->on_enter, _rgpStates[i]->on_state, _rgpStates[i]->on_exit);
   }
  
  _pFsm = new Fsm(_rgpStates[_state]);

  // Static States
  _pFsm->add_transition(_rgpStates[System::States::startup], _rgpStates[System::States::transitioning], System::Triggers::Transitioning, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::startup], _rgpStates[System::States::retracted], System::Triggers::Retracted, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::startup], _rgpStates[System::States::deployed], System::Triggers::Deployed, nullptr);

  _pFsm->add_transition(_rgpStates[System::States::transitioning], _rgpStates[System::States::deploy_door_opening], System::Triggers::Deploy, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::transitioning], _rgpStates[System::States::retract_door_opening], System::Triggers::Retract, nullptr);

  _pFsm->add_transition(_rgpStates[System::States::retracted], _rgpStates[System::States::deploy_door_opening], System::Triggers::Deploy, nullptr);

  _pFsm->add_transition(_rgpStates[System::States::deployed], _rgpStates[System::States::retract_door_opening], System::Triggers::Retract, nullptr);

  // Moving States
  _pFsm->add_transition(_rgpStates[System::States::deploy_door_opening], _rgpStates[System::States::error], System::Triggers::DoorTimeout, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::deploy_door_opening], _rgpStates[System::States::error], System::Triggers::DoorError, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::deploy_door_opening], _rgpStates[System::States::retract_door_closing], System::Triggers::Abort, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::deploy_door_opening], _rgpStates[System::States::slider_deploying], System::Triggers::DoorOpened, nullptr);

  _pFsm->add_transition(_rgpStates[System::States::slider_deploying], _rgpStates[System::States::error], System::Triggers::DoorNotOpen, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::slider_deploying], _rgpStates[System::States::error], System::Triggers::SliderStuck, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::slider_deploying], _rgpStates[System::States::error], System::Triggers::SliderError, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::slider_deploying], _rgpStates[System::States::slider_retracting], System::Triggers::Abort, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::slider_deploying], _rgpStates[System::States::deploy_door_closing], System::Triggers::Deployed, nullptr);

  _pFsm->add_transition(_rgpStates[System::States::deploy_door_closing], _rgpStates[System::States::error], System::Triggers::DoorTimeout, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::deploy_door_closing], _rgpStates[System::States::error], System::Triggers::DoorError, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::deploy_door_closing], _rgpStates[System::States::retract_door_opening], System::Triggers::Abort, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::deploy_door_closing], _rgpStates[System::States::deployed], System::Triggers::DoorClosed, nullptr);

  _pFsm->add_transition(_rgpStates[System::States::retract_door_opening], _rgpStates[System::States::error], System::Triggers::DoorTimeout, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::retract_door_opening], _rgpStates[System::States::error], System::Triggers::DoorError, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::retract_door_opening], _rgpStates[System::States::deploy_door_closing], System::Triggers::Abort, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::retract_door_opening], _rgpStates[System::States::slider_retracting], System::Triggers::DoorOpened, nullptr);

  _pFsm->add_transition(_rgpStates[System::States::slider_retracting], _rgpStates[System::States::error], System::Triggers::DoorNotOpen, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::slider_retracting], _rgpStates[System::States::error], System::Triggers::SliderStuck, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::slider_retracting], _rgpStates[System::States::error], System::Triggers::SliderError, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::slider_retracting], _rgpStates[System::States::slider_deploying], System::Triggers::Abort, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::slider_retracting], _rgpStates[System::States::retracted], System::Triggers::DoorClosed, nullptr);

  _pFsm->add_transition(_rgpStates[System::States::retract_door_closing], _rgpStates[System::States::error], System::Triggers::DoorTimeout, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::retract_door_closing], _rgpStates[System::States::error], System::Triggers::DoorError, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::retract_door_closing], _rgpStates[System::States::deploy_door_opening], System::Triggers::Abort, nullptr);
  _pFsm->add_transition(_rgpStates[System::States::retract_door_closing], _rgpStates[System::States::deployed], System::Triggers::DoorClosed, nullptr);

  _pFsm->add_transition(_rgpStates[System::States::error], _rgpStates[System::States::retract_door_opening], System::Triggers::Reset, nullptr);



  return true;
}

bool System::begin() {
  // No need to call Machine::begin()
  Log.traceln("System::begin()");
  _stateStrings.init(_progmem_SystemStates);
  _triggerStrings.init(_progmem_SystemTriggers);

  // not really needed as Machine's constructor inits; but if a distinct intial _state
  // or trigger were needed.
  _state = (StateType)System::States::startup;
  _trigger = (TriggerType)System::Triggers::None;
  assert(States::slider_retracting + 1 == _stateStrings.getNumStrings());
  assert(Triggers::SliderError + 1 == _triggerStrings.getNumStrings());
  bool result = true;

  result = configStateMachine();

  // TODO: Fix this
  _trigger = System::Triggers::Retracted;

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
        Log.errorln(F("ERROR: The slider is in the stuck _state at system startup."));
        _status = Triggers::Error;
        break;

      case Slider::SliderStatus::Error:
        Log.errorln(F("ERROR: The slider is in an error _state at system startup."));
        _status = Triggers::Error;
        break;

      default:
        Log.errorln(F("ERROR: The slider is in an unknown _state at system startup."));
        _status = Triggers::Error;
    }

    if (door.getStatus() == Door::Status::Error) {
      Log.errorln(F("ERROR: The door is in an error _state at system startup."));
      _status = Triggers::Error;
    }
#endif
    Log.noticeln(F("Setup complete : awaiting your command"));

    // Start the statemachine
    //begin_state_machine();

  } else {
    Log.errorln(F("System error : failed to configure hardware"));
  }

  // Enable the watchdog timer for 2 seconds to ensure Ethernet operations don't
  // lock the system
  // TODO: FIx
  // Watchdog.enable(2000);

  return result;
}

void System::runStateMachines() {
  Log.traceln(F("System::runStateMachines() - _state: %S, _trigger: %S"), _stateStrings.getString(_state), _triggerStrings.getString(_trigger));
  // trigger() ends up callilng old->on_exit, new->on_enter

  Log.traceln("Fsm: m_num_transitions = %d", _pFsm->m_num_transitions);
  Log.traceln("Fsm: m_current_state = %X", _pFsm->m_current_state);
  _pFsm->trigger(_trigger);

  _pFsm->trigger(Triggers::Deployed);

  // runs the current _state's on_state then checks for
  // timed transitions
  Log.traceln(F("calling run_machine()"));
  _pFsm->run_machine();

  // Reset the trigger
  //setTrigger(None);

  // the current _state's on_state has returned
  // returning from here puts us back in loop()...
}

void System::setTrigger(TriggerType trigger) {
  Log.traceln(F("System::setTrigger(%S)"), _triggerStrings.getString(trigger));
  // Trigger a _state transition (asynchronously)
  // TODO: Redraw dispay?

  _trigger = trigger;
}

void System::setCurrentState(StateType state) {
  _state = state;
  logStatus(F("Enter _state: %S"), _stateStrings.getString(_state));
#if 0
  _screen.setRedraw();
#endif
}

TriggerType System::process(StateType currentState) {
  Log.traceln(F("System::process(%S)"), _stateStrings.getString(currentState));
  // Handle work (from on_state). Return new _state transition trigger (or None)
  // DO NOT set _trigger or call setTrigger(); the calling on_state will do so
  TriggerType trigger = (TriggerType)Triggers::None;

#if 0
  //    Log.traceln(F("System::process()"));
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
      Log.errorln(F("ERROR: The slider is in the stuck _state."));
      trigger = Triggers::Error;
      break;

    case Slider::SliderStatus::Error:
      Log.errorln(F("ERROR: The slider is in an error _state."));
      trigger = Triggers::Error;
      break;

    default:
      Log.errorln(F("ERROR: The slider is in an unknown _state."));
      trigger = Triggers::Error;
  }

  if (door.getStatus() == Door::Status::Error) {
    Log.errorln(F("ERROR: The door is in an error _state."));
    trigger = Triggers::Error;
  }
#endif
  //logStatus("System::process()");

  return Triggers::Retracted;
  //return trigger;
}

size_t System::printTo(Print &p) const {
  return p.print(_stateStrings.getString((int)_state));
};
