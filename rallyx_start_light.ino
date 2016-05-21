#include "TimerOne.h"

#define PIN_LED     13
#define PIN_RED     6
#define PIN_YELLOW  5
#define PIN_GREEN   3
#define PIN_TRIGGER 9
#define PIN_POT_HI  A3
#define PIN_POT     A2
#define PIN_POT_LO  A1

#define DEBOUNCE_MAX 50 // milliseconds

#define DEBUG 0

#if DEBUG
  #define DEBUG_PRINT(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
#endif

enum fsm_state_t {
  STATE_NO_TRIG_WAIT,
  STATE_TRIG_WAIT,
  STATE_YELLOW,
  STATE_GREEN,
  STATE_ABORT
};

enum color_t {
  OFF,
  RED,
  YELLOW,
  GREEN
};

volatile uint32_t ms_time = 0;

void light_color( enum color_t color );
void light(bool r, bool y, bool g);
void loop_delay(void);

void timer1_tick(void)
{
  ms_time++;
}

void setup()
{
  pinMode(PIN_TRIGGER, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_YELLOW, OUTPUT);
  pinMode(PIN_RED, OUTPUT);

  pinMode(PIN_POT_HI, OUTPUT);
  pinMode(PIN_POT_LO, OUTPUT);
  pinMode(PIN_POT, INPUT);

  digitalWrite(PIN_POT_HI, HIGH);
  digitalWrite(PIN_POT_LO, LOW);
  
  Serial.begin(115200); // debug output

  DEBUG_PRINT("setup()");

  digitalWrite(PIN_LED, HIGH);

  light_color(RED);
  DEBUG_PRINT("RED");
  delay(1000);
  light_color(YELLOW);
  DEBUG_PRINT("YELLOW");
  delay(1000);
  light_color(GREEN);
  DEBUG_PRINT("GREEN");
  delay(1000);
  light_color(YELLOW);
  DEBUG_PRINT("YELLOW");
  delay(1000);
  light_color(RED);
  DEBUG_PRINT("RED");

  digitalWrite(PIN_LED, LOW);

  // use Timer1 for accurate loop delays
  Timer1.initialize();
  Timer1.attachInterrupt(timer1_tick,1000); // 1ms period
  Timer1.start();
}

void loop()
{
  enum fsm_state_t state = STATE_ABORT;
  int count = 0;
  bool active = false;
  uint8_t debounce_count = 0;

  DEBUG_PRINT("loop()");
  DEBUG_PRINT("STATE_ABORT");

  while(true)
  {
    // debounce input trigger, input is active low
    if( !digitalRead(PIN_TRIGGER) )
    {
      debounce_count = debounce_count < DEBOUNCE_MAX ? debounce_count + 1 : DEBOUNCE_MAX;
    }
    else
    {
      debounce_count = debounce_count > 0 ? debounce_count - 1 : 0;
    }

    if( debounce_count == DEBOUNCE_MAX && active == false)
    {
      active = true;
      digitalWrite(PIN_LED, HIGH);
      DEBUG_PRINT("active = true");
    }
    
    if( debounce_count == 0 && active == true )
    {
      active = false;
      digitalWrite(PIN_LED, LOW);
      DEBUG_PRINT("active = false");
    }

    switch( state )
    {
      case STATE_NO_TRIG_WAIT:
        // wait for trigger to be deactivated
        light_color(RED);
        count = active ? 0 : count + 1;

        if( count >= 250 )
        {
          count = 0;
          state = STATE_TRIG_WAIT;
          DEBUG_PRINT("STATE_TRIG_WAIT");
        }
        break;

      case STATE_TRIG_WAIT:
        light_color(RED);
        count = active ? count + 1 : 0;

        if( count >= 250 )
        {
           count = 0;
           state = STATE_YELLOW;
          DEBUG_PRINT("STATE_YELLOW");
        }
        break;

      case STATE_YELLOW:
        if( (count % 1000) < 500 )
        {
          light_color(YELLOW);
        }
        else
        {
          light_color(OFF);
        }

        count++;

        if( active == false )
        {
          count = 0;
          state = STATE_ABORT;
          DEBUG_PRINT("STATE_ABORT");
        }
        else if( count >= 3000)
        {
          count = 0;
          state = STATE_GREEN;
          DEBUG_PRINT("STATE_GREEN");
        }
        else
        {
          if( count % 1000 == 0 )
          {
            DEBUG_PRINT(count / 1000);
          }
        }
        break;

      case STATE_GREEN:
        light_color(GREEN);

        count++;

        if( active == false )
        {
          count = 0;
          state = STATE_ABORT;
          DEBUG_PRINT("STATE_ABORT");
        }
        else if( count >= 30000)
        {
          count = 0;
          state = STATE_NO_TRIG_WAIT;
          DEBUG_PRINT("STATE_NO_TRIG_WAIT");
        }
        else
        {
          if( count % 1000 == 0 )
          {
            DEBUG_PRINT(count / 1000);
          }
        }
        break;

      case STATE_ABORT:
      default:
        if( (count % 500) < 250 )
        {
          light_color(RED);
        }
        else
        {
          light_color(OFF);
        }
        
        count++;

        if( count >= 3000 )
        {
          count = 0;
          state = STATE_NO_TRIG_WAIT;
          DEBUG_PRINT("STATE_NO_TRIG_WAIT");
        }
        else
        {
          if( count % 1000 == 0 )
          {
            DEBUG_PRINT(count / 1000);
          }
        }
        break;
    }

    // loop executes once per millisecond
    loop_delay();
  }
}

void loop_delay(void)
{
  static uint32_t t = 0;
  while(t == ms_time);
  t = ms_time;
}

void light_color( enum color_t color )
{
  switch( color )
  {
    case RED:
      light(1,0,0);
      break;
    case YELLOW:
      light(0,1,0);
      break;
    case GREEN:
      light(0,0,1);
      break;
    case OFF:
      light(0,0,0);
      break;
  }
}

void light(bool r, bool y, bool g)
{
  uint16_t pwm = analogRead(PIN_POT);
  pwm = pwm >> 2;
  // bound limits so we have a min light and at max the full output is used.
  if( pwm > 245 )
  {
    pwm = 255;
  }
  else if( pwm < 10 )
  {
    pwm = 10;
  }
  // pwm is a 10bit value, shift it down to 8bit output
  analogWrite(PIN_RED, pwm * r);
  analogWrite(PIN_YELLOW, pwm * y);
  analogWrite(PIN_GREEN, pwm * g);
}

