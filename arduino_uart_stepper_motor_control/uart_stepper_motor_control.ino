int PUL = 7; // define Pulse pin
int DIR = 6; // define Direction pin
int ENA = 5; // define Enable Pin

const int STEPS_PER_REVOLUTION = 400; // 기본 스텝 수

String inputString = "";         // 문자열을 저장하기 위한 변수
bool stringComplete = false;     // 문자열 완료 플래그

void setup() {
  pinMode(PUL, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(ENA, OUTPUT);
  Serial.begin(115200); // 시리얼 통신 시작
}

void moveByValue(float value) {
  int steps = value * STEPS_PER_REVOLUTION;  // -1.0은 -STEPS_PER_REVOLUTION, 1.0은 STEPS_PER_REVOLUTION로 변환

  // 방향 설정
  if (steps > 0) {
    digitalWrite(DIR, LOW);
  } else {
    digitalWrite(DIR, HIGH);
  }

  steps = abs(steps);  // 양의 값으로 변환
  
  for (int i = 0; i < steps; i++) {
    digitalWrite(ENA, HIGH);
    digitalWrite(PUL, HIGH);
    delayMicroseconds(100);
    digitalWrite(PUL, LOW);
    delayMicroseconds(100);
  }
}

void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;

    if (inChar == '\n') {
      stringComplete = true;
    }
  }

  if (stringComplete) {
    int commaIndex = inputString.indexOf(','); // 첫 번째 쉼표의 위치 찾기
    float value = inputString.substring(0, commaIndex).toFloat(); // 첫 번째 데이터를 float 값으로 변환

    if (value >= -1.0 && value <= 1.0) {
      moveByValue(value);  // 원하는 값만큼 움직이기
    } else {
      Serial.println("Invalid input. Please enter a value between -1.0 and 1.0.");
    }

    inputString = "";
    stringComplete = false;
  }
}
