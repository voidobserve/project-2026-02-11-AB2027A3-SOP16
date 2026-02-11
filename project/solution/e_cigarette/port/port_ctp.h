#ifndef _PORT_CTP_H
#define _PORT_CTP_H


#define PORT_CTP_SCL_PORT         GPIOA_REG       
#define PORT_CTP_SCL_PIN          GPIO_PIN_3

#define PORT_CTP_SDA_PORT         GPIOA_REG       
#define PORT_CTP_SDA_PIN          GPIO_PIN_4

#define PORT_CTP_INT_PORT         GPIOA_REG       
#define PORT_CTP_INT_PIN          GPIO_PIN_5

#define PORT_CTP_RST_PORT         GPIOA_REG       
#define PORT_CTP_RST_PIN          GPIO_PIN_9

#define PORT_CTP_RST_H()          gpio_set_bits(PORT_CTP_RST_PORT, PORT_CTP_RST_PIN);
#define PORT_CTP_RST_L()          gpio_reset_bits(PORT_CTP_RST_PORT, PORT_CTP_RST_PIN);

void port_ctp_init(void);
void port_ctp_reset(void);
void port_ctp_exit(void);

#endif  //_PORT_CTP_H
