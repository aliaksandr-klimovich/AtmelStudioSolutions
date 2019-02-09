#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>
#include "SPI_config.h"

void SPI_MasterInit(void);
void SPI_MasterTransmit(char cData);
void SPI_SlaveInit(void);
char SPI_SlaveReceive(void);

#endif /* SPI_H_ */