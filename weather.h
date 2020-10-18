#ifndef WEATHER_INCLUDED
#define WEATHER_INCLUDED

typedef enum { WEATHER_SUNNY, WEATHER_CLOUDY, WEATHER_RAIN, WEATHER_STORM } Weather;

void drawWeather();
Weather getCurrentWeather();
Weather getCurrentForecast();
void newDayWeather();

#endif