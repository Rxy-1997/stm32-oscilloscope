# 基于 STM32F103 的数字示波器

STM32F103 通过 ADC + DMA 连续采集模拟信号，在 0.96 寸 OLED 上实时绘制波形，用旋转编码器调节采样率与纵向缩放。

![platform](https://img.shields.io/badge/MCU-STM32F103C8T6-blue)
![library](https://img.shields.io/badge/library-SPL%203.5.0-green)
![ide](https://img.shields.io/badge/IDE-Keil%20MDK%205-orange)

## 功能特性

- 单通道模拟信号采集，输入范围 0–3.3 V，12 位分辨率
- 每屏 128 个采样点，逐点增量刷新
- 实时计算并显示峰峰值 Vpp，单位 V
- 采样率可在约 2 kHz – 500 kHz 之间调节，默认 100 kHz
- 双工作模式：
  - **X 模式**：旋转编码器调节采样率
  - **Y 模式**：旋转编码器调节纵向缩放（1–5 倍）
- 编码器按键切换模式
- 输出 1 kHz PWM 方波，可用于自测与探头校准

## 硬件组成

| 模块 | 型号 / 接口 | 引脚 | 说明 |
|---|---|---|---|
| 主控 | STM32F103C8T6 | — | Cortex-M3，72 MHz，64 KB Flash |
| 显示 | SSD1306 OLED 128×64 | PB8 / PB9 | I2C，软件模拟时序 |
| 信号输入 | ADC1_IN0 | PA0 | 0–3.3 V，12 位 |
| 旋转编码器 | TIM4 编码器模式 | PB6 / PB7 | 正交解码，带按键 |
| 编码器按键 | EXTI2 | PB2 | 下降沿中断 |
| PWM 输出 | TIM3_CH1 | PA6 | 1 kHz 方波 |
| 预留 LED | GPIO 推挽输出 | PA1 / PA2 | 驱动已实现，主流程未启用 |
| 预留按键 | GPIO 上拉输入 | PB1 / PB11 | 驱动已实现，主流程未启用 |

## 软件结构

```
oscilloscope/
├── Hardware/            外设驱动
│   ├── ad.c/h           ADC + DMA + 定时器触发采样
│   ├── oled.c/h         SSD1306 驱动与显存
│   ├── oled_font.h      8×16 ASCII 字模
│   ├── display.c/h      波形绘制、Vpp 计算
│   ├── encoder.c/h      旋转编码器与按键中断
│   ├── pwm_out.c/h      PWM 方波输出
│   ├── key.c/h          独立按键（预留）
│   └── led.c/h          LED 控制（预留）
├── System/
│   └── delay.c/h        SysTick 微秒/毫秒延时
├── User/
│   ├── main.c           主流程与状态机
│   ├── stm32f10x_it.c/h 中断向量入口
│   └── stm32f10x_conf.h 标准库配置
├── Library/             ST 标准外设库 3.5.0
├── Start/               CMSIS 内核与启动文件
├── Objects/             编译输出（不入库）
└── Project.uvprojx      Keil MDK 工程
```

## 关键实现

**采样链路**

TIM2 工作在 PWM 模式，用 CC2 事件按设定频率触发 ADC1 单次转换；ADC 结果由 DMA1 通道 1 循环搬运到 `ad_data[128]`。整条链路由硬件驱动，CPU 不参与搬运，只负责读取结果。

采样率由 TIM2 的自动重装值决定：计数器时钟 1 MHz，周期值 `arr` 在 2–500 之间调整，对应采样率 1 MHz / arr。

**波形绘制**

`oled_gram[8][128]` 作为显存，每帧只写入变化的像素：

1. 把 ADC 值按比例映射到 48 像素高的波形区（y = 16–63）
2. 与上一帧同一列的位置比较，位置变化时擦除旧点
3. 新旧位置之间用竖线补齐，避免信号快速变化时出现断线
4. 整帧结束后一次性刷屏，减少 I2C 传输次数

**Vpp 计算**

遍历 128 点求最大值与最小值，按 `(max - min) × 3.3 / 4095` 换算成电压，拆成整数与小数部分分别显示。

## 编译与烧录

**Keil MDK 5**

```
打开 Project.uvprojx → Build (F7) → Download (F8)
```

编译器为 ARMCC V5.06，需安装 STM32F1xx Device Family Pack。

**VS Code（可选）**

安装 Keil Assistant 插件，在设置中指定 `UV4.exe` 路径：

```json
"KeilAssistant.MDK.Uv4Path": "C:\\Keil_v5\\UV4\\UV4.exe"
```

之后可在侧边栏直接编译、下载，输出与 Keil 一致。

## 使用说明

1. 上电后 OLED 左上角显示 `STM32 Oscilloscope`，右上角显示实时 Vpp
2. 第二行显示当前模式：`Mode: X` 或 `Mode: Y`
3. 旋转编码器：
   - X 模式下改变采样率（`ARR` 值 2–500）
   - Y 模式下改变纵向缩放（1–5 倍）
4. 按下编码器按键切换模式

## 已知问题与改进方向

- 无触发功能，波形自由滚动，无法稳定显示周期信号
- 单通道，不支持双踪与通道间运算
- 未实现频率测量、FFT、自动量程

## 许可说明

`Library/` 与 `Start/` 为 ST 官方标准外设库与 CMSIS 文件，遵循 ST 原有许可协议；其余代码可自由参考使用。
