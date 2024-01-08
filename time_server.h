String timeserverloc="http://worldtimeapi.org/api/timezone/Asia/Taipei";

int lastchecktime=millis();
int lastchecksec=millis();
int timecheckduration=120; //second(s)

String datetime_f_t;
String datetime_f_d;

void gettime(bool oled_out,bool force);
void addsecond();
void oled_outp_datetime();

void gettime(bool oled_out,bool force=false){
  //Serial.println("gettime()");
    if((datetime_f_t!=""&&millis()-lastchecktime<timecheckduration*1000)&&!force){
      if(millis()-lastchecksec>=955){//caculating runtime delay, generally 1000 milliseconds
        addsecond();
        lastchecksec=millis();
        if(oled_out){
          oled_outp_datetime();
        }
        
      }  
      return;
    }
    lastchecktime=millis();
    
    WiFiClient client1;
    HTTPClient http;
    http.begin(client1,timeserverloc);
    int httpResponseCode = http.GET();

    if (httpResponseCode == HTTP_CODE_OK) {
        String payload = http.getString();
        //Serial.println("HTTP Response:");
        //Serial.println(payload);
        JSONVar res_arr=JSON.parse(payload);
        String timeraw=res_arr["datetime"];
        //Serial.println(timeraw);
    
        String timeraw_1[charnum(timeraw,'T')+1];
        fillarray(timeraw_1,timeraw,'T');
    
        String date_f=timeraw_1[0];
        String timeraw_2=timeraw_1[1];
        String time_f=trimstr(timeraw_2,0,8);
        String zone=res_arr["abbreviation"];

        datetime_f_t=time_f;

        datetime_f_d=date_f+" "+zone;

        if(oled_out){
          oled_outp_datetime();
        }
    }
    else {
        //Serial.print("HTTP request failed with error code: ");
        //Serial.println(httpResponseCode);
    }
}

void addsecond(){
    int hr=trimstr(datetime_f_t,0,2).toInt();
    int minu=trimstr(datetime_f_t,3,5).toInt();
    int sec=trimstr(datetime_f_t,6,8).toInt();
    
    sec++;
    if(sec>=60){
      sec-=60;
      minu++;
    }
    if(minu>=60){
      minu-=60;
      hr++;
    }
    if(hr>=24){
      gettime(true,true);
      return;
    }

    String hr_s=String(hr);
    if(strlen(hr_s.c_str())==1){
      hr_s="0"+hr_s;;
    }
    String minu_s=String(minu);
    if(strlen(minu_s.c_str())==1){
      minu_s="0"+minu_s;
    }
    String sec_s=String(sec);
    if(strlen(sec_s.c_str())==1){
      sec_s="0"+sec_s;
    }
    datetime_f_t=hr_s+":"+minu_s+":"+sec_s;
}

void oled_outp_datetime(){
  oled1.del_data();
  int screenWidth = oled1.getWidth();
  int screenHeight = oled1.getHeight();

  int width_t=oled1.getUTF8Width(datetime_f_t.c_str());
  int x_t=(screenWidth-width_t)/2;
  int y_t=18;
  oled1.add_data(datetime_f_t,u8g2_font_logisoso18_tr,x_t,y_t);

  int width_d=oled1.getUTF8Width(datetime_f_d.c_str());
  int x_d=(screenWidth-width_d)/2;
  int y_d=screenHeight;
  oled1.add_data(datetime_f_d,u8g2_font_7x14B_tr,x_d,y_d);
}
