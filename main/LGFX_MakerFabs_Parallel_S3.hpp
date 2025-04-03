// based on https://github.com/lovyan03/LovyanGFX/blob/experimental/src/lgfx_user/LGFX_ESP32S2_MakerabsParallelTFTwithTouch.hpp
// LGFX for Makerfabs ESP32-S2-Parallel-TFT-with-Touch
// https://github.com/Makerfabs/Makerfabs-ESP32-S2-Parallel-TFT-with-Touch/

// modified for the S3 version's pins as with https://github.com/Makerfabs/Makerfabs-ESP32-S3-Parallel-TFT-with-Touch/blob/main/firmware/SD16_3.5/SD16_3.5.ino
#pragma once

#define LGFX_USE_V1

#include <LovyanGFX.hpp>
#include <driver/i2c.h>


class LGFX : public lgfx::LGFX_Device
{
  static constexpr int I2C_PORT_NUM = I2C_NUM_0;
  static constexpr int I2C_PIN_SDA = 39;
  static constexpr int I2C_PIN_SCL = 40;
  static constexpr int I2C_PIN_INT = 38;

  lgfx::Bus_Parallel8 _bus_instance;
  lgfx::Panel_ILI9488 _panel_instance;
  lgfx::Light_PWM     _light_instance;
  lgfx::ITouch*  _touch_instance_ptr = nullptr;

  /// Detects and configures the touch panel during initialization;
  bool init_impl(bool use_reset, bool use_clear) override
  {
    if (_touch_instance_ptr == nullptr)
    {
      lgfx::ITouch::config_t cfg;
      lgfx::i2c::init(I2C_PORT_NUM, I2C_PIN_SDA, I2C_PIN_SCL);
      if (lgfx::i2c::beginTransaction(I2C_PORT_NUM, 0x38, 400000, false).has_value()
       && lgfx::i2c::endTransaction(I2C_PORT_NUM).has_value())
      {
        _touch_instance_ptr = new lgfx::Touch_FT5x06();
        cfg = _touch_instance_ptr->config();
        cfg.i2c_addr = 0x38;
        cfg.x_max = 320;
        cfg.y_max = 480;
      }
      else
      if (lgfx::i2c::beginTransaction(I2C_PORT_NUM, 0x48, 400000, false).has_value()
       && lgfx::i2c::endTransaction(I2C_PORT_NUM).has_value())
      {
        _touch_instance_ptr = new lgfx::Touch_NS2009();
        cfg = _touch_instance_ptr->config();
        cfg.i2c_addr = 0x48;
        cfg.x_min = 368;
        cfg.y_min = 212;
        cfg.x_max = 3800;
        cfg.y_max = 3800;
      }
      if (_touch_instance_ptr != nullptr)
      {
        cfg.i2c_port = I2C_PORT_NUM;
        cfg.pin_sda  = I2C_PIN_SDA;
        cfg.pin_scl  = I2C_PIN_SCL;
        cfg.pin_int  = I2C_PIN_INT;
        cfg.freq = 400000;
        cfg.bus_shared = false;
        _touch_instance_ptr->config(cfg);
        _panel_instance.touch(_touch_instance_ptr);
      }
    }
    return lgfx::LGFX_Device::init_impl(use_reset, use_clear);
  }

public:

  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();

      cfg.freq_write = 16000000;
      cfg.pin_wr = 9;
      cfg.pin_rd = -1;
      cfg.pin_rs = 46;

      cfg.pin_d0 = 10;
      cfg.pin_d1 = 11;
      cfg.pin_d2 = 12;
      cfg.pin_d3 = 13;
      cfg.pin_d4 = 14;
      cfg.pin_d5 = 21;
      cfg.pin_d6 = 47;
      cfg.pin_d7 = 48;
      //cfg.pin_d8 = 3;
      //cfg.pin_d9 = 8;
      //cfg.pin_d10 = 16;
      //cfg.pin_d11 = 15;
      //cfg.pin_d12 = 7;
      //cfg.pin_d13 = 6;
      //cfg.pin_d14 = 5;
      //cfg.pin_d15 = 4;
      _bus_instance.config(cfg);
      _panel_instance.bus(&_bus_instance);
    }

    {
      auto cfg = _panel_instance.config();

      cfg.pin_cs          =    3;
      cfg.pin_rst         =    -1;
      cfg.pin_busy        =    -1;
      cfg.offset_rotation =     0;

      cfg.panel_width = 320;                  // 屏幕宽度
      cfg.panel_height = 480;                 // 屏幕高度
      cfg.offset_x = 0;                       // X 偏移
      cfg.offset_y = 0;                       // Y 偏移
      cfg.dummy_read_pixel = 8;               // 像素读取时的 dummy cycle 数
      cfg.dummy_read_bits = 1;                // 读取时的 dummy bit 数
      cfg.readable = false;                    // 是否允许读取
      cfg.invert = false;                     // 是否反转显示
      cfg.rgb_order = false;                  // RGB/BGR 顺序
      cfg.dlen_16bit = false;                 // 使用 16-bit 数据长度
      cfg.bus_shared = false;                 // 总线是否共享

      _panel_instance.config(cfg);
    }

    {
      auto cfg = _light_instance.config();

      cfg.pin_bl = 45;
      cfg.invert = false;
      cfg.freq   = 44100;
      cfg.pwm_channel = 7;

      _light_instance.config(cfg);
      _panel_instance.light(&_light_instance);
    }
    setPanel(&_panel_instance);

    //setColorDepth(32); 
  }
};
