bool amDoor = false;
byte currentColor = 0;

byte doorCombo[6] = {4, 4, 4, 4, 4, 4};
Color cardColors[5] = {RED, YELLOW, GREEN, BLUE, OFF};

Timer doorTimer;
#define DOOR_CODE_TIME 20000

bool isSolved = false;

void setup() {
  // put your setup code here, to run once:
  randomize();
  currentColor = random(3);
}

void loop() {

  if (amDoor) {
    doorLoop();
  } else {
    cardLoop();
  }

}

void doorLoop() {
  if (buttonMultiClicked()) {
    if (buttonClickCount() == 3 ) {
      amDoor = false;
    }
  }

  if (doorTimer.isExpired()) {
    //reset doorTimer
    doorTimer.set(DOOR_CODE_TIME);
    //randomize door code
    FOREACH_FACE(f) {
      doorCombo[f] = random(4);
    }
    //reset isSolved
    isSolved = false;
  }

  //look at neighbors, determine if the combo has been made
  byte correctNeighbors = 0;
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {//neighbor!
      if (doorCombo[f] < 4) {//this face WANTS a neighbor
        byte neighborColor = getCardColor(getLastValueReceivedOnFace(f));
        if (neighborColor == doorCombo[f]) {
          correctNeighbors++;
        }
      }
    } else {//no neighbor!
      if (doorCombo[f] == 4) {//this face wants NO NEIGHBOR
        correctNeighbors++;
      }
    }
  }//end of neighborhood check loop

  if (correctNeighbors == 6) {
    isSolved = true;
  }

  if (isSolved) {
    setColor(WHITE);
    setColorOnFace(RED, random(5));
    setColorOnFace(YELLOW, random(5));
    setColorOnFace(GREEN, random(5));
    setColorOnFace(BLUE, random(5));
  } else {
    FOREACH_FACE(f) {
      setColorOnFace(cardColors[doorCombo[f]], f);
    }
  }

}

void cardLoop() {
  if (buttonSingleClicked()) {
    currentColor = (currentColor + 1) % 4;
  }

  if (buttonDoubleClicked()) {
    currentColor = random(3);
  }

  if (buttonMultiClicked()) {
    if (buttonClickCount() == 3 ) {
      amDoor = true;
    }
  }

  byte sendData = (amDoor << 2) | (currentColor);
  setValueSentOnAllFaces(sendData);

  setColor(cardColors[currentColor]);
}

byte getAmDoor(byte data) {
  return (data >> 2);
}

byte getCardColor(byte data) {
  return (data & 3);
}
