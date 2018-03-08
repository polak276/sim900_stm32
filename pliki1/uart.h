#ifndef _UART_H
#define _UART_H

#define BUFFER_SIZE 100

void uart_receive(char bajt);
void send_command_wait100ms(char * command);
void clear_rx(void);
void send_command_to_sim900(char * command, int opoznienie);
void get_message(void);
void send_names(void);
void odeslij_potwierdzenie(void);
void odeslij_blad(void);

#endif
