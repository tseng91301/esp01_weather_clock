//I2C setup
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define SDA D1
#define SCL D2

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);     // 0.91吋長型螢幕使用
int imgWidth=128;
int imgHeight=32;

void outputoled_pic(const unsigned char input_pic[],int start_x=0,int start_y=0,int width=imgWidth,int height=imgHeight){
    u8g2.clearBuffer();                    // clear the internal memory
    u8g2.drawXBMP(start_x,start_y, width, height, input_pic);    //繪圖
    u8g2.sendBuffer();                    // transfer internal memory to the display
}
void outputoled_txt(String wordin,int start_x=0,int start_y=10){
    String wordin_arr[charnum(wordin,'\n')+1];
    fillarray(wordin_arr,wordin,'\n');

    u8g2.clearBuffer();                    // clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr);  // choose a suitable font
    int arrlen=sizeof(wordin_arr)/sizeof(wordin_arr[0]);
    for(int a=0;a<arrlen;a++){
        //Serial.println("u8g2.drawStr");
        //Serial.println(wordin_arr[a].c_str());
        u8g2.drawStr(start_x,start_y,wordin_arr[a].c_str());
        start_y+=11;
    }
    u8g2.sendBuffer();                    // transfer internal memory to the display
}
