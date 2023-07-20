#ifndef   MBED_STEERING_HPP
#define   MBED_STEERING_HPP
#include <cmath>
#include <mbed.h>
#include <vector>

#define pi 3.1415

class Tire{
private:
    DigitalOut p1_;
    DigitalOut p2_;
    PwmOut speed_;
    PwmOut servo_;
    float offset_;
    void sietaToPwmOut(float θ){
        //0~180を500~2500に変換 todo:データシートに合わせる
        float degree = 180 * θ / pi;
        float pulse_width_us = (degree - 0) * (2500 - 500) / (180 - 0) + 500;
        printf(" pulse %f \t", pulse_width_us);
        // servo_.pulsewidth_us(pulse_width_us); //真ん中1500usだっけか？
    }
    void sppedToPwmOut(float speed){
        //モタドラによりけり todo:データシートに合わせる
        //todo 速度を正規化する
        //todo speedを絶対値にする
        printf("speed %f  \t\n", speed);
        // if(speed>0){
        //     // p1_ = true;
        //     // p2_ = false;
        //     // speed_ = speed;
        // }else if(speed<0){
        //     // p1_ = false;
        //     // p2_ = true;
        //     // speed_ = -speed;
        // }else{ //ストール帽子
        //     // p1_ = false;
        //     // p2_ = false;
        //     // speed_ = 0.0;
        // }
    }
public:
    Tire(PinName p1, PinName p2, PinName speed, PinName servo, float offset): p1_(p1), p2_(p2), speed_(speed), servo_(servo){
        offset_ = offset;
        speed_ = 0.0;
        servo_.period_us(20000);
        servo_.pulsewidth_us(1500); //真ん中1500usだっけか？
    }

    void run(float Vx, float Vy, float aw, float θ_v){
        float θ = θ_v + offset_;
        float vx = Vx + aw * -sin(θ);
        float vy = Vy + aw * cos(θ);
        float vnorm = sqrtf(vx*vx + vy*vy);
        float θ_x = θ;
        if(θ_x > 0){
            vnorm *= 1;
        }else if(θ_x > pi){
            vnorm *= -1;
        }
        printf("tire: speed = %f, sieta = %f", vnorm, θ_x*180/pi);
        sietaToPwmOut(θ_x);
        sppedToPwmOut(vnorm);
    }
};


class Steering{
private:
    vector<Tire> tires;
    float a = 0.5;
public:
    Steering(const Tire& p1,const Tire& p2, const Tire& p3){
        tires.push_back(p1);
        tires.push_back(p2);
        tires.push_back(p3);
    }

    void run(float vx, float vy, float w){ //指令を受け取ったら一つ一つのタイヤに対して命令を送る。
        printf("start ");
        float θ_v = atan2f(vx, vy); //共通項は計算しておく。
        for(auto i = tires.begin(); i != tires.end() ; i++){
            printf("tire ");
            i->run(vx, vy, a*w, θ_v); //各タイヤに変数を注入する。
        }
    }
};
#endif // MBED_STEERING_HPP