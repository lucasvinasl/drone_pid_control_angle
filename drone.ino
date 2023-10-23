// Biblioteca e objeto motor
#include <Servo.h>
Servo motor;

// Definir variáveis
float ang_saida;                                                                     // leitura em bits do angulo do motor
float erro_atual = 0, soma_erro = 0, soma_erro_ant = 0, dif_erro = 0, erro_ant = 0;  // erros
float setPoint = 90;                                                                 // setPoint fixado de 90
float kp = 0, ki = 0, kd = 0;                                                        //AUMENTAR KP PARA PARAR DE BATER
float p = 0, i = 0, d = 0;
int cont = 0;  // contador de amostras
float z = 0;


// Definindo função de controle do PID
float funcLimite(float kp, float ki, float kd, float erro_atual, float soma_erro, float dif_erro) {

  float p = kp * erro_atual;
  float i = ki * soma_erro;
  float d = kd * dif_erro;

  float x = p + i + d;
  x = x + 70;

  return x;
}


void setup() {

  // sinal de controle do motor
  motor.attach(5);
  delay(10000);

  // partida do motor - tirar de inércia com rotação mínima
  for (int i = 0; i < 75; i++) {
    motor.write(i);
    delay(15);
  }

  Serial.begin(9600);
}

void loop() {

  cont++;

  // angulo do acelerometro
  int angulo;
  ang_saida = 0;  // angulo em bits

  for (int i = 1; i <= 600; i++) {
    // leitura do acelerometro
    angulo = analogRead(A0);
    ang_saida = ang_saida + angulo;
  }

  // calculado os erros de leituras para jogar no PID
  ang_saida = ang_saida / 600;

  ang_saida = (-0.375 * (ang_saida) + 261.98);
  erro_atual = ang_saida - setPoint;
  dif_erro = erro_atual - erro_ant;
  soma_erro = soma_erro_ant + erro_atual;

  // valores de ciclos para os angulos de ataque
  if (cont <= 300) {
    setPoint = 90;
    kp = 0.01;
    ki = 0.00499;
    kd = 0.05009;
  } else if (cont <= 500) {
    setPoint = 115;
    kp = 0.015;
    ki = 0.006;
    kd = 0.06;

  } else if (cont <= 700) {
    setPoint = 90;
  } else if (cont <= 900) {
    setPoint = 60;
  } else {
    setPoint = 90;
    kp = 0.014;
    ki = 0.00505;
    kd = 0.05009;
  }


  // Função de saída
  z = funcLimite(kp, ki, kd, erro_atual, soma_erro, dif_erro);
  Serial.print("   PID: ");
  Serial.print(z);
  Serial.print("   SetPoint: ");
  Serial.print(setPoint);
  Serial.print("   Angulo: ");
  Serial.print(ang_saida);

  if (z > 178) {
    z = 178;
    motor.write(z);

  } else if (z < 76) {
    z = 75;
    motor.write(z);

  } else {
    motor.write(z);
  }
  Serial.print("  Contador: ");
  Serial.println(cont);

  erro_ant = erro_atual;
  soma_erro_ant = soma_erro;
}