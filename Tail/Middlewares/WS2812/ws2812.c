#include "ws2812.h"
#include "delay.h"

uint8_t pixelBuffer[PIXEL_NUM][24]; //存储每一个像素的单个像素位，用以DMA传输到SPI的DR寄存器对RGB进行控制

uint8_t order_array[PIXEL_NUM] = {6,5,4,3,2,1,0,7,8,9,10,11,12,13,14,15,24,23,22,21,20,19,18,17,16,25,26,27,28,29,30,31,32,33,42,41,40,39,38,37,36,35,34,43,44,45,46,47,48,49,50,51,60,59,58,57,56,55,54,53,52,61,62,63,64,65,66,67,68,69,76,75,74,73,72,71,70};
uint8_t jump_array[] = {6,69,0};

void ws281x_init(void)
{
  gpio_init_type gpio_initstructure;
  spi_init_type  spi_init_struct;
  dma_init_type dma_init_struct;

	crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);//GPIOA时钟使能 
	crm_periph_clock_enable(CRM_SPI1_PERIPH_CLOCK, TRUE);//SPI1时钟使能 	
  crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);//使能DMA传输
	
	/* spi1 mosi pin */
  gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;  
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pull           = GPIO_PULL_NONE;  
  gpio_initstructure.gpio_mode           = GPIO_MODE_MUX; 
  gpio_initstructure.gpio_pins = GPIO_PINS_7;
  gpio_init(GPIOA, &gpio_initstructure);
	
	spi_default_para_init(&spi_init_struct);
  spi_init_struct.transmission_mode = SPI_TRANSMIT_HALF_DUPLEX_TX;
  spi_init_struct.master_slave_mode =SPI_MODE_MASTER;
  spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_16;
  spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
  spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
  spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_LOW;
  spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
  spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
  spi_init(SPI1, &spi_init_struct);
	
  spi_enable(SPI1, TRUE);
	spi_i2s_dma_transmitter_enable(SPI1,TRUE);
	
  dma_reset(DMA1_CHANNEL3);
  dma_default_para_init(&dma_init_struct);  
  /*容量配置*/
  dma_init_struct.buffer_size =PIXEL_NUM * 24;
  /*方向:由内存到外设*/
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  /*内存地址*/
  dma_init_struct.memory_base_addr = (uint32_t)pixelBuffer;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
  dma_init_struct.memory_inc_enable = TRUE;
  /*外设地址*/
  dma_init_struct.peripheral_base_addr = (uint32_t) &(SPI1 -> dt);
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init(DMA1_CHANNEL3, &dma_init_struct); 
  
  ws281x_closeAll();  //关闭全部的灯
  delay_ms(100); //关闭全部的灯需要一定的时间  
}

void ws281x_closeAll(void)
{
  uint16_t i;
  uint8_t j;
  
  for(i = 0; i < PIXEL_NUM; ++i)
  {
    for(j = 0; j < 24; ++j)
    {
      pixelBuffer[i][j] = WS_LOW;
    }
  }
  ws281x_show(); 
}


uint32_t ws281x_color(uint8_t red, uint8_t green, uint8_t blue)
{
  return green << 16 | red << 8 | blue;
}

void ws281x_setPixelColor(uint16_t n ,uint32_t GRBcolor)
{
  uint8_t i;
  if(n < PIXEL_NUM)
  {
    for(i = 0; i < 24; ++i)
    {
      pixelBuffer[n][i] = (((GRBcolor << i) & 0X800000) ? WS_HIGH : WS_LOW);
    }
  }
}

void ws281x_setPixelRGB(uint16_t n ,uint8_t red, uint8_t green, uint8_t blue)
{
  uint8_t i;
  
  if(n < PIXEL_NUM)
  {
    for(i = 0; i < 24; ++i)
    {
      pixelBuffer[n][i] = (((ws281x_color(red,green,blue) << i) & 0X800000) ? WS_HIGH : WS_LOW);
    }
  }
}

void ws281x_closePixel(uint16_t n)
{
  uint8_t i;
  if(n < PIXEL_NUM)
  {
    for(i = 0; i < 24; ++i)
    {
      pixelBuffer[n][i] = WS_LOW;
    }
  }
}

void ws281x_show(void)
{
    dma_channel_enable(DMA1_CHANNEL3, FALSE);//关闭SPI1 MOSI DMA1所指示的通道 
    dma_flag_clear(DMA1_FDT3_FLAG);    
 	  dma_data_number_set(DMA1_CHANNEL3, 24 * PIXEL_NUM );//DMA通道的DMA缓存的大小
 	  dma_channel_enable(DMA1_CHANNEL3, TRUE);//使能SPI1 MOSI DMA1所指示的通道 
}

void ws281x_setNothing(void)
{
	
}

void ws281x_setColor(uint32_t GRBcolor)
{
	uint8_t i;
	for(i=0; i<PIXEL_NUM; i++)
	{
		ws281x_setPixelColor(i, GRBcolor);
  }
	ws281x_show();
}

void ws281x_setArrayColor(const uint8_t * array, uint32_t GRBcolor)
{
	uint8_t i,j = 0;
	array++;
	for(i=0; i<PIXEL_NUM; i++)
	{
		*array ? ws281x_setPixelColor(order_array[i], GRBcolor) : ws281x_setPixelColor(order_array[i], 0x000000);
		array++;if(i==jump_array[j]){array++;j++;}
  }
	ws281x_show();
}

void ws281x_setMixArrayColor(const uint8_t * array1, uint32_t GRBcolor1,const uint8_t * array2, uint32_t GRBcolor2)
{
	uint8_t i,j = 0;
	array1++;array2++;
	for(i=0; i<PIXEL_NUM; i++)
	{
		*array2 ? ws281x_setPixelColor(order_array[i], GRBcolor2) : (*array1 ? ws281x_setPixelColor(order_array[i], GRBcolor1) : ws281x_setPixelColor(order_array[i], 0x000000));
		array1++;array2++;if(i==jump_array[j]){array1++;array2++;j++;}
  }
	ws281x_show();
}

void ws281x_addArrayColor(const uint8_t * array, uint32_t GRBcolor)
{
	uint8_t i,j = 0;
	array++;
	for(i=0; i<PIXEL_NUM; i++)
	{
		*array ? ws281x_setPixelColor(order_array[i], GRBcolor) : ws281x_setNothing();
		array++;if(i==jump_array[j]){array++;j++;}
	}
	ws281x_show();
}

void ws281x_setArrayPixelRGB(const uint8_t * red, const uint8_t * green, const uint8_t * blue)
{
	uint8_t i;
	for(i=0; i<PIXEL_NUM; i++)
	{
		ws281x_setPixelRGB(order_array[i], *red, *green, *blue);
		red++;green++;blue++;
	}
	ws281x_show();
}

//uint32_t ws281x_wheel(uint8_t wheelPos) {
//  wheelPos = 255 - wheelPos;
//  if(wheelPos < 85) {
//    return ws281x_color(255 - wheelPos * 3, 0, wheelPos * 3);
//  }
//  if(wheelPos < 170) {
//    wheelPos -= 85;
//    return ws281x_color(0, wheelPos * 3, 255 - wheelPos * 3);
//  }
//  wheelPos -= 170;
//  return ws281x_color(wheelPos * 3, 255 - wheelPos * 3, 0);
//}

uint32_t ws281x_wheel(uint8_t wheelPos) {
  wheelPos = 255 - wheelPos;
  if(wheelPos < 85) {
    return ws281x_color((255-wheelPos*3)/25, 0, (wheelPos*3)/25);
  }
  if(wheelPos < 170) {
    wheelPos -= 85;
    return ws281x_color(0, (wheelPos*3)/25, (255-wheelPos*3)/25);
  }
  wheelPos -= 170;
  return ws281x_color((wheelPos*3)/25, (255-wheelPos*3)/25, 0);
}

void ws281x_rainbow(uint8_t wait) {
  uint16_t i;

	for(i=0; i<PIXEL_NUM; i++) {
		ws281x_setPixelColor(i, ws281x_wheel((i+wait) & 255));
	}
	ws281x_show();
}

//void ws281x_rainbow(uint8_t wait) {
//  uint16_t i, j;

//  for(j=0; j<256; j++) {
//    for(i=0; i<PIXEL_NUM; i++) {
//      ws281x_setPixelColor(i, ws281x_wheel((i+j) & 255));
//    }
//    ws281x_show();
////    delay_ms(wait);
//  }
//}






