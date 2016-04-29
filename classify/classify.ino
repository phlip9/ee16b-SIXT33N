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
#define SIZE 54
float sum = 0.0;
int16_t loudness = 0;

//timer period
float timer_ms = 0.35;
int i = 0;

/*---------------*/
/* CODE BLOCK A1 */
/*---------------*/

float pca_vec[SIZE] = {-0.488770338522, -0.643855175887, -0.261603229122, -0.270541402687, -0.148655267695, 0.260156144459, -0.104715190793, 0.186803972462, 0.0165952910963, -0.124684878916, -0.158571795179, 0.0508069253774, 0.0823181872481, -0.121820083939, 0.0447286307149, 0.0110834211124, 0.0147160755336, 0.0218893363327, -0.00885838457187, 0.00954396034742, 0.00108778305059, 0.010844340259, -0.00601341487712, 0.00203741475892, 0.00110274942828, 0.0126500593489, 0.00370299029689, -0.00600995828807, 0.0107646966934, -0.00788752296546, -0.0018484830367, 0.00551864448651, -0.00691775481491, -0.00302018771757, -0.00910023043392, -0.00290255998421, -8.62166152623e-05, 0.00172605114597, 0.0070156015266, -0.000358873520847, -0.00932947166191, -0.000510280313892, 0.00677154873586, 0.0070425584482, -0.00821548967786, -0.00464736416273, -0.00322001629319, 0.00128330913423, 0.00128096772976, -0.0038641754756, 0.00463121341691, 0.00319244788801, 0.000892393954332, 0.00397019280257}; // YOUR CODE HERE
/*float pca_mean[SIZE] = {…}; // YOUR CODE HERE*/

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

    float sample[64];
    for (int i = 0; i < 64; i++) {
        float re_i = re[2*i];
        float im_i = im[2*i + 1];
        sample[i] = sqrt(re_i*re_i + im_i*im_i);
    }

    float sample_cut[54];
    for (int i = 0; i < 54; i++) {
        sample_cut[i] = sample[i + 10];
    }

    float dot = 0.0;
    for (int i = 0; i < 54; i++) {
        dot += pca_vec[i] * sample_cut[i];
    }

    Serial.print("dot: ");
    Serial.print(dot);
    if (dot > -5 && dot < 5) {
        Serial.println("; SICK VIOLIN SOLO.csv");
    } else {
        Serial.println("; mm.csv");
    }
    
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

