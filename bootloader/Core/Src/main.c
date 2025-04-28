/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"


#include <stdio.h>
#include "shell_port.h"
#include "logging.h"

#include "bootutil/bootutil.h"
#include "bootutil/image.h"


#include <string.h>


/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef hlpuart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_LPUART1_UART_Init(void);
int Serial_ReadChar(void);
static void do_boot(struct boot_rsp *rsp);


void GreenLED(GPIO_PinState State)
{
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, State);
}
void BlueLED(GPIO_PinState State)
{
  HAL_GPIO_WritePin(GPIOB, LD2_Pin, State);
}
void RedLED(GPIO_PinState State)
{
  HAL_GPIO_WritePin(GPIOB, LD3_Pin, State);
}



int read_flash() {
  const struct flash_area *fa;
  int res = flash_area_open(0, &fa);  // Open primary flash area
  if (res != 0) {
      printf("Failed to open flash area\n");
      return -1;
  }

  // Check if the offset and length are within bounds
  uint32_t read_offset = 0;
  uint32_t read_length = 256;
  if (read_offset + read_length > fa->fa_size) {
      printf("Read out of bounds\n");
      flash_area_close(fa);
      return -1;
  }

  uint8_t buffer[256];  // Buffer to hold read data
  res = flash_area_read(fa, read_offset, buffer, read_length);
  if (res != 0) {
      printf("Flash read failed\n");
  } else {
      printf("Flash read succeeded\n");
      // Optionally, print the buffer content to verify
      for (int i = 0; i < 256; i++) {
          printf("%02X ", buffer[i]);
      }
  }

  flash_area_close(fa);  // Don't forget to close the area
  return res;
}


#define APP_START_ADDRESS 0x08020000

void jump_to_application(void)
{
    // Disable all interrupts
    __disable_irq();

    // Deinit peripherals here if needed (e.g., clocks, UART, timers)

    // Clear pending interrupts
    for (int i = 0; i < 8; i++) {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    // Set vector table to new app
    SCB->VTOR = APP_START_ADDRESS;

    // Set Main Stack Pointer
    __set_MSP(*(volatile uint32_t *)APP_START_ADDRESS);

    // Jump to application reset handler
    uint32_t app_reset_handler = *(volatile uint32_t *)(APP_START_ADDRESS + 4);
    void (*reset_handler)(void) = (void (*)(void))app_reset_handler;
    reset_handler();
}




/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  setvbuf(stdout, NULL, _IONBF, 0);  // <--- Add this line here
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_LPUART1_UART_Init();
  
  //Turn RX IN on for UART
  HAL_NVIC_SetPriority(LPUART1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(LPUART1_IRQn);

  BlueLED(GPIO_PIN_SET);

  // Echo "Hello World"
  char msg[] = "Hurr Durr, I'm a Bootloader\r\n";
  HAL_UART_Transmit(&hlpuart1, (uint8_t*)msg, sizeof(msg)-1, HAL_MAX_DELAY);

  //read_flash();
  
  EXAMPLE_LOG("\n\r___  ________ _   _ _                 _   ");
  EXAMPLE_LOG("|  \\/  /  __ \\ | | | |               | |  ");
  EXAMPLE_LOG("| .  . | /  \\/ | | | |__   ___   ___ | |_ ");
  EXAMPLE_LOG("| |\\/| | |   | | | | '_ \\ / _ \\ / _ \\| __|");
  EXAMPLE_LOG("| |  | | \\__/\\ |_| | |_) | (_) | (_) | |_ ");
  EXAMPLE_LOG("\\_|  |_/\\____/\\___/|_.__/ \\___/ \\___/ \\__|");

  EXAMPLE_LOG("==Starting Bootloader==");

  struct boot_rsp rsp;
  int rv = boot_go(&rsp);

  if (rv == 0) {
    do_boot(&rsp);
  }

  EXAMPLE_LOG("No bootable image found. Falling into Bootloader CLI:");

  shell_processing_loop();
  
  while (1)
  {
  }


}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}


#define RX_BUFFER_SIZE 128
volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
volatile uint16_t rx_head = 0;
volatile uint16_t rx_tail = 0;
static uint8_t rx_byte;  // temp storage for each received byte

void MX_LPUART1_UART_Init(void)
{
    hlpuart1.Instance = LPUART1;
    hlpuart1.Init.BaudRate = 115200;
    hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
    hlpuart1.Init.StopBits = UART_STOPBITS_1;
    hlpuart1.Init.Parity = UART_PARITY_NONE;
    hlpuart1.Init.Mode = UART_MODE_TX_RX;
    hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    HAL_UART_Init(&hlpuart1);

    // Start first receive interrupt
    HAL_UART_Receive_IT(&hlpuart1, &rx_byte, 1);
}

// This function is called by HAL when a UART receive interrupt happens
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == LPUART1)
    {
        uint16_t next_head = (rx_head + 1) % RX_BUFFER_SIZE;

        if (next_head != rx_tail) // buffer not full
        {
            rx_buffer[rx_head] = rx_byte;
            rx_head = next_head;
        }
        // else: buffer full, you can handle overflow here if needed

        // Restart interrupt reception
        HAL_UART_Receive_IT(&hlpuart1, &rx_byte, 1);
    }
}

// Optional: helper function to read from the buffer
int Serial_ReadChar(void)
{
    if (rx_head == rx_tail)
    {
        return -1; // No data available
    }
    uint8_t ch = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % RX_BUFFER_SIZE;
    return ch;
}
void LPUART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&hlpuart1);
}

int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
    return len;
}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  HAL_PWREx_EnableVddIO2();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);



  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_8;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);



  
/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */


















///// Upstream Example 
__attribute__((noinline))
static void prv_enable_vfp( void ){
  volatile uint32_t *cpacr = (volatile uint32_t *)0xE000ED88;
  *cpacr |= 0xf << 20;
}

static void start_app(uint32_t pc, uint32_t sp) {
  __asm volatile ("MSR msp, %0" : : "r" (sp) : );
  void (*application_reset_handler)(void) = (void *)pc;
  application_reset_handler();
}

static void do_boot(struct boot_rsp *rsp) {
  EXAMPLE_LOG("Starting Main Application");
  EXAMPLE_LOG("  Image Start Offset: 0x%x", (int)rsp->br_image_off);

  // We run from internal flash. Base address of this medium is 0x0
  uint32_t vector_table = 0x0 + rsp->br_image_off + rsp->br_hdr->ih_hdr_size;

  uint32_t *app_code = (uint32_t *)vector_table;
  uint32_t app_sp = app_code[0];
  uint32_t app_start = app_code[1];

  EXAMPLE_LOG("  Vector Table Start Address: 0x%x. PC=0x%x, SP=0x%x",
              (int)vector_table, app_start, app_sp);

  // We need to move the vector table to reflect the location of the main application
  volatile uint32_t *vtor = (uint32_t *)0xE000ED08;
  *vtor = (vector_table & 0xFFFFFFF8);

  start_app(app_start, app_sp);
}

