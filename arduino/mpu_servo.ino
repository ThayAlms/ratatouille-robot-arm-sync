// Controle de 2 servos a partir de MPU6050 (complementary filter)
// Requer apenas Wire.h e Servo.h
// Pinos de servo: 9 e 10 (ajuste conforme necessário)

#include <Wire.h>
#include <Servo.h>
#include <math.h>

// ----- Configurações de pinos -----
const int SERVO_PIN[2] = {9, 10};

// ----- Objetos -----
Servo servoA, servoB;

// ----- MPU6050 registers -----
const int MPU_ADDR = 0x68; // endereço I2C padrão

// ----- Sensores bruto -----
int16_t accX, accY, accZ;
int16_t gyroX, gyroY, gyroZ;

// ----- Filtro complementar -----
float pitch = 0.0; // rotação no eixo X
float roll  = 0.0; // rotação no eixo Y
unsigned long lastTime = 0;
const float alpha = 0.98; // quanto confiar no giroscópio (0..1). Aumente para menos "burbulha" do acel.

// ----- Mapeamento para servo -----
const float ANGLE_MIN = -60.0; // ângulo esperado mínimo do sensor (ajustar)
const float ANGLE_MAX = 60.0;  // ângulo esperado máximo do sensor (ajustar)
const int SERVO_MIN = 0;
const int SERVO_MAX = 180;

// Offsets / inversões por servo (ajuste para seu mecanismo)
int offsetServoDeg[2] = {0, 0}; // deslocamento em graus para cada servo
bool invertServo[2] = {false, false};

// Suavização (lerp) - 0 = sem suavização, 1 = travado. Use algo pequeno como 0.2
const float smoothing = 0.20;

// Função auxiliar lerp
float lerp(float a, float b, float t) {
  return a + (b - a) * t;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  // Inicializa MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // PWR_MGMT_1
  Wire.write(0);    // sair do sleep
  Wire.endTransmission(true);

  // configura escala (opcional): assumiremos valores padrão (±250 dps e ±2g)
  // attach servos
  servoA.attach(SERVO_PIN[0]);
  servoB.attach(SERVO_PIN[1]);

  // inicializa tempo
  lastTime = micros();

  // Pequena pausa
  delay(100);
  Serial.println("MPU6050 + 2 servos - iniciando");
}

void loop() {
  unsigned long now = micros();
  float dt = (now - lastTime) / 1000000.0; // segundos
  if (dt <= 0) dt = 0.001;
  lastTime = now;

  // Ler dados do MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // ACCEL_XOUT_H
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);

  accX = Wire.read() << 8 | Wire.read();
  accY = Wire.read() << 8 | Wire.read();
  accZ = Wire.read() << 8 | Wire.read();
  int16_t temp = Wire.read() << 8 | Wire.read(); // não usado
  gyroX = Wire.read() << 8 | Wire.read();
  gyroY = Wire.read() << 8 | Wire.read();
  gyroZ = Wire.read() << 8 | Wire.read();

  // Converções:
  // acelerômetro raw -> g (±2g): sensibilidade ~16384 LSB/g (se default)
  float ax = accX / 16384.0;
  float ay = accY / 16384.0;
  float az = accZ / 16384.0;

  // giroscópio raw -> deg/s (±250 dps): sensibilidade ~131 LSB/(deg/s)
  float gx = gyroX / 131.0;
  float gy = gyroY / 131.0;
  float gz = gyroZ / 131.0;

  // --- calcular ângulos pelo acelerômetro (em graus) ---
  float rollAcc  = atan2(ay, az) * 180.0 / M_PI; // rotaçao ao redor do eixo X
  float pitchAcc = atan2(-ax, sqrt(ay*ay + az*az)) * 180.0 / M_PI; // rotação ao redor eixo Y

  // --- integrar giroscópio (rate) ---
  // gx, gy fornecem rotações em deg/s (eixo depende do sensor/posição)
  // Ajuste sinais se necessário
  float gyroPitchRate = gy; // pode precisar trocar sinais dependendo da orientação física
  float gyroRollRate  = gx;

  // Complementary filter
  pitch = alpha * (pitch + gyroPitchRate * dt) + (1 - alpha) * pitchAcc;
  roll  = alpha * (roll  + gyroRollRate  * dt) + (1 - alpha) * rollAcc;

  // Mapear ângulos para servo
  int servoAngle0 = angleToServo(roll, 0);
  int servoAngle1 = angleToServo(roll, 1); 

  // Aplicar suavização
  static float lastServo[2] = {90.0, 90.0};
  lastServo[0] = lerp(lastServo[0], servoAngle0, smoothing);
  lastServo[1] = lerp(lastServo[1], servoAngle1, smoothing);

  servoA.write((int)round(lastServo[0]));
  servoB.write((int)round(lastServo[1]));

  // Debug opcional
  Serial.print("Pitch: "); Serial.print(pitch,2);
  Serial.print("  Roll: "); Serial.print(roll,2);
  Serial.print("  S0: "); Serial.print((int)lastServo[0]);
  Serial.print("  S1: "); Serial.println((int)lastServo[1]);

  delay(10); // ajuste o loop (10 ms ~ 100Hz)
}

int angleToServo(float angleDeg, int servoIndex) {
  // aplica offset e inversão
  float a = angleDeg + offsetServoDeg[servoIndex];
  if (invertServo[servoIndex]) a = -a;
  // map de ANGLE_MIN..ANGLE_MAX -> SERVO_MIN..SERVO_MAX
  float mapped = (a - ANGLE_MIN) * (SERVO_MAX - SERVO_MIN) / (ANGLE_MAX - ANGLE_MIN) + SERVO_MIN;
  if (mapped < SERVO_MIN) mapped = SERVO_MIN;
  if (mapped > SERVO_MAX) mapped = SERVO_MAX;
  return (int)round(mapped);
}
