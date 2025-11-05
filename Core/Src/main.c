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
#include "loopback.h"
#include "wizchip_conf.h"

/************ W6300 Pinout ************

    PA6     ------> QUADSPI_BK2_IO0
    PA7     ------> QUADSPI_BK2_IO1
    PC4     ------> QUADSPI_BK2_IO2
    PC5     ------> QUADSPI_BK2_IO3
    PB1     ------> QUADSPI_CLK
    PC11   ------> QUADSPI_BK2_NCS
    PA4     ------> QSPI_RST

***************************************/


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
QSPI_HandleTypeDef hqspi;

UART_HandleTypeDef huart3;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* USER CODE BEGIN PV */
static wiz_NetInfo g_net_info = {
    .mac = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56}, // MAC address
    .ip = {192, 168, 11, 2},                     // IP address
    .sn = {255, 255, 255, 0},                    // Subnet Mask
    .gw = {192, 168, 11, 1},                     // Gateway
    .dns = {8, 8, 8, 8},                         // DNS server
#if _WIZCHIP_ > W5500
    .lla = {
        0xfe, 0x80, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x02, 0x08, 0xdc, 0xff,
        0xfe, 0x57, 0x57, 0x25
    },             // Link Local Address
    .gua = {
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    },             // Global Unicast Address
    .sn6 = {
        0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    },             // IPv6 Prefix
    .gw6 = {
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    },             // Gateway IPv6 Address
    .dns6 = {
        0x20, 0x01, 0x48, 0x60,
        0x48, 0x60, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x88, 0x88
    },             // DNS6 server
    .ipmode = NETINFO_STATIC_ALL
#else
    .dhcp = NETINFO_STATIC
#endif
};

static uint8_t g_tcp_server_buf[2048] = {
    0,
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_QUADSPI_Init(void);
/* USER CODE BEGIN PFP */
void print_network_information(wiz_NetInfo net_info);
void print_ipv6_addr(uint8_t* name, uint8_t* ip6addr);
void wizchip_initialize(void);
void wizchip_reset(void);
static inline void wizchip_deselect(void);
static inline void wizchip_select(void);
void W6300_QspiReadByte(uint8_t opcode, uint32_t addr, uint8_t *pbuf, uint16_t len);
void W6300_QspiWriteByte(uint8_t opcode, uint32_t addr, uint8_t *pbuf, uint16_t len);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
uint8_t arr[2] = {0xff, 0x55,0,0};
/* USER CODE BEGIN 0 */
int __io_putchar(int ch)
{
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}
/* USER CODE END 0 */

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

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_QUADSPI_Init();
  /* USER CODE BEGIN 2 */
  wizchip_reset();
  wizchip_initialize();
  network_initialize(g_net_info);
  print_network_information(g_net_info);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  loopback_tcps(0, g_tcp_server_buf, 5000);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 384;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief QUADSPI Initialization Function
  * @param None
  * @retval None
  */
static void MX_QUADSPI_Init(void)
{

  /* USER CODE BEGIN QUADSPI_Init 0 */

  /* USER CODE END QUADSPI_Init 0 */

  /* USER CODE BEGIN QUADSPI_Init 1 */

  /* USER CODE END QUADSPI_Init 1 */
  /* QUADSPI parameter configuration*/
	  hqspi.Instance = QUADSPI;
	  hqspi.Init.ClockPrescaler = 32;
	  hqspi.Init.FifoThreshold = 1;
	  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_NONE;
	  hqspi.Init.FlashSize = 23;
	  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_3_CYCLE;
	  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
	  hqspi.Init.FlashID = QSPI_FLASH_ID_2;
	  hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
	  if (HAL_QSPI_Init(&hqspi) != HAL_OK)
	  {
	    Error_Handler();
	  }
  /* USER CODE BEGIN QUADSPI_Init 2 */

  /* USER CODE END QUADSPI_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 6;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.battery_charging_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(QSPI_RST_GPIO_Port, QSPI_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : QSPI_RST_Pin */
  GPIO_InitStruct.Pin = QSPI_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QSPI_RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void W6300_QspiWriteByte(uint8_t opcode, uint32_t addr, uint8_t *pbuf, uint16_t len)
{
    QSPI_CommandTypeDef sCommand = {0};

	if(_WIZCHIP_QSPI_MODE_ == QSPI_SINGLE_MODE){
		sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
	    sCommand.AlternateByteMode    = QSPI_ALTERNATE_BYTES_1_LINE;
		sCommand.DataMode = QSPI_DATA_1_LINE;

	}
    else if(_WIZCHIP_QSPI_MODE_ == QSPI_DUAL_MODE){
		sCommand.AddressMode = QSPI_ADDRESS_2_LINES;
	    sCommand.AlternateByteMode    = QSPI_ALTERNATE_BYTES_2_LINES;
		sCommand.DataMode = QSPI_DATA_2_LINES;
    }
    else if(_WIZCHIP_QSPI_MODE_ == QSPI_QUAD_MODE){
		sCommand.AddressMode = QSPI_ADDRESS_4_LINES;
	    sCommand.AlternateByteMode    = QSPI_ALTERNATE_BYTES_4_LINES;
		sCommand.DataMode = QSPI_DATA_4_LINES;
    }

    // 1. Instruction Phase (1 byte)
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction = opcode;

    // 2. Address Phase (2 byte)
    sCommand.AddressSize = QSPI_ADDRESS_16_BITS;
    sCommand.Address = addr & 0xFFFF;

    // 3. Dummy Phase (1 byte)
//    sCommand.DummyCycles = 2;
    sCommand.AlternateBytesSize   = QSPI_ALTERNATE_BYTES_8_BITS;
    sCommand.AlternateBytes       = 0x00;                // replace Dummy with Alt (Using dummy parameters causes sampling errors in dual and quad modes)

    // 4. Data Phase (n byte)
    sCommand.NbData = len;

    // 5. And so on
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;


    if (HAL_QSPI_Command(&hqspi, &sCommand, 1000) == HAL_OK)
    {
//        HAL_QSPI_Transmit_DMA(&hqspi, pbuf);
        HAL_QSPI_Transmit(&hqspi, pbuf, 1000);
    }

    while (__HAL_QSPI_GET_FLAG(&hqspi, QSPI_FLAG_BUSY))
        ;
}

void W6300_QspiReadByte(uint8_t opcode, uint32_t addr, uint8_t *pbuf, uint16_t len)
{
    QSPI_CommandTypeDef sCommand = {0};

	if(_WIZCHIP_QSPI_MODE_ == QSPI_SINGLE_MODE){
		sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
		sCommand.DataMode = QSPI_DATA_1_LINE;
		sCommand.DummyCycles = 8;
	}
    else if(_WIZCHIP_QSPI_MODE_ == QSPI_DUAL_MODE){
		sCommand.AddressMode = QSPI_ADDRESS_2_LINES;
		sCommand.DataMode = QSPI_DATA_2_LINES;
		sCommand.DummyCycles = 4;
    }
    else if(_WIZCHIP_QSPI_MODE_ == QSPI_QUAD_MODE){
		sCommand.AddressMode = QSPI_ADDRESS_4_LINES;
		sCommand.DataMode = QSPI_DATA_4_LINES;
		sCommand.DummyCycles = 2;
    }

    // 1. Instruction Phase (1 byte)
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction = opcode;

    // 2. Address Phase (2 byte)
    sCommand.AddressSize = QSPI_ADDRESS_16_BITS;
    sCommand.Address = addr & 0xFFFF;

    // 3. Data Phase (n byte)
    sCommand.NbData = len;

    // 4. And so on
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    if (HAL_QSPI_Command(&hqspi, &sCommand, 1000) == HAL_OK)
    {
        HAL_QSPI_Receive(&hqspi, pbuf,1000);
    }

    while (__HAL_QSPI_GET_FLAG(&hqspi, QSPI_FLAG_BUSY))
        ;
}


static inline void wizchip_select(void) {
}

static inline void wizchip_deselect(void) {
}

void wizchip_reset(void) {

   HAL_GPIO_WritePin(QSPI_RST_GPIO_Port, QSPI_RST_Pin, GPIO_PIN_RESET);
   HAL_Delay(100);

   HAL_GPIO_WritePin(QSPI_RST_GPIO_Port, QSPI_RST_Pin, GPIO_PIN_SET);
   HAL_Delay(100);
}

void network_initialize(wiz_NetInfo net_info) {
   ctlnetwork(CN_SET_NETINFO, (void*) &net_info);
}

void wizchip_initialize(void) {

#if   _WIZCHIP_QSPI_MODE_ == QSPI_SINGLE_MODE
    printf("QSPI Mode: SINGLE\n");
#elif _WIZCHIP_QSPI_MODE_ == QSPI_DUAL_MODE
    printf("QSPI Mode: DUAL\n");

#elif _WIZCHIP_QSPI_MODE_ == QSPI_QUAD_MODE
    printf("QSPI Mode: QUAD\n");
#endif

   reg_wizchip_qspi_cbfunc(W6300_QspiReadByte, W6300_QspiWriteByte);
   reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
   /* W5x00 initialize */
   uint8_t temp;

   uint8_t memsize[2][8] = {{4, 4, 4, 4, 4, 4, 4, 4}, {4, 4, 4, 4, 4, 4, 4, 4}};


   if (ctlwizchip(CW_INIT_WIZCHIP, (void *)memsize) == -1)
      {
          #if _WIZCHIP_ <= W5500
          printf(" W5x00 initialized fail\n");
          #else
          printf(" W6x00 initialized fail\n");
          #endif

          return;
      }
      /* Check PHY link status */
   wiz_NetInfo tmp;
   ctlnetwork(CN_GET_NETINFO, (void*)&tmp);
      do
      {
          if (ctlwizchip(CW_GET_PHYLINK, (void *)&temp) == -1)
          {
              printf(" Unknown PHY link status\n");

              return;
          }
      } while (temp == PHY_LINK_OFF);
}


void print_network_information(wiz_NetInfo net_info)
{
    uint8_t tmp_str[8] = {
        0,
    };

    ctlnetwork(CN_GET_NETINFO, (void *)&net_info);
    ctlwizchip(CW_GET_ID, (void *)tmp_str);
#if _WIZCHIP_ <= W5500
    if (net_info.dhcp == NETINFO_DHCP)
    {
        printf("====================================================================================================\n");
        printf(" %s network configuration : DHCP\n\n", (char *)tmp_str);
    }
    else
    {
        printf("====================================================================================================\n");
        printf(" %s network configuration : static\n\n", (char *)tmp_str);
    }

    printf(" MAC         : %02X:%02X:%02X:%02X:%02X:%02X\n", net_info.mac[0], net_info.mac[1], net_info.mac[2], net_info.mac[3], net_info.mac[4], net_info.mac[5]);
    printf(" IP          : %d.%d.%d.%d\n", net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3]);
    printf(" Subnet Mask : %d.%d.%d.%d\n", net_info.sn[0], net_info.sn[1], net_info.sn[2], net_info.sn[3]);
    printf(" Gateway     : %d.%d.%d.%d\n", net_info.gw[0], net_info.gw[1], net_info.gw[2], net_info.gw[3]);
    printf(" DNS         : %d.%d.%d.%d\n", net_info.dns[0], net_info.dns[1], net_info.dns[2], net_info.dns[3]);
    printf("====================================================================================================\n\n");
#else
    printf("==========================================================\n");
    printf(" %s network configuration\n\n", (char *)tmp_str);

    printf(" MAC         : %02X:%02X:%02X:%02X:%02X:%02X\r\n", net_info.mac[0], net_info.mac[1], net_info.mac[2], net_info.mac[3], net_info.mac[4], net_info.mac[5]);
    printf(" IP          : %d.%d.%d.%d\r\n", net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3]);
    printf(" Subnet Mask : %d.%d.%d.%d\r\n", net_info.sn[0], net_info.sn[1], net_info.sn[2], net_info.sn[3]);
    printf(" Gateway     : %d.%d.%d.%d\r\n", net_info.gw[0], net_info.gw[1], net_info.gw[2], net_info.gw[3]);
    printf(" DNS         : %d.%d.%d.%d\r\n", net_info.dns[0], net_info.dns[1], net_info.dns[2], net_info.dns[3]);
    print_ipv6_addr(" GW6 ", net_info.gw6);
    print_ipv6_addr(" LLA ", net_info.lla);
    print_ipv6_addr(" GUA ", net_info.gua);
    print_ipv6_addr(" SUB6", net_info.sn6);
    print_ipv6_addr(" DNS6", net_info.dns6);
    printf("==========================================================\n\n");
#endif

}

void print_ipv6_addr(uint8_t* name, uint8_t* ip6addr)
{
	printf("%s        : ", name);
	printf("%04X:%04X", ((uint16_t)ip6addr[0] << 8) | ((uint16_t)ip6addr[1]), ((uint16_t)ip6addr[2] << 8) | ((uint16_t)ip6addr[3]));
	printf(":%04X:%04X", ((uint16_t)ip6addr[4] << 8) | ((uint16_t)ip6addr[5]), ((uint16_t)ip6addr[6] << 8) | ((uint16_t)ip6addr[7]));
	printf(":%04X:%04X", ((uint16_t)ip6addr[8] << 8) | ((uint16_t)ip6addr[9]), ((uint16_t)ip6addr[10] << 8) | ((uint16_t)ip6addr[11]));
	printf(":%04X:%04X\r\n", ((uint16_t)ip6addr[12] << 8) | ((uint16_t)ip6addr[13]), ((uint16_t)ip6addr[14] << 8) | ((uint16_t)ip6addr[15]));
}
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
