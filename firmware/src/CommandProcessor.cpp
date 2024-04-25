#include <Arduino.h>
#include "CommandProcessor.h"
#include <FastLED.h>


#define NUM_LEDS 256

CRGB leds[NUM_LEDS];
int left[] = {
  4,5,6,7,8,9,10,11,
  20,21,22,23,24,25,26,27,
  36,37,38,39,40,41,42,43,
  52,53,54,55,56,57,58,59,
  68,69,70,71,72,73,74,75,
  84,85,86,87,88,89,90,91,
  100,101,102,103,104,105,106,107,
  116,117,118,119,120,121,122,123,
  132,133,134,135,136,137,138,139,
  145,146,147,148,149,150,151,152,153,154,155,156,157,158,
  162,163,164,165,166,167,168,169,170,171,172,173,
  179,180,181,182,183,184,185,186,187,188,
  196,197,198,199,200,201,202,203,
  213,214,215,216,217,218,
  230,231,232,233,
  247,248
};

int right[] = {
  244, 235, 212, 203, 180, 171, 148, 139, 116,
  245, 234, 213, 202, 181, 170, 149, 138, 117,
  246, 233, 214, 201, 182, 169, 150, 137, 118,
  247, 232, 215, 200, 183, 168, 151, 136, 119,
  248, 231, 216, 199, 184, 167, 152, 135, 120,
  249, 230, 217, 198, 185, 166, 153, 134, 121,
  250, 229, 218, 197, 186, 165, 154, 133, 122,
  251, 228, 219, 196, 187, 164, 155, 132, 123,
  110, 109, 108, 107, 106, 105, 104, 103, 102, 
  101, 100, 99, 98, 97, 82, 83, 84, 85, 86, 87, 
  88, 89, 90, 91, 92, 93, 76, 75, 74, 73, 72, 71, 
  70, 69, 68, 67, 52, 53, 54, 55, 56, 57, 58, 59, 
  42, 41, 40, 39, 38, 37, 22, 23, 24, 25, 8, 7
};



int stop[] = {
  244,245,246,247,248,249,250,251,236,235,234,233,232,
  231,230,229,228,227,210,211,212,213,214,215,216,217,
  218,219,220,221,206,205,204,203,202,201,200,199,198,
  197,196,195,194,193,176,175,144,143,112,111,80,79,177,
  174,145,142,113,110,81,78,178,173,146,141,114,109,82,77
  ,179,172,147,140,115,108,83,76,180,171,148,139,116,107,
  84,75,181,170,149,138,117,106,85,74,182,169,150,137,118,
  105,86,73,183,168,151,136,119,104,87,72,184,167,152,135,
  120,103,88,71,185,166,153,134,121,102,89,70,186,165,154,
  133,122,101,90,69,187,164,155,132,123,100,91,68,188,163,
  156,131,124,99,92,67,189,162,157,130,125,98,93,66,190,161,
  158,129,126,97,94,65,191,160,159,128,127,96,95,64,49,50,51,
  52,53,54,55,56,57,58,59,60,61,62,45,44,43,42,41,40,39,38,37,
  36,35,34,19,20,21,22,23,24,25,26,27,28,11,10,9,8,7,6,5,4
};


int slow[] = {
25,37,38,57,58,59,67,68,69,70,71,72,73,74,75,
76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,
91,92,93,97,98,99,100,101,102,103,104,105,106,
107,108,109,110,111,112,113,114,115,116,117,118,
119,120,121,122,123,124,125,126,127,128,129,130,
131,132,133,134,135,136,137,138,139,140,141,142,143,
144,145,146,147,148,149,150,151,152,153,154,155,156,
157,158,162,163,164,165,166,167,168,169,170,171,172,
173,174,175,176,177,178,179,180,181,182,183,184,185,
186,187,188,196,197,198,217,218,230,
};

bool patternActive = false;
int currentPattern = 0;
int blinkCount = 0;
const int maxBlinks = 10; // Maximum number of blinks before turning off

bool isInArray(int val, int arr[], int size) {
  for (int i = 0; i < size; i++) {
    if (arr[i] == val) {
      return true;
    }
  }
  return false;
}

const char *words[] = {
    "slow",
    "left",
    "right",
    "stop",
    "_invalid",
};

void commandQueueProcessorTask(void *param)
{
    CommandProcessor *commandProcessor = (CommandProcessor *)param;
    while (true)
    {
        uint16_t commandIndex = 0;
        if (xQueueReceive(commandProcessor->m_command_queue_handle, &commandIndex, portMAX_DELAY) == pdTRUE)
        {
            commandProcessor->processCommand(commandIndex);
        }
    }
}

void displayPattern(int patternIndex) {
  switch (patternIndex) {
    case 0:
      FastLED.setBrightness(10);
      for (int i = 0; i < NUM_LEDS; i++) {
        if (isInArray(i, slow, sizeof(slow) / sizeof(slow[0]))) {
          leds[i] = CRGB::Gray;
        } else {
          leds[i] = CRGB::Black;
        }
      }
      break;
    case 1:
      for (int i = 0; i < NUM_LEDS; i++) {
        if (isInArray(i, left, sizeof(left) / sizeof(left[0]))) {
          leds[i] = CRGB::Yellow;
        } else {
          leds[i] = CRGB::Black;
        }
      }
      break;
    case 2:
      for (int i = 0; i < NUM_LEDS; i++) {
        if (isInArray(i, right, sizeof(right) / sizeof(right[0]))) {
          leds[i] = CRGB::Yellow;
        } else {
          leds[i] = CRGB::Black;
        }
      }
      break; // Exit the switch statement after processing case 2

      case 3:
        FastLED.setBrightness(10);
        for (int i = 0; i < NUM_LEDS; i++) {
            if (isInArray(i, stop, sizeof(stop) / sizeof(stop[0]))) {
            leds[i] = CRGB::Green;
            } else {
            leds[i] = CRGB::Black;
            }
        }
      break; // Exit the switch statement after processing case 3
  }
  FastLED.show(); // Show LEDs after processing each case
}

void CommandProcessor::processCommand(uint16_t commandIndex)
{ 
    switch(commandIndex) {
    case 0:
        patternActive = true;
        currentPattern = 0;
        blinkCount = 0; // Reset blink count when pattern starts
        break;
    case 1:
        patternActive = true;
        currentPattern = 1;
        blinkCount = 0; // Reset blink count when pattern starts
        break;
    case 2:
        patternActive = true;
        currentPattern = 2;
        blinkCount = 0; // Reset blink count when pattern starts
        break;
    case 3:
        patternActive = true;
        currentPattern = 3;
        FastLED.clear(); // Clear LEDs immediately for case 2
        displayPattern(currentPattern); // Display LEDs without delay
        patternActive = false; // Turn off pattern immediately for case 2
        break;
    default:
        patternActive = false;
        FastLED.clear();
        FastLED.show();
        break;
}
    if (patternActive && currentPattern != 3) { // Skip blinking for case 2
    displayPattern(currentPattern);
    delay(500); // Delay for pattern visibility
    FastLED.clear(); // Turn off pattern
    FastLED.show();
    delay(500); // Delay before next blink
    blinkCount++; // Increment blink count
    if (blinkCount >= maxBlinks) {
      patternActive = false; // Turn off pattern after max blinks
      FastLED.clear();
      FastLED.show();
    }
  }
}
CommandProcessor::CommandProcessor()
{
    FastLED.addLeds<WS2812B, GPIO_NUM_9, RGB>(leds, NUM_LEDS);
    FastLED.setBrightness(3); // Adjust brightness as needed
    FastLED.clear();
    
    // allow up to 5 commands to be in flight at once
    m_command_queue_handle = xQueueCreate(5, sizeof(uint16_t));
    if (!m_command_queue_handle)
    {
        Serial.println("Failed to create command queue");
    }
    // kick off the command processor task
    TaskHandle_t command_queue_task_handle;
    xTaskCreate(commandQueueProcessorTask, "Command Queue Processor", 2048, this, 1, &command_queue_task_handle);
}

void CommandProcessor::queueCommand(uint16_t commandIndex, float best_score)
{
    // unsigned long now = millis();
    if (commandIndex != 5 && commandIndex != -1)
    {
        Serial.printf("* %ld Detected command %s(%f)\n", millis(), words[commandIndex], best_score);
        if (xQueueSendToBack(m_command_queue_handle, &commandIndex, 0) != pdTRUE)
        {
            Serial.println("No more space for command");
        }
    }
}