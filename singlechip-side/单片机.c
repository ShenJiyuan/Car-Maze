/*

The command string recived from PC has 6 bit(chars):
Bit 0 is '$' as start bit
Bit 1 is direction bit : forwad or backward
Bit 2 is direction bit : left or right
Bit 3 is angle value bit
Bit 4 is speed value bit
Bit 5 is '#' :stop bit

*/
#include <msp430g2553.h>
unsigned char command_s[7],precomm_s[7],j=0,flag=1,right_speed=0,left_speed=0;
#define anglecycle  3000
#define anglecenter  1500
#define anglecorref  50

void INITIALIZATION(void)
{
WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  /*
   * Clock Set.
   */
  if (CALBC1_1MHZ==0xFF)		// If calibration constant erased
  {
    while(1);                               // do not load, trap CPU!!
  }
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
  DCOCTL = CALDCO_1MHZ;

  /*
   * USCI Set:UART,9600baud,P1.1 = RXD, P1.2=TXD,enable RX interrupt.
   */
  P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
  P1SEL2 = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 104;                            // 1MHz 9600
  UCA0BR1 = 0;                              // 1MHz 9600
  UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt

  /*
   * TimerA1:PWM1 and PWM2 for Motor.
   */
  P2DIR |= (BIT1+BIT4+BIT2+BIT5);
  //P2SEL |= (BIT1+BIT4+BIT2+BIT5);           //Enable the P2.1~P2.2 and P2.4~P2.5 peripheral PWM module.
  TA1CCR0 = 200;                           //PWM cycle
  TA1CCTL1 = OUTMOD_7;                      //TimerA1 CCTL1 (p2.1 and P2.2) output mode 7
  TA1CCTL2 = OUTMOD_7;                      //TimerA1 CCTL2 (p2.4 and P2.5) output mode 7
  TA1CTL = TASSEL_2 + MC_1;                 //Timer A mode control: 1 - Up to CCR0 and start timer

  /*
   * TimerA0:PWM0 for angle of the servo.
  */
  //P1DIR |= BIT6;                            //P1.6 set output Pulsewidth-->Angle
 // P2SEL &= ~BIT7;
  P1SEL |= BIT6;                            //Enable the P1.6 PWM function
 // P2SEL2 &= ~BIT6;
 // P2SEL2 &= ~BIT7;
  TA0CCR0 = anglecycle;                     //set angle PWM cycle :20ms
  TA0CCR1 = anglecenter;                    //init angle pulse width
  TA0CCTL1 = OUTMOD_7;                      //PWM output mode
  TA0CTL = TASSEL_2 + MC_1;                 //Timer A mode control: 3 - Up/Down and start timer

  P2DIR |=BIT0 + BIT3 +BIT7 ;


  __bis_SR_register(GIE);                     // interrupts enabled

}

void delay(unsigned long time)
{
	volatile unsigned i;
	i = time;
	do (i--);
	while (i != 0);
}

void MOTOR_RIGHT_BACK(void)
{
    delay(1000);
    P2SEL &= ~BIT4;          //stop p2.2(backward)PWM output
    delay(2000);            //deadzero
    P2OUT &= ~BIT4;
    P2SEL |= BIT5 ;          //start p2.1(forward)PWM output,dead
 }

void MOTOR_RIGHT_FORWARD(void)
{
    delay(1000);
    P2SEL &= ~BIT5;          //stop p2.2(backward)PWM output
    delay(2000);            //deadzero
    P2OUT &= ~BIT5;
    P2SEL |= BIT4 ;          //start p2.1(forward)PWM output,dead
}

void MOTOR_LEFT_BACK(void)
{
    delay(1000);
    P2SEL &= ~BIT1;          //stop p2.2(backward)PWM output
    delay(2000);            //deadzero
    P2OUT &= ~BIT1;
    P2SEL |= BIT2 ;          //start p2.1(forward)PWM output,dead
}

void MOTOR_LEFT_FORWARD(void)
{
    delay(1000);
    P2SEL &= ~BIT2;          //stop p2.2(backward)PWM output
    delay(2000);            //deadzero
     P2OUT &= ~BIT2;
    P2SEL |= BIT1;          //start p2.1(forward)PWM output,dead
}

void MOTOR_RIGHT_SPEED(unsigned char speed)
{
TA1CCR1 = 23*speed;        //change pulse width for right wanted "speed"
}

void MOTOR_LEFT_SPEED(unsigned char speed)
{
TA1CCR2 =  23*speed;        //change pulse width for right wanted "speed"
}

void SERVO_ANGLE(unsigned char dir,unsigned char angle)
{
if(dir==1)                            //if rightward
{
P1DIR |= BIT6;                         //servo PWM output
TA0CCR1 =anglecenter + anglecorref*angle;  //change pulse width for rightward
delay(2*anglecycle);
P2OUT |= BIT3;
P2OUT &= BIT0;
P1DIR &= ~BIT6;                         //stop servo PWM output for servo steady
}
else if(dir==0)                        //if leftward
{
P1DIR |= BIT6;                         //servo PWM output
TA0CCR1 = anglecenter - anglecorref*angle;  //change pulse width for leftward
delay(2*anglecycle);
P2OUT |= BIT0;
P2OUT &= BIT3;
P1DIR &= ~BIT6;                         //stop servo PWM output for servo steady
}
}

void main(void)
{
 INITIALIZATION();                    //init
 SERVO_ANGLE(0,0);
 //MOTOR_RIGHT_SPEED(9);
 //MOTOR_LEFT_SPEED(9);//servo center
 MOTOR_RIGHT_FORWARD();               //right forward
 MOTOR_LEFT_FORWARD();                //left forward
 MOTOR_RIGHT_SPEED(0);                //right medium speed
 MOTOR_LEFT_SPEED(0);                 //left medium speed
 while(1)
 {

   if(flag==1)                       //if command content changes
  { flag=0;                          //clear change flag
  if(command_s[5]==1)                //if run diriction changes
  {
    if(!command_s[1])                //if forward
    {
    MOTOR_RIGHT_FORWARD();
    MOTOR_LEFT_FORWARD();
    }
    else if(command_s[1])              //if backward
   {
    MOTOR_RIGHT_BACK();
    MOTOR_LEFT_BACK();
   }
  }
  if((command_s[5]==2)||(command_s[5]==3)) //if angle change

  {
    SERVO_ANGLE(command_s[2],command_s[3]);
    if(command_s[2])
    {
    MOTOR_RIGHT_FORWARD();               //right forward
 MOTOR_LEFT_BACK();                //left forward
 MOTOR_RIGHT_SPEED(6);                //right medium speed
 MOTOR_LEFT_SPEED(6);
    }
    else
    {
     MOTOR_RIGHT_BACK();               //right forward
 MOTOR_LEFT_FORWARD();                //left forward
 MOTOR_RIGHT_SPEED(6);                //right medium speed
 MOTOR_LEFT_SPEED(6);
    }
  }
   if(command_s[5]==4)
   {
    right_speed = command_s[4];
    left_speed = command_s[4] ;
    MOTOR_RIGHT_SPEED(right_speed);
   MOTOR_LEFT_SPEED(left_speed);
   }
  }

 }
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)             //recive command
{
	//__bis_SR_register_on_exit(LPM0);
	unsigned char message_c,i=0;
	message_c=UCA0RXBUF;
        UCA0TXBUF = UCA0RXBUF;
	if(message_c=='$')
	{
		j=0;
		precomm_s[j]='$';
                j++;
	}
        else if((precomm_s[0]=='$')&&(j>0)&&(j<6)&&(message_c!='#'))
        {
        precomm_s[j] = message_c;
        j++;
        }
        else if((precomm_s[0]=='$')&&(message_c=='#')&&(j==6))
        {
          precomm_s[j] = message_c;
          for(i=0;i<=6;i++)
         command_s[i] = precomm_s[i]-'0' ;
          flag=1;
          j=7;
        }
	else j=7;
}
