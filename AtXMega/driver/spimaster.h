// // spimaster.h: Implementation für TWI I²C-Interface
// 
// #ifndef _SPI_MASTER_IMP_
// #define _SPI_MASTER_IMP_
// 
// #define SPI_BUFFER_SIZE
// 
// // Für C++-Files
// #ifdef __cplusplus
// 	extern "C" {
// #endif
// 
// #include <stdbool.h>
// #include <avr/io.h>
// #include <avr/interrupt.h>
// 
// enum SPI_status{
// 	SPI_STATUS_BLOCKED = 1,
// 	SPI_STATUS_SUCCESS = 2
// }
// 
// // Datenstruktur für TWI-Interface
// typedef struct SPI_Master {
// 	SPI_t *interface,
// 	uint8_t *write_buffer[SPI_BUFFER_SIZE],
// 	uint8_t *read_buffer[SPI_BUFFER_SIZE],
// 	uint8_t bytes_to_write=0,
// 	uint8_t bytes_written=0,
// 	uint8_t status
// } SPI_Master_t;
// 
// 
// 
// void SPIMasterInit(SPI_Master_t* spi, SPI_t *module,uint8_t intLevel, uint8_t prescaler);
// void SPIMasterWriteRead(SPI_Master_t* spi, uint8_t * data[]);
// void SPIMasterInterruptHandler(SPI_Master_t* spi);
// 
// // Für C++-Files
// #ifdef __cplusplus
// 	}
// #endif
// 
// #endif