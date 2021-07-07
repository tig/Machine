#include <Arduino.h>
#include <ArduinoLog.h>
#include <assert.h>
#include <FlashStringTable.h>
#include <Fsm.h>
#include <FlashStringTable.h>

#include "Machine.h"
#include "Piggy.h"

BEGIN_FLASH_STRING_TABLE_CLASS(Piggy)
ADD_FLASH_STRING("Mrs")
ADD_FLASH_STRING("Piggy")
ADD_FLASH_STRING("Loves")
ADD_FLASH_STRING("Kermit")
ADD_FLASH_STRING("The")
ADD_FLASH_STRING("Frog")
ADD_FLASH_STRING("!")
END_FLASH_STRING_TABLE()

Piggy::Piggy() : strings(_progmem_Piggy) {
  // pstate = &_rgpStates[0];

  // currentState = 0;
  // g_pfsm = pfsm = new Fsm(&_rgpStates[currentState]);

  // pfsm->add_transition(&_rgpStates[0], &_rgpStates[1], 1, nullptr);
  // pfsm->add_transition(&_rgpStates[1], &_rgpStates[0], 1, nullptr);

  // Log.traceln("run_machine");
  // pfsm->run_machine();
  // Log.traceln("run_machine");
  // pfsm->run_machine();
  // Log.traceln("run_machine");
  // pfsm->run_machine();
}

size_t Piggy::printTo(Print &p) const { return p.print(strings.getString(3)); };
