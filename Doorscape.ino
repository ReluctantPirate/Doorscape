bool amDoor = false;

byte doorCombo[6] = {8, 8, 8, 8, 8, 8};
Color cardColors[9] = {RED, YELLOW, GREEN, BLUE, RED, YELLOW, GREEN, BLUE, dim(WHITE, 50)};

enum cardTypes {REDCARD, YELCARD, GRECARD, BLUCARD, REDTRAP, YELTRAP, GRETRAP, BLUTRAP, EMPTY};
byte deckContents[13] = {REDCARD, YELCARD, GRECARD, BLUCARD, REDCARD, YELCARD, GRECARD, BLUCARD, REDTRAP, YELTRAP, GRETRAP, BLUTRAP, EMPTY};
byte deckPosition = 0;

Timer doorTimer;
#define DOOR_CODE_TIME 20000

Timer drawAnimTimer;
#define DRAW_ANIM_TIME 400

bool isSolved = false;

void setup() {
  // put your setup code here, to run once:
  randomize();
  shuffleDeck();
}

void loop() {

  if (amDoor) {
    doorLoop();
  } else {
    cardLoop();
    cardDisplay();
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
      if (doorCombo[f] == 4) {
        doorCombo[f] = 8;
      }
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
      if (doorCombo[f] == 8) {//this face wants NO NEIGHBOR
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
    if (deckPosition < 12) {
      deckPosition++;
      drawAnimTimer.set(DRAW_ANIM_TIME);
    } else {
      //BAD TIMES YO
    }
  }

  if (buttonDoubleClicked()) {
    deckPosition = 0;
    shuffleDeck();
  }

  if (buttonMultiClicked()) {
    if (buttonClickCount() == 3 ) {
      amDoor = true;
    }
  }

  byte sendData = (amDoor << 4) | (deckContents[deckPosition]);
  setValueSentOnAllFaces(sendData);
}

void cardDisplay() {
  //am I idle, or am I drawing?
  if (drawAnimTimer.isExpired()) {//just sitting idly showing cards
    setColor(cardColors[deckContents[deckPosition]]);
    if (deckPosition < 12) {
      setColorOnFace(cardColors[deckContents[deckPosition + 1]], 0);
    }
  } else {//doing draw animation
    byte currentFrame = map(DRAW_ANIM_TIME - drawAnimTimer.getRemaining(), 0, DRAW_ANIM_TIME, 1, 3);
    switch (currentFrame) {
      case 1:
        setColor(cardColors[deckContents[deckPosition - 1]]);
        setColorOnFace(cardColors[deckContents[deckPosition]], 0);
        break;
      case 2:
        setColor(cardColors[deckContents[deckPosition - 1]]);
        setColorOnFace(cardColors[deckContents[deckPosition]], 0);
        setColorOnFace(cardColors[deckContents[deckPosition]], 1);
        setColorOnFace(cardColors[deckContents[deckPosition]], 5);
        break;
      case 3:
        setColor(cardColors[deckContents[deckPosition]]);
        setColorOnFace(cardColors[deckContents[deckPosition - 1]], 3);
        break;
    }
  }
}

void shuffleDeck() {
  for (byte i = 0; i < 30; i++) {
    byte swapA = random(11);
    byte swapB = random(11);
    byte temp = deckContents[swapA];
    deckContents[swapA] = deckContents[swapB];
    deckContents[swapB] = temp;
  }
}

byte getAmDoor(byte data) {
  return (data >> 4);
}

byte getCardColor(byte data) {
  return (data & 15);
}
