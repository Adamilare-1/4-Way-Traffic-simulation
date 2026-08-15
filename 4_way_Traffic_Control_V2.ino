/*
  4-Way Traffic Light Controller (millis()-based, precise timing)
  Phase 1: Lane 1 + Lane 3
  Phase 2: Lane 2 + Lane 4

  Full cycle (4 steps):
    1) Phase1 RED    | Phase2 GREEN   -> lasts RED_TIME
    2) Phase1 ORANGE | Phase2 GREEN   -> lasts ORANGE_TIME
    3) Phase1 GREEN  | Phase2 RED     -> lasts RED_TIME
    4) Phase1 GREEN  | Phase2 ORANGE  -> lasts ORANGE_TIME
    (back to step 1)

  Real-world target timing: Red 5s, Orange 1s, Green 6s
  (values below are scaled 1/4 to correct for this board's clock discrepancy)
*/

// Lane 1
const int L1_RED = 13;
const int L1_YEL = 12;
const int L1_GRN = 11;

// Lane 2
const int L2_RED = 10;
const int L2_YEL = 9;
const int L2_GRN = 8;

// Lane 3
const int L3_RED = 7;
const int L3_YEL = 6;
const int L3_GRN = 5;

// Lane 4
const int L4_RED = 4;
const int L4_YEL = 3;
const int L4_GRN = 2;

enum LightState { RED_S, ORANGE_S, GREEN_S };

// Scaled 1/4 to correct for observed 4x timing discrepancy
const unsigned long RED_TIME    = 1250; // targets ~5s in reality
const unsigned long ORANGE_TIME = 250;  // targets ~1s in reality

int step = 0; // 0,1,2,3 -> cycles through the 4 steps above
unsigned long stateStartTime = 0;

void setup() {
  int pins[] = {L1_RED, L1_YEL, L1_GRN,
                L2_RED, L2_YEL, L2_GRN,
                L3_RED, L3_YEL, L3_GRN,
                L4_RED, L4_YEL, L4_GRN};

  for (int i = 0; i < 12; i++) {
    pinMode(pins[i], OUTPUT);
  }

  applyStep();
  stateStartTime = millis();
}

void setLane(int redPin, int yelPin, int grnPin, LightState state) {
  digitalWrite(redPin, state == RED_S ? HIGH : LOW);
  digitalWrite(yelPin, state == ORANGE_S ? HIGH : LOW);
  digitalWrite(grnPin, state == GREEN_S ? HIGH : LOW);
}

void applyStep() {
  LightState phase1State, phase2State;

  switch (step) {
    case 0: phase1State = RED_S;    phase2State = GREEN_S;  break;
    case 1: phase1State = ORANGE_S; phase2State = GREEN_S;  break;
    case 2: phase1State = GREEN_S;  phase2State = RED_S;    break;
    case 3: phase1State = GREEN_S;  phase2State = ORANGE_S; break;
  }

  setLane(L1_RED, L1_YEL, L1_GRN, phase1State);
  setLane(L3_RED, L3_YEL, L3_GRN, phase1State);
  setLane(L2_RED, L2_YEL, L2_GRN, phase2State);
  setLane(L4_RED, L4_YEL, L4_GRN, phase2State);
}

unsigned long currentStepDuration() {
  switch (step) {
    case 0: return RED_TIME;    // Phase1 red
    case 1: return ORANGE_TIME; // Phase1 orange
    case 2: return RED_TIME;    // Phase2 red (Phase1 still green)
    case 3: return ORANGE_TIME; // Phase2 orange (Phase1 still green)
  }
  return RED_TIME; // fallback, shouldn't be reached
}

void loop() {
  unsigned long elapsed = millis() - stateStartTime;

  if (elapsed >= currentStepDuration()) {
    step = (step + 1) % 4;
    applyStep();
    stateStartTime = millis();
  }
}