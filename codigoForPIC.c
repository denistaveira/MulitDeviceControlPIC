// CHAVES DE FUNÇÃO:
//  --------CHAVE1--------  --------CHAVE2---------
// |GLCD\LCD ( 1) = OFF   |DIS1    ( 1) = OFF   |
// |RX       ( 2) = ON    |DIS2    ( 2) = OFF   |
// |TX       ( 3) = ON    |DIS3    ( 3) = OFF   |
// |REL1     ( 4) = OFF   |DIS4    ( 4) = OFF   |
// |REL2     ( 5) = OFF   |INFR    ( 5) = OFF   |
// |SCK      ( 6) = OFF   |RESIS   ( 6) = OFF   |
// |SDA      ( 7) = OFF   |TEMP    ( 7) = OFF   |
// |RTC      ( 8) = OFF   |VENT    ( 8) = OFF   |
// |LED1     ( 9) = OFF   |AN0     ( 9) = OFF   |
// |LED2     (10) = OFF   |AN1     (10) = OFF   |
//  --------------------- ----------------------

// --- Ligações entre PIC e LCD ---
sbit LCD_RS at RE2_bit;   // PINO 2 DO PORTD INTERLIGADO AO RS DO DISPLAY
sbit LCD_EN at RE1_bit;   // PINO 3 DO PORTD INTERLIGADO AO EN DO DISPLAY
sbit LCD_D7 at RD7_bit;  // PINO 7 DO PORTD INTERLIGADO AO D7 DO DISPLAY
sbit LCD_D6 at RD6_bit;  // PINO 6 DO PORTD INTERLIGADO AO D6 DO DISPLAY
sbit LCD_D5 at RD5_bit;  // PINO 5 DO PORTD INTERLIGADO AO D5 DO DISPLAY
sbit LCD_D4 at RD4_bit;  // PINO 4 DO PORTD INTERLIGADO AO D4 DO DISPLAY

// Selecionando direção de fluxo de dados dos pinos utilizados para a comunicação com display LCD
sbit LCD_RS_Direction at TRISE2_bit;  // SETA DIREÇÃO DO FLUXO DE DADOS DO PINO 2 DO PORTD
sbit LCD_EN_Direction at TRISE1_bit;  // SETA DIREÇÃO DO FLUXO DE DADOS DO PINO 3 DO PORTD
sbit LCD_D7_Direction at TRISD7_bit;  // SETA DIREÇÃO DO FLUXO DE DADOS DO PINO 7 DO PORTD
sbit LCD_D6_Direction at TRISD6_bit;  // SETA DIREÇÃO DO FLUXO DE DADOS DO PINO 6 DO PORTD
sbit LCD_D5_Direction at TRISD5_bit;  // SETA DIREÇÃO DO FLUXO DE DADOS DO PINO 5 DO PORTD
sbit LCD_D4_Direction at TRISD4_bit;  // SETA DIREÇÃO DO FLUXO DE DADOS DO PINO 4 DO PORTD

void main(){

   char ucRead;        // Variavel para armazenar o dado lido.
   unsigned int Control = 1;  // +++++++ VARIAVEL DE CONTROLE DA COMUNICAÇÃO ++++++++
                              // LEMBRE DE ALTERAR ESSA VARIAVE TAMBEM NO ARDUINO
   unsigned int estado = 0;   //O código inicia com o estado igual a zero
   int x = 0;
   adcon1 = 6;
   ADCON1  = 0x0E;                           //Configura os pinos do PORTB como digitais, e RA0 (PORTA) como analógico
   trisc.f0 = 0;   //configura pino RC0 como saida
   trise.f0 = 0;   //configura pino RE0 como saida
   Lcd_Init();                               //Inicializa módulo LCD
   Lcd_Cmd(_LCD_CURSOR_OFF);                 //Apaga cursor
   Lcd_Cmd(_LCD_CLEAR);                      //Limpa display
   PORTA = 255;
   TRISA = 255;                 // Configura PORTA pins como entrada
   
   UART1_Init(9600);  // Utiliza bibliotecas do compilador para configuração o Baud rate.
   Lcd_Cmd(_LCD_CLEAR);     //Limpa o display do LCD
   trisc.f0 = 0;   //configura pino RC0 como saida
   trise.f0 = 0;   //configura pino RE0 como saida

   PWM1_Init(5000);      // Inicializa m?dulo PWM com 5Khz
   PWM1_Start();         // Start PWM
   
   while(1){  // SELECIONE A VARIAVEL DE CONTROLE (CONTROL) DECLARADA ACIMA.

     if (Control == 1){   // O PIC (Control = 0) envia um caracter e o Arduino responde com outro caracter.
       if(UART1_Data_Ready()){  // Verifica se o dado enviado foi recebido no buffer
         ucRead = UART1_Read(); // Lê o primeiro dado da serial.
         Delay_ms(50);   // Pausa de 50ms.
         if (ucRead == 'H'){   //Se o caracter recebido for H deve-se acionar algum componente
          estado = 1;          //O estado passa a ser 1 quando recebe H ou L
          Lcd_Cmd(_LCD_CLEAR);        //Limpa o display do LCD
          lcd_out(1,1,"Acender");
          //lcd_chr_cp (ucRead);
          x = 1;              //Determina o estado (ligado) do componente
         }
         if (ucRead == 'L'){  //Se o caracter recebido for L deve-se desligar algum componente
          estado = 1;         //O estado passa a ser 1 quando recebe H ou L
          Lcd_Cmd(_LCD_CLEAR);      //Limpa o display do LCD
          lcd_out(1,1,"Apagar");
          //lcd_chr_cp (ucRead);
          x = 0;              //Determina o estado (desligado) do componente
         }
         if (ucRead == 'A' &&  estado ==1){  //Se o novo caracter for 'A' e o caractere anterior for H ou L, realiza a ação sobre o componente 1
            estado = 0;        //Volta ao estado 0
            lcd_out(2,1,"Rele 1");
            portc.f0 = x;      //Liga o dispositivo se x for 1 e desliga se for 0
            UART1_Write('1');  //Envia por serial 1 confirmando a ação no dispositivo 1
         }
         if (ucRead == 'B' &&  estado ==1){  //Se o novo caracter for 'B' e o caractere anterior for H ou L, realiza a ação sobre o componente 2
            estado = 0;        //Volta ao estado 0
            lcd_out(2,1,"Rele 2");
            porte.f0 = x;      //Liga o dispositivo se x for 1 e desliga se for 0
            UART1_Write('2');  //Envia por serial 2 confirmando a ação no dispositivo 2
         }
         if (ucRead == 'C' &&  estado ==1){  //Se o novo caracter for 'C' e o caractere anterior for H ou L, realiza a ação sobre a ventinha di FPGA
            estado = 0;        //Volta ao estado 0
            lcd_out(2,1,"Ventuinha");
            if(x == 0) PWM1_Set_Duty(0);      //Liga o dispositivo se x for 1 e desliga se for 0
            if(x == 1) PWM1_Set_Duty(255);
            UART1_Write('3');  //Envia por serial 3 confirmando a ação no dispositivo 3
         }
       }
     }
   }
}