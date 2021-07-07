#pragma once

class Piggy : public Printable {
 public:
  Piggy();
  size_t printTo(Print& p) const;

  StringTable strings;

  Fsm* pfsm;
  State* pstate;
  int currentState;
};
