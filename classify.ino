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
#define SIZE 64
float sum = 0.0;
int16_t loudness = 0;

//timer period
float timer_ms = 0.35;
int i = 0;

int numSamples = 0;
int currSum = 0;

/*---------------*/
/* CODE BLOCK A1 */
/*---------------*/

float pca_vec[SIZE] = {0.0454235320115, 0.0833433465979, 0.0523490740498, 0.0273970098584, 0.0159031576311, 0.0106421780887, 0.0353039245288, 0.0246552559277, 0.0149484980867, 0.0483212670142, 0.0166723052959, -0.00162551148454, 0.0149360673377, 0.0441784735446, 0.0262002828474, 0.0321929783614, 0.0365313740695, 0.0493877233093, 0.0406881691054, 0.0310848082124, 0.0279219518908, 0.0163531390321, 2.41786515769e-05, 0.0347701258818, 0.0691084516984, 0.217912659838, 0.0986368775651, 0.0551738965652, 0.108904977726, 0.065612742116, 0.102675637947, 0.307756196984, 0.528286867173, 0.314953303036, 0.209971390881, 0.0520755124445, 0.0588622233274, 0.204084721866, 0.152789095613, 0.0624855705812, 0.0288489917552, 0.0600677855841, 0.0820259050437, 0.0718611438542, 0.0328147600461, 0.0430775936321, 0.10322660152, 0.243643627455, 0.234140069054, 0.188580368323, 0.186891408956, 0.203283763713, 0.0649805402809, 0.0324393316411, 0.0294183751705, 0.063394990453, 0.076044294734, 0.0724260052506, 0.0297776343848, 0.0123345866693, 0.0116474396285, 0.0319812880284, 0.0391822808722, 0.0717464531462};

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
    if (loudness > 500) {
  
      float sample[64];
      for (int i = 0; i < 64; i++) {
          float re_i = re[2*i];
          float im_i = im[2*i + 1];
          sample[i] = sqrt(re_i*re_i + im_i*im_i);
      }
  
      /*float sample_cut[54];
      for (int i = 0; i < 54; i++) {
          sample_cut[i] = sample[i + 10];
      }*/
  
      float dot = 0.0;
      for (int i = 0; i < 64; i++) {
          dot += pca_vec[i] * sample[i];
      }
  
      Serial.print("dot: ");
      Serial.println(dot);
      int toAdd = 0;
      numSamples++;
      if (dot > 5.6) {
          //Serial.println("ARIANA GRANDE");
          toAdd = 0;
      } else {
          //Serial.println("MISTY MOUNTAINS");
          toAdd = 1;
      }
      currSum += toAdd;
      if (numSamples > 10) {
        numSamples = 0;
        if (currSum < 5) {
          Serial.println("Ariana Grande");
        } else {
          Serial.println("Misty mountains");
        }
        currSum = 0;
      }
    } else {
      numSamples = 0;
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

