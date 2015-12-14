#ifndef _SPI_H_
#define _SPI_H_

void spi_master_init();
void spi_master_cs_set();
void spi_master_cs_clear();
void spi_master_send(uint8_t data);

#endif
