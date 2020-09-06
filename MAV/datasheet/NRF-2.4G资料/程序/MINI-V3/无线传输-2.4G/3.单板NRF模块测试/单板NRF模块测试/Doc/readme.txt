
单板测试两个NRF模块

/* NRF1 把模块直接插入NRF模块专用接口即可
 *
 * NRF2 用杜邦线在SPI2接口与NRF模块相连
 *
 * 模块		--开发板
 * 1			--GND
 * 2			--3.3V
 * 3_CE		--PB12
 * 4_CSN	--PB8  
 * 5_SCK	--PB13
 * 6_MOSI	--PB15
 * 7_MISO	--PB14
 * 8_IRQ	--PB9	
 */