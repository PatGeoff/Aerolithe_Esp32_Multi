// #include <Arduino.h>
// #include "driver/ledc.h"

// // Constants
// const int ledPinA = 14; // GPIO pin connected to the Base of the TIP120 with a 100ohms resistance. //  The pins on Wroom-32 must be ADC1, so here pin 34 and 35 are used
// const int potPin = 35; // ADC-capable pin for the potentiometer
// const int ledChannel = 5; // Use PWM channel 0
// const int pwmFrequency = 20000; // 20 kHz frequency for smoother dimming
// const int pwmResolution = 8; // 8-bit resolution -- ne pas mettre 12 bit 

// void setupLED() {
//  // pinMode(potPin, INPUT);
// //  pinMode(ledPinA, OUTPUT);
//   // Configure the PWM timer
//   ledc_timer_config_t ledc_timer = {
//     .speed_mode       = LEDC_HIGH_SPEED_MODE,
//     .duty_resolution  = (ledc_timer_bit_t)pwmResolution,
//     .timer_num        = LEDC_TIMER_0,
//     .freq_hz          = pwmFrequency,
//     .clk_cfg          = LEDC_AUTO_CLK
//   };
//   ledc_timer_config(&ledc_timer);

//   // Configure the PWM channel
//   ledc_channel_config_t ledc_channel = {
//     .gpio_num       = ledPinA,
//     .speed_mode     = LEDC_HIGH_SPEED_MODE,
//     .channel        = (ledc_channel_t)ledChannel,
//     .intr_type      = LEDC_INTR_FADE_END,



//     .timer_sel      = LEDC_TIMER_0,
//     .duty           = 0,
//     .hpoint         = 0
//   };
//   ledc_channel_config(&ledc_channel);

//   Serial.println("LEDC setup completed.");
// }

// void lightsOn() {
//   static int lastDutyCycle = 0;
//   int potValue = analogRead(potPin); // Read the potentiometer value
//   int targetDutyCycle = map(potValue, 0, 4095, 0, 255); // Map to PWM duty cycle

//   // Gradually interpolate between the current and target duty cycle for smooth transition
//   if (lastDutyCycle != targetDutyCycle) {
//     int step = (targetDutyCycle > lastDutyCycle) ? 1 : -1;
//     for (int dutyCycle = lastDutyCycle; dutyCycle != targetDutyCycle; dutyCycle += step) {
//       ledc_set_duty(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)ledChannel, dutyCycle);
//       ledc_update_duty(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)ledChannel);
//       delay(2); // Smaller delay for smoother transition
//     }
//     lastDutyCycle = targetDutyCycle;
//   }

//   // Print the potentiometer and duty cycle values for debugging 
//   Serial.print("Potentiometer Value: ");
//   Serial.print(potValue);
//   Serial.print("  Duty Cycle: ");
//   Serial.println(targetDutyCycle);
  
//   delay(50); // Delay to avoid excessive updates
// }




