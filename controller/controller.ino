static unsigned long const baud_rate = 57600; // Ca. 5760 bytes per second, we send one byte per state so 100 bytes per second.

static int const pin_clk    = 2; // Red, the clock.
static int const pin_d0     = 3; // Yellow, the data PIN (corresponds to Q8 or Q7 for workaround).
static int const pin_strobe = 4; // Orange, resets the shift register.

// The first bit to be read. This is usually 1 but can be used as a workaround if D0 is connected to Q7 instead of Q8
// and the A button is sent last.
static int const first_bit = 1;

static uint8_t read();

void setup()
{
  Serial.begin(baud_rate);

  pinMode(pin_clk, OUTPUT);
  pinMode(pin_d0, INPUT);
  pinMode(pin_strobe, OUTPUT);

  digitalWrite(pin_clk, LOW);
  digitalWrite(pin_strobe, LOW);
}

void loop()
{
  static auto last = uint8_t{ 0 };
  auto const curr = read();
  if (curr != last)
  {
    Serial.write(~curr);
    last = curr;
  }
  delay(10);
}

uint8_t read()
{
  // https://web.archive.org/web/20160709181841/http://www.mit.edu/~tarvizo/nes-controller.html
  auto res = uint8_t{ 0 };

  digitalWrite(pin_strobe, HIGH);
  delayMicroseconds(12);
  digitalWrite(pin_strobe, LOW);

  for (int i = 0; i < 8; ++i)
  {
    uint8_t button = digitalRead(pin_d0);

    int bit = (first_bit + i) % 8;
    res |= button << bit;

    delayMicroseconds(6);
    digitalWrite(pin_clk, HIGH);
    delayMicroseconds(6);
    digitalWrite(pin_clk, LOW);
  }

  delayMicroseconds(6);
  digitalWrite(pin_clk, HIGH);
  delayMicroseconds(6);
  digitalWrite(pin_clk, LOW);

  return res;
}