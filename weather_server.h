String weatherserverloc="http://192.168.0.52:5625/dealweatherserver.php?hr=";

int lastcheckweather=millis();
int lastdumpweather=millis();
int checkweatherduration=600; //second(s)

String weather_des=""; 
String weather_temp_c="";
String weather_humidity="";
String weather_chance_of_rain="";
String weather_location="";

void getweather(){
  //Serial.println("getweather()");
  String current_d=trimstr(datetime_f_d,0,10);
  int current_t=(trimstr(datetime_f_t,0,2)).toInt();
  WiFiClient client1;
  HTTPClient http;
  String weatherserverloc2=weatherserverloc+String(current_t);
  //Serial.println(weatherserverloc2);
  http.begin(client1,weatherserverloc2);
  int httpResponseCode = http.GET();
  if (httpResponseCode == HTTP_CODE_OK) {
    String payload = http.getString();
    //Serial.println("HTTP Response:");
    //Serial.println(payload);
    JSONVar res_arr=JSON.parse(payload);

    weather_des=String(res_arr["weather_des"]);
    weather_temp_c=String(res_arr["weather_temp_c"]);
    weather_humidity=String(res_arr["weather_humidity"]);
    weather_chance_of_rain=String(res_arr["weather_chance_of_rain"]);
    weather_location=String(res_arr["weather_location"]);
  }
  gettime(false,true);
}

void oled_outp_weather_daybig(){
  //Serial.println("oled_outp_weather_daybig()");
  if(millis()-lastcheckweather>=checkweatherduration*1000||weather_des==""){
    getweather();
  }
  if(millis()-lastdumpweather<=switch_mode_sec*1000){
    return;
  }
  lastdumpweather=millis();
  oled1.del_data();

  oled1.add_data(weather_des,u8g2_font_6x12_tf,0,12);

  String t_h_r="T:"+weather_temp_c+"dC H:"+weather_humidity+"% R:"+weather_chance_of_rain+"%";
  oled1.add_data(t_h_r,u8g2_font_6x12_tf,0,23);

  oled1.add_data(weather_location,u8g2_font_6x12_tf,0,32);
}
