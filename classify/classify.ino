/* classify.ino
** Processing Integration sketch for SIXT33N Music version
** 
** EE16B Spring 2016
** Emily Naviasky & Nathaniel Mailoa
**
*/

#include "./fix_fft.c"
#define MIC_INPUT P6_0

//data array and index pointer
int8_t re[128]={0};
int8_t im[128]={0};
volatile int re_pointer = 0;

// Loudness variables
#define SIZE 128
float sum = 0.0;
int16_t loudness = 0;

//timer period
float timer_ms = 0.35;
int i = 0;


/*---------------*/
/* CODE BLOCK A1 */
/*---------------*/

float pca_vec[SIZE] = {…}; // YOUR CODE HERE
float pca_mean[SIZE] = {…}; // YOUR CODE HERE

/*----------------------*/
/* END OF CODE BLOCK A1 */
/*----------------------*/


void setup()
{
  //microphone ADC
  pinMode(MIC_INPUT, INPUT);
  pinMode(P6_1, INPUT);
  pinMode(P6_5, INPUT);
  //recording light
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  re_pointer = 0;
  setTimer();
  reset_blinker();
  
  Serial.begin(38400); 
}

void loop()
{
  // This loop is entered every time the
  // data collection buffer is filled and
  // code is ready to start a new FFT
  if(re_pointer == 128){
    // Loudness calculation (with filter)
    // loudness ranges from 2500 to 4500
    sum = 0;
    for (int i = 0; i < SIZE; i++){
      sum += abs(re[i]);
    }      
    loudness = loudness*0.5 + sum*0.5;
    Serial.print("loudness: ");
    Serial.println(loudness);
    
    // make sure imaginary is set to zero
    memset(im, 0, sizeof(im));
    
    // take a 128-point fft
    fix_fft(re,im,7,0);


    /*--------------*/
    /* CODE BLOCK B */
    /*--------------*/
    
    // Modify this section to add your
    // data processing and decision making
    // YOUR CODE HERE
    
    // Classification
    // YOUR CODE HERE 
    
    /*---------------------*/
    /* END OF CODE BLOCK B */
    /*---------------------*/


    re_pointer = 0;
  }
}


#pragma vector=TIMER0_A0_VECTOR    // Timer A ISR
__interrupt void Timer1_A0_ISR( void )
{
  if(re_pointer < 128){
    digitalWrite(RED_LED, HIGH);
    re[re_pointer] = (analogRead(P6_0) >> 4) - 128;
    re_pointer += 1;
  }
}

void setTimer(){
  // set the timer based on 25MHz clock
  TA0CCR0 = (unsigned int)(25000*timer_ms);       
  // enable interrupts for Timer A
  TA0CCTL0 = CCIE;             
  __bis_SR_register(GIE);
  TA0CTL = TASSEL_2 + MC_1 + TACLR + ID_0;
}

void reset_blinker(){
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(RED_LED, HIGH);
  delay(100);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  delay(100);
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  delay(100);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  delay(100);
  digitalWrite(GREEN_LED, LOW);
}
