// spimaster.c: Softwareschnittstelle für TWI
// 
// //////////
// Header //
// //////////
// 
// #include "spimaster.h"
// 
// //////////////
// Funktionen //
// //////////////
// ISR(SPIC_INT_vect)
// {
// 	//SPIMasterInterruptHandler();
// }
// 
// void SPIMasterInterruptHandler(SPI_Master_t* spi)
// {
// 	spi->read_buffer[bytes_written-1] = spi->interface->DATA;	//Byte an MISO lesen
// 	if(bytes_written<=bytes_to_write)							//Weiteres Byte senden
// 		spi->interface->DATA = spi->write_buffer[spi->bytes_written++];
// 	else														//Transaktion fertig
// 	{
// 		spi->status = SPI_STATUS_SUCCESS;
// 	}
// 
// }
// 
// void SPIMasterInit(SPI_Master_t* spi, SPI_t *module,uint8_t intLevel, uint8_t prescaler)
// {
// 	spi->interface = module;
// 	spi->interface->CTRL = (1<<6) | (0<<5) | (1<<4) | (0b00<<2) | prescaler;//Enable|msb|Master|SPI Transfer Mode (siehe Specsheet 20.7.1)|Prescaler
// 	spi->interface->INTCTRL = intLevel; //Bit0,1: Interupt Level/Enable
// 	
// }
// 
// void SPIMasterWriteRead(SPI_Master_t* spi, uint8_t * data[], uint8_t lenght)
// {
// 	if(lenght<SPI_BUFFER_SIZE)
// 	{
// 		spi->write_buffer = data;
// 		spi->bytes_to_write = lenght;
// 
// 		spi->status = SPI_STATUS_BLOCKED;
// 
// 		spi->interface->DATA = spi->write_buffer[0];
// 		spi->bytes_written = 1;
// 	}
// }