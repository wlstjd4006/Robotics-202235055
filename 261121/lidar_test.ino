/*
 * Back  :    0°
 * Left  :   90°
 * Front :  180°
 * Right :  270°
*/

/*
 * Byte 0       : Header (0x54)
 * Byte 1       : VerLen (0x2C)
 * Byte 2~3     : 회전 속도
 * Byte 4~5     : 시작 각도
 * Byte 6~41    : 거리 측정값 12개
 *                각 측정점 = 3 byte
 *                distance 2 byte + confidence 1 byte
 * Byte 42~43   : 끝 각도
 * Byte 44~45   : timestamp
 * Byte 46      : CRC
*/


#define LIDAR_RX 16

uint8_t packet[47];
uint16_t distances[360] = {0};

float previousAngle = -1.0;


// LiDAR가 low byte, high byte 순서로 보낸 2바이트를 하나의 16비트 정수로 합친다
uint16_t readUInt16(uint8_t low, uint8_t high) {
    return low | (high << 8);
}


// 0~359도에 저장된 거리값을 모두 출력한다
void printAllDistances() {
    for (int degree = 0; degree < 360; degree++) {
        Serial.print(degree);
        Serial.print(" deg : ");
        Serial.print(distances[degree]);
        Serial.println(" mm");
    }

    Serial.println("----------------");
}


void setup() {
    Serial.begin(115200);  // 시리얼 모니터
    Serial2.begin(230400, SERIAL_8N1, LIDAR_RX, -1);  // LiDAR -> RX 시리얼 통신 (TX 사용안함)
}


void loop() {
    if (!Serial2.available()) {
        return;
    }

    // packet의 첫 byte는 항상 0x54
    if (Serial2.read() != 0x54) {
        return;
    }

    // 두 번째 byte가 들어올 때까지 기다린다
    while (!Serial2.available()) {
    }

    // 두 번째 byte가 0x2C인지 확인
    if (Serial2.read() != 0x2C) {
        return;
    }

    packet[0] = 0x54;
    packet[1] = 0x2C;

    // header 2바이트를 제외한 나머지 packet 데이터를 읽는다
    for (int i = 2; i < 47; i++) {
        while (!Serial2.available()) {
        }

        packet[i] = Serial2.read();
    }

    // 시작 각도와 끝 각도는 각각 2바이트
    uint16_t startRaw = readUInt16(packet[4], packet[5]);
    uint16_t endRaw = readUInt16(packet[42], packet[43]);

    // 각도값은 0.01도 단위로 들어오므로 100으로 나눈다
    float startAngle = startRaw / 100.0;
    float endAngle = endRaw / 100.0;

    if (endAngle < startAngle) {
        endAngle += 360.0;
    }

    // 한 packet에는 12개 측정점이 있으므로 시작점과 끝점 사이를 11구간으로 나눈다
    float angleStep = (endAngle - startAngle) / 11.0;

    for (int i = 0; i < 12; i++) {
        // 각 측정점은 distance 2바이트 + confidence 1바이트로 구성
        int index = 6 + i * 3;

        // 거리값 2바이트를 mm 단위 정수로 변환
        uint16_t distance = readUInt16(packet[index], packet[index + 1]);

        // 현재 측정점이 어느 각도에 해당하는지 계산
        float angle = startAngle + angleStep * i;

        if (angle >= 360.0) {
            angle -= 360.0;
        }

        // 360도에서 0도로 넘어오면 한 바퀴 완료로 보고 전체 거리값 출력
        if (previousAngle > 300.0 && angle < 60.0) {
            printAllDistances();
        }

        // float 각도를 가장 가까운 정수 각도로 반올림
        int degree = (int)(angle + 0.5);

        if (degree >= 360) {
            degree = 0;
        }

        // 0은 유효하지 않은 거리값으로 보고 저장하지 않는다
        if (distance > 0) {
            distances[degree] = distance;
        }

        previousAngle = angle;
    }
}
