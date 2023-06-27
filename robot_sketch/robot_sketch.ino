#include <Servo.h> 

#define InfraRot  2    // Der Sharp IR Sesor ist an Pin A2

#define rLED      6    //rote LED
#define gLED      7    //grüne LED

#define N_Vorne_A     12   // Richtung A
#define N_Vorne_B     13   // Richtung B

#define PWM_A     3    // Geschwindigkeit A
#define PWM_B     11   // Geschwindigkeit B

#define Bremse_A   9    // Bremse A
#define Bremse_B   8    // Bremse B

#define S_ervo     5    // Servo wird an Pin 5 angesteuert


#define FahrGeschw         125             // 255 ist MaxGeschw
#define DrehGeschw         255             // Drehen
#define Links              LOW             // Links drehen bei LOW
#define Rechts             HIGH            // Rechts drehen bei HIGH
#define MaxNaehe           350             // Maximaler Sensorwert (Nähe)  
#define ServoRechts        50              // Servo - Winkel rechts
#define ServoLinks         170             // Servo - Winkel links

Servo DerServo;  




// An welcher Servo Position ist die Distanz zum Hindernis am geringsten?
int SuchHindernis()
{
  digitalWrite(Bremse_A, HIGH); 
  digitalWrite(Bremse_B, HIGH);
  
  digitalWrite(gLED, LOW);
  digitalWrite(rLED, HIGH);
  digitalWrite(gLED, LOW);

  int ServoPos;
  int MinDist = 0;
  int MinServoPos = 0;
 
  // Von Rechts nach Links alle Servo Positionen anfahren.
  for(ServoPos = ServoRechts; ServoPos <= ServoLinks; ServoPos++)
  {
    // Servo Wert einstellen
    DerServo.write(ServoPos);
    delay(5);
    // aktueller Sensor Wert näher, als der minimale Wert?
    if(analogRead( InfraRot ) > MinDist )
    {
      // Ja: aktueller Wert = neues Minimum
      MinDist = analogRead( InfraRot );
      //Servo Position merken
      MinServoPos = ServoPos; 
    }   
  }
  // Die gefundene Servoposition einstellen und Wert zurückgeben
  DerServo.write(MinServoPos);
  return MinServoPos;
}

int ServoPos = 90;
boolean Drehung = Rechts;

// Vorwärts fahren und dabei den Sensor links/rechts bewegen
void Fahren()
{
  DerServo.write( ServoPos );
  //Drehrichtung einstellen
  digitalWrite( N_Vorne_A, HIGH );
  digitalWrite( N_Vorne_B, HIGH );
  
  //Drehgeschwindigkeit einstellen
  analogWrite( PWM_A, FahrGeschw );
  analogWrite( PWM_B, FahrGeschw );
  
  //Bremsen lösen
  digitalWrite( Bremse_A , LOW );
  digitalWrite( Bremse_B , LOW );
  
  // Servo weiter in bisher bewegte Richtung bewegen
  if( Drehung == Links )
    ServoPos = ServoPos+1;  
  if( Drehung == Rechts ) 
    ServoPos = ServoPos-1;  
    
  // beim erreichen des Grenzwertes in andere Richtung drehen 
  if( ServoPos > ServoLinks )
    Drehung = Rechts;
  if( ServoPos < ServoRechts )
    Drehung = Links; 
}



// Drehen
void Drehen( boolean Richtung )
{
  delay( 500 );

  digitalWrite( N_Vorne_A, Richtung );    // Motor A in die "RICHTUNG" drehen
  digitalWrite( N_Vorne_B, !Richtung );   // Motor B in entgegen der "RICHTUNG" drehen
   
  // Geschwindigkeit für das Drehen einstellen
  analogWrite( PWM_A, DrehGeschw );
  analogWrite( PWM_B, DrehGeschw );
  
  // Bremsen lösen
  digitalWrite( Bremse_A, LOW );
  digitalWrite( Bremse_B, LOW );
  
  // Drehen bis Sensor größere Entfernung misst
  while( MaxNaehe < analogRead( InfraRot ) )
  {
   delay( 50 );
  }
  
  digitalWrite( Bremse_A, HIGH ); 
  digitalWrite( Bremse_B, HIGH );
  delay(1000);
}








void setup( )
{
  //Motor A (rechts) initialisieren
  pinMode( N_Vorne_A, OUTPUT );    // Pin für Richtung Motor A als Ausgang definieren
  pinMode( Bremse_A, OUTPUT );  // Pin für Bremse Motor A als Ausgang definieren
  
  //Motor B (links) initialisieren
  pinMode( N_Vorne_B, OUTPUT );    // Pin für Richtung Motor B als Ausgang definieren
  pinMode( Bremse_B, OUTPUT );  // Pin für Bremse Motor B als Ausgang definieren

  digitalWrite( Bremse_A, HIGH );
  digitalWrite( Bremse_B, HIGH );

  pinMode(rLED, OUTPUT);
  pinMode(gLED, OUTPUT);

  // Servo initialiseren und zentrieren
  DerServo.attach( S_ervo );
  DerServo.write(110);
  delay( 500 );

   while( MaxNaehe  < analogRead( InfraRot ) )
  {
    delay( 100 );
  }
}








void loop( )
{
  digitalWrite(rLED, LOW);
  digitalWrite(gLED, HIGH);
  
  int Hindernis;  
  int Entfernung;
  
  Entfernung = analogRead( InfraRot );    // Messung des Sensors
  if(Entfernung > MaxNaehe )       // je größer der Wert desto näher das Hindernis
  {
    Hindernis = SuchHindernis();  // position des Hindernisses erkennen

     // Hindernis links
    if(Hindernis <= 50)
    {
      Drehen( Rechts );  // Rechts drehen
    }
    // Hindernis rechts
    if( Hindernis > 50 )
    {
      Drehen( Links );   // Links drehen
    }

  }
  
  Fahren();
  delay( 10 );
}
