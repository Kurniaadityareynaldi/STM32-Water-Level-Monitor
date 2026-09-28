/* USER CODE BEGIN Header */
/**
 * @file    main.c
 * @brief   Main application for ADC level monitoring and OLED display.
 *
 * @note
 * This file is generated/managed by STM32CubeMX. User application code
 * should remain inside the USER CODE sections where applicable.
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ssd1306.h"
#include "fonts.h"

#include <stdio.h>

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

/* Private defines -----------------------------------------------------------*/

/* ADC level calibration */
#define ADC_MIN_VALUE           0U
#define ADC_MAX_VALUE           250U
#define LEVEL_CORRECTION_FACTOR 0.473f

/* OLED display */
#define OLED_TEXT_BUFFER_SIZE   20U

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_ADC1_Init(void);

static void OLED_Display(uint16_t adc_value, float level_cm,
                         const char *percentage);
static void Update_Level_Display(uint16_t adc_value);

/* Private variables ---------------------------------------------------------*/
static uint16_t adc_dma_value = 0U;
static uint16_t adc_value = 0U;

static float level_cm = 0.0f;

static char percentage_text[OLED_TEXT_BUFFER_SIZE];
static char adc_text[OLED_TEXT_BUFFER_SIZE];
static char level_text[OLED_TEXT_BUFFER_SIZE];

/* -------------------------------------------------------------------------- */
/* LED helper functions                                                       */
/* -------------------------------------------------------------------------- */

static void LED_1_2_OFF(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
}

static void LED_1_ON_2_OFF(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
}

static void LED_1_OFF_2_ON(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
}

static void LED_1_ON_2_ON(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
}

/* -------------------------------------------------------------------------- */
/* Application functions                                                      */
/* -------------------------------------------------------------------------- */

/**
 * @brief Convert the ADC reading into a percentage and level in centimeters.
 *
 * ADC_MIN_VALUE corresponds to 0% and ADC_MAX_VALUE corresponds to 100%.
 * The correction factor is applied only to the calculated physical level.
 */
static void Update_Level_Display(uint16_t adc)
{
    float percentage_value = 0.0f;

    if (adc <= ADC_MIN_VALUE)
    {
        percentage_value = 0.0f;
    }
    else if (adc >= ADC_MAX_VALUE)
    {
        percentage_value = 100.0f;
    }
    else
    {
        percentage_value =
            ((float)(ADC_MAX_VALUE - adc) /
             (float)(ADC_MAX_VALUE - ADC_MIN_VALUE)) * 100.0f;
    }

    level_cm = percentage_value * LEVEL_CORRECTION_FACTOR;

    /*
     * Display level in 25% steps:
     *   < 25%  -> calculated percentage
     *   25-49% -> 25%
     *   50-74% -> 50%
     *   75-99% -> 75%
     *   >= 100% -> 100%
     */
    if (percentage_value >= 100.0f)
    {
        snprintf(percentage_text, sizeof(percentage_text), "100%%");
    }
    else if (percentage_value >= 75.0f)
    {
        snprintf(percentage_text, sizeof(percentage_text), "75%%");
    }
    else if (percentage_value >= 50.0f)
    {
        snprintf(percentage_text, sizeof(percentage_text), "50%%");
    }
    else if (percentage_value >= 25.0f)
    {
        snprintf(percentage_text, sizeof(percentage_text), "25%%");
    }
    else
    {
        snprintf(percentage_text, sizeof(percentage_text),
                 "%.2f%%", percentage_value);
    }
}

/**
 * @brief Update all OLED fields.
 */
static void OLED_Display(uint16_t adc, float level,
                         const char *percentage)
{
    snprintf(adc_text, sizeof(adc_text), "%u", adc);
    snprintf(level_text, sizeof(level_text), "%.2f cm", level);

    SSD1306_GotoXY(0, 0);
    SSD1306_Puts("Kondisi:", &Font_7x10, SSD1306_COLOR_WHITE);

    SSD1306_GotoXY(60, 0);
    SSD1306_Puts("       ", &Font_7x10, SSD1306_COLOR_WHITE);

    SSD1306_GotoXY(60, 0);
    SSD1306_Puts(percentage, &Font_7x10, SSD1306_COLOR_WHITE);

    SSD1306_GotoXY(0, 20);
    SSD1306_Puts("Nilai ADC:", &Font_7x10, SSD1306_COLOR_WHITE);

    SSD1306_GotoXY(75, 20);
    SSD1306_Puts("       ", &Font_7x10, SSD1306_COLOR_WHITE);

    SSD1306_GotoXY(75, 20);
    SSD1306_Puts(adc_text, &Font_7x10, SSD1306_COLOR_WHITE);

    SSD1306_GotoXY(0, 40);
    SSD1306_Puts("Ketinggian:", &Font_7x10, SSD1306_COLOR_WHITE);

    SSD1306_GotoXY(75, 40);
    SSD1306_Puts("       ", &Font_7x10, SSD1306_COLOR_WHITE);

    SSD1306_GotoXY(75, 40);
    SSD1306_Puts(level_text, &Font_7x10, SSD1306_COLOR_WHITE);

    SSD1306_UpdateScreen();
}

/* -------------------------------------------------------------------------- */
/* Main application                                                           */
/* -------------------------------------------------------------------------- */

/**
 * @brief Application entry point.
 * @retval int
 */
int main(void)
{
    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();
    MX_DMA_Init();
    MX_I2C1_Init();
    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_ADC1_Init();

    SSD1306_Init();

    /*
     * TIM2/TIM3 are retained from the original project configuration.
     * No active timer callback is currently used in the application logic.
     */
    HAL_TIM_Base_Start_IT(&htim2);
    HAL_TIM_Base_Start_IT(&htim3);

    /* Start ADC1 in DMA continuous-conversion mode. */
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc_dma_value, 1U);

    while (1)
    {
        adc_value = adc_dma_value;

        Update_Level_Display(adc_value);
        OLED_Display(adc_value, level_cm, percentage_text);

        HAL_Delay(100U);
    }
}

/* -------------------------------------------------------------------------- */
/* System Clock Configuration                                                 */
/* -------------------------------------------------------------------------- */

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}

/* -------------------------------------------------------------------------- */
/* ADC1 Initialization                                                        */
/* -------------------------------------------------------------------------- */

static void MX_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;

    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }

    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

/* -------------------------------------------------------------------------- */
/* I2C1 Initialization                                                        */
/* -------------------------------------------------------------------------- */

static void MX_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 400000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
}

/* -------------------------------------------------------------------------- */
/* TIM2 Initialization                                                        */
/* -------------------------------------------------------------------------- */

static void MX_TIM2_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 7200U - 1U;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 1000U;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

/* -------------------------------------------------------------------------- */
/* TIM3 Initialization                                                        */
/* -------------------------------------------------------------------------- */

static void MX_TIM3_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 7200U - 1U;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 3000U;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

    if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

    if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

/* -------------------------------------------------------------------------- */
/* DMA Initialization                                                         */
/* -------------------------------------------------------------------------- */

static void MX_DMA_Init(void)
{
    __HAL_RCC_DMA1_CLK_ENABLE();
}

/* -------------------------------------------------------------------------- */
/* GPIO Initialization                                                        */
/* -------------------------------------------------------------------------- */

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* Initial output states. */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 |
        GPIO_PIN_8 | GPIO_PIN_9 |
        LED_1_Pin | LED_2_Pin,
        GPIO_PIN_RESET
    );
    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_0 | GPIO_PIN_1,
        GPIO_PIN_RESET
    );

    /* PC13 output. */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* PA5, PA6 and PA7 outputs. */
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* PB0 and PB1 outputs. */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* PB11-PB14 level-switch inputs with pull-up. */
    GPIO_InitStruct.Pin =
        GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* PB15 level-switch input with pull-down. */
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* PA8, PA9 and LED outputs. */
    GPIO_InitStruct.Pin =
        GPIO_PIN_8 | GPIO_PIN_9 | LED_1_Pin | LED_2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* -------------------------------------------------------------------------- */
/* Error Handler                                                              */
/* -------------------------------------------------------------------------- */

void Error_Handler(void)
{
    __disable_irq();

    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the source file and line number where assert_param failed.
 * @param file Source file name.
 * @param line Source line number.
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;
    (void)line;

    /* Add debug handling here if required. */
}

#endif /* USE_FULL_ASSERT */
