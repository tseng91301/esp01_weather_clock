//I2C setup
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


class Oled:public U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C{
    private:
        int addr;
        int scl_pin;
        int sda_pin;
        int width;
        int height;
        
        int scroll_time=20;
        

        String all_text="";

        int text_num=0;
        struct _text_inf{
            String text;
            uint8_t *type;
            int nowtime=millis();
            int start_x=0;
            int xlen=0;
            int xtmp;
            int fix=0;
            int start_y=10;
        };
        _text_inf *text_inf=new _text_inf[0];
    public:
        Oled(const int sda_p=2,const int scl_p=0,int w=128,int h=32):
            U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ scl_p, /* data=*/ sda_p)     // 0.91吋長型螢幕使用
        {
            width=w;
            height=h;
            sda_pin=sda_p;
            scl_pin=scl_p;
            begin();
        }
        void data_init(){
            text_num=0;
            text_inf=new _text_inf[0];
        }
        void del_data(){
            delete[] text_inf;
            data_init();
        }
        void show(int delaytime=1){
            int tn=millis();
            while(millis()-tn<delaytime){
                clearBuffer();
                for(int a=0;a<text_num;a++){
                    setFont(text_inf[a].type);
                    if(text_inf[a].xlen==0){
                        text_inf[a].xlen=getStrWidth(text_inf[a].text.c_str());
                    }
                    if(millis()-text_inf[a].nowtime>scroll_time){
                        text_inf[a].nowtime=millis();
                        if(text_inf[a].xlen>width){
                            if(text_inf[a].xlen+text_inf[a].xtmp-(width-10)<=0){
                                text_inf[a].xtmp=0;
                            }else{
                                text_inf[a].xtmp-=1;
                            }
                        }
                    }
                    if(text_inf[a].fix){
                        drawStr(text_inf[a].start_x,text_inf[a].start_y,text_inf[a].text.c_str());
                    }else{
                        drawStr(text_inf[a].xtmp,text_inf[a].start_y,text_inf[a].text.c_str());
                    }
                    delay(1);
                }
                sendBuffer();
            }
        }
        int get_word_width(String t,const uint8_t *ty){
            setFont(ty);
            int outp=getUTF8Width(t.c_str());
            return(outp);
        }
        void pic(const unsigned char input_pic[],int start_x=0,int start_y=0){
            clearBuffer();                    // clear the internal memory
            drawXBMP(start_x,start_y, width, height, input_pic);    //繪圖
            sendBuffer();                    // transfer internal memory to the display
        }
        void add_data(String t,const uint8_t *ty,int start_x,int start_y,int fix=0){
            _text_inf *tmp=new _text_inf[text_num+1];
            for(int a=0;a<text_num;a++){
                tmp[a]=text_inf[a];
            }
            tmp[text_num].text=t;
            tmp[text_num].start_x=start_x;
            //tmp[text_num].xlen=t.length()*6;
            tmp[text_num].xtmp=0;
            tmp[text_num].start_y=start_y;
            tmp[text_num].type=const_cast<uint8_t*>(ty);
            tmp[text_num].fix=fix;
            text_num+=1;
            delete[] text_inf;
            text_inf=new _text_inf[text_num];
            for(int a=0;a<text_num;a++){
                text_inf[a]=tmp[a];
            }
            delete[] tmp;
        }
        void txt(String wordin,int start_x=0,int start_y=10,int fix=0){
            if(all_text==wordin){
                return;
            }
            all_text=wordin;
            del_data();
            String wordin_arr[charnum(wordin,'\n')+1];
            fillarray(wordin_arr,wordin,'\n');
            int arrlen=sizeof(wordin_arr)/sizeof(wordin_arr[0]);
            for(int a=0;a<arrlen;a++){
                add_data(wordin_arr[a],u8g2_font_ncenB08_tr,start_x,start_y,fix);
                start_y+=11;
            }
        }
        ~Oled(){
            delete[] text_inf;
        }
};