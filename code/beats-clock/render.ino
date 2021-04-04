/*
 * This file is part of COVID Calendar.
 *
 * COVID Calendar is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * COVID Calendar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with COVID Calendar. If not, see <https://www.gnu.org/licenses/>.
 */
 
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include <Time.h>
#include <TimeLib.h>

#include "config_struct.h"

Adafruit_AlphaNum4 alpha0 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 alpha1 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 alpha2 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 alpha3 = Adafruit_AlphaNum4();

extern calConfig_t calendarConfig;

void render_setup()
{
  alpha0.begin(0x70);  // pass in the address
  alpha1.begin(0x71);  // pass in the address
  alpha2.begin(0x72);  // pass in the address
  alpha3.begin(0x73);  // pass in the address
}

void render_time(time_t timestamp, uint8_t bright) {
  static time_t last_beat_timestamp;
  static uint32_t last_beat_millis;
  uint32_t beats, centibeats;
  struct tm *day_info;
  char line1[10], line2[11];
  char front_spacer[3];
  char end_spacer[3];
  uint8_t i, j;
  char c;
  bool dot;

  bright /= 16;

  if (bright == 0)
  {
    alpha0.clear();
    alpha1.clear();
    alpha2.clear();
    alpha3.clear();

    alpha0.writeDisplay();
    alpha1.writeDisplay();
    alpha2.writeDisplay();
    alpha3.writeDisplay();
    
    return;
  }

  alpha0.setBrightness(bright);
  alpha1.setBrightness(bright);
  alpha2.setBrightness(bright);
  alpha3.setBrightness(bright);

  if (timestamp != last_beat_timestamp)
  {
    last_beat_timestamp = timestamp;
    last_beat_millis = millis(); 
  }
  
  /* Add an hour - Swatch Internet Time is GMT+1 */
  timestamp += 60 * 60;
  day_info = gmtime(&timestamp);
  
  /* get seconds since midnight */
  timestamp %= 86400;
  
  /* convert to milliseconds, and add time since clock changed */
  timestamp *= 1000;
  timestamp += (millis() - last_beat_millis);
  
  /* 86.4 seconds per beat - divide milliseconds by 864 to get centibeats */
  centibeats = timestamp / 864;

  beats = centibeats / 100;
  centibeats %= 100;
  if (beats > 999)
  {
    /* 4 digits for beats - no front indent, 1 space end indent */
    /* @1000.00_ - decimal doesn't count */
    front_spacer[0] = 0;
    end_spacer[0] = ' ';
    end_spacer[1] = 0;
  }
  else if (beats > 99)
  {
    /* 3 digits for beats - 1 space indent in front and back */
    /* _@999.00_ - decimal doesn't count */
    front_spacer[0] = ' ';
    front_spacer[1] = 0;
    end_spacer[0] = ' ';
    end_spacer[1] = 0;
  }
  else if (beats > 9)
  {
    /* 2 digits for beats - 1 space front, 2 spaces back */
    /* _@99.00__ - decimal doesn't count */
    front_spacer[0] = ' ';
    front_spacer[1] = 0;
    end_spacer[0] = ' ';
    end_spacer[1] = ' ';
    end_spacer[2] = 0;
  }
  else 
  {
    /* 1 digits for beats - 2 space indent, front and back */
    /* __@9.00__ - decimal doesn't count */
    front_spacer[0] = ' ';
    front_spacer[1] = ' ';
    front_spacer[2] = 0;
    end_spacer[0] = ' ';
    end_spacer[1] = ' ';
    end_spacer[2] = 0;
  }
  
  snprintf(line1, 10, "%s@%d.%02d%s", front_spacer, beats, centibeats, end_spacer);
  snprintf(line2, 11, "%02d%02d%04d", day_info->tm_mday, day_info->tm_mon + 1, day_info->tm_year + 1900);

  for (i=0, j=0; i < strlen(line1); i++, j++)
  {
    c = line1[i];
    dot = false;
    if (i +1 < strlen(line1))
    {
      if (line1[i+1] == '.')
      {
        i += 1;
        dot = true;
      }
    }
    if (j < 4)
    {
      alpha0.writeDigitAscii(j, c, dot);
    }
    else
    {
      alpha1.writeDigitAscii(j-4, c, dot);
    }
  }
  
  alpha2.writeDigitAscii(0, line2[0], false);
  alpha2.writeDigitAscii(1, line2[1], true);
  alpha2.writeDigitAscii(2, line2[2], false);
  alpha2.writeDigitAscii(3, line2[3], true);
  alpha3.writeDigitAscii(0, line2[4], false);
  alpha3.writeDigitAscii(1, line2[5], false);
  alpha3.writeDigitAscii(2, line2[6], false);
  alpha3.writeDigitAscii(3, line2[7], false);
  
  alpha0.writeDisplay();
  alpha1.writeDisplay();
  alpha2.writeDisplay();
  alpha3.writeDisplay();
}
