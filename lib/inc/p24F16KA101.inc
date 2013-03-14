;==========================================================================
;  PIC24F16KA101 Standard Assembly Include File
; 
;  (c) Copyright 2012 Microchip Technology, All rights reserved
;==========================================================================

.NOLIST

;==========================================================================
;  This header file defines configurations, registers, and other useful
;  bits of information for the PIC24F16KA101 microcontroller.  These names
;  are taken to match the data sheets as closely as possible.
;
;  Note that the processor must be selected before this file is included.
;  The processor may be selected the following ways:
;
;       1. Command line switch:
;               C:\> pic30-as file.s -p24F16KA101
;       2. Placing a ".equ" directive before the ".include":
;               .equ __24F16KA101, 1
;               .include "p24F16KA101.inc"
;       3. Setting the processor in the MPLAB IDE Project Dialog
;==========================================================================

;==========================================================================
;
;       Verify Processor
;
;==========================================================================

.IFNDEF __24F16KA101
   .ERROR "Include file does not match processor setting."
.ENDIF


;==========================================================================
;
;       Register Definitions
;
;==========================================================================

;----- Register Equates (Low Byte, High Byte) -----------------------------

        .extern WREG0
        .equiv  WREG0L, _WREG0
        .equiv  WREG0H, _WREG0+1

        .extern WREG1
        .equiv  WREG1L, _WREG1
        .equiv  WREG1H, _WREG1+1

        .extern WREG2
        .equiv  WREG2L, _WREG2
        .equiv  WREG2H, _WREG2+1

        .extern WREG3
        .equiv  WREG3L, _WREG3
        .equiv  WREG3H, _WREG3+1

        .extern WREG4
        .equiv  WREG4L, _WREG4
        .equiv  WREG4H, _WREG4+1

        .extern WREG5
        .equiv  WREG5L, _WREG5
        .equiv  WREG5H, _WREG5+1

        .extern WREG6
        .equiv  WREG6L, _WREG6
        .equiv  WREG6H, _WREG6+1

        .extern WREG7
        .equiv  WREG7L, _WREG7
        .equiv  WREG7H, _WREG7+1

        .extern WREG8
        .equiv  WREG8L, _WREG8
        .equiv  WREG8H, _WREG8+1

        .extern WREG9
        .equiv  WREG9L, _WREG9
        .equiv  WREG9H, _WREG9+1

        .extern WREG10
        .equiv  WREG10L, _WREG10
        .equiv  WREG10H, _WREG10+1

        .extern WREG11
        .equiv  WREG11L, _WREG11
        .equiv  WREG11H, _WREG11+1

        .extern WREG12
        .equiv  WREG12L, _WREG12
        .equiv  WREG12H, _WREG12+1

        .extern WREG13
        .equiv  WREG13L, _WREG13
        .equiv  WREG13H, _WREG13+1

        .extern WREG14
        .equiv  WREG14L, _WREG14
        .equiv  WREG14H, _WREG14+1

        .extern WREG15
        .equiv  WREG15L, _WREG15
        .equiv  WREG15H, _WREG15+1

        .extern SPLIM
        .equiv  SPLIML, _SPLIM
        .equiv  SPLIMH, _SPLIM+1

        .extern PCL
        .equiv  PCLL, _PCL
        .equiv  PCLH, _PCL+1

        .extern PCH
        .equiv  PCHL, _PCH
        .equiv  PCHH, _PCH+1

        .extern TBLPAG
        .equiv  TBLPAGL, _TBLPAG
        .equiv  TBLPAGH, _TBLPAG+1

        .extern PSVPAG
        .equiv  PSVPAGL, _PSVPAG
        .equiv  PSVPAGH, _PSVPAG+1

        .extern RCOUNT
        .equiv  RCOUNTL, _RCOUNT
        .equiv  RCOUNTH, _RCOUNT+1

        .extern SR
        .equiv  SRL, _SR
        .equiv  SRH, _SR+1

        .extern CORCON
        .equiv  CORCONL, _CORCON
        .equiv  CORCONH, _CORCON+1

        .extern DISICNT
        .equiv  DISICNTL, _DISICNT
        .equiv  DISICNTH, _DISICNT+1

        .extern CNEN1
        .equiv  CNEN1L, _CNEN1
        .equiv  CNEN1H, _CNEN1+1

        .extern CNEN2
        .equiv  CNEN2L, _CNEN2
        .equiv  CNEN2H, _CNEN2+1

        .extern CNPU1
        .equiv  CNPU1L, _CNPU1
        .equiv  CNPU1H, _CNPU1+1

        .extern CNPU2
        .equiv  CNPU2L, _CNPU2
        .equiv  CNPU2H, _CNPU2+1

        .extern CNPD1
        .equiv  CNPD1L, _CNPD1
        .equiv  CNPD1H, _CNPD1+1

        .extern CNPD2
        .equiv  CNPD2L, _CNPD2
        .equiv  CNPD2H, _CNPD2+1

        .extern INTCON1
        .equiv  INTCON1L, _INTCON1
        .equiv  INTCON1H, _INTCON1+1

        .extern INTCON2
        .equiv  INTCON2L, _INTCON2
        .equiv  INTCON2H, _INTCON2+1

        .extern IFS0
        .equiv  IFS0L, _IFS0
        .equiv  IFS0H, _IFS0+1

        .extern IFS1
        .equiv  IFS1L, _IFS1
        .equiv  IFS1H, _IFS1+1

        .extern IFS3
        .equiv  IFS3L, _IFS3
        .equiv  IFS3H, _IFS3+1

        .extern IFS4
        .equiv  IFS4L, _IFS4
        .equiv  IFS4H, _IFS4+1

        .extern IEC0
        .equiv  IEC0L, _IEC0
        .equiv  IEC0H, _IEC0+1

        .extern IEC1
        .equiv  IEC1L, _IEC1
        .equiv  IEC1H, _IEC1+1

        .extern IEC3
        .equiv  IEC3L, _IEC3
        .equiv  IEC3H, _IEC3+1

        .extern IEC4
        .equiv  IEC4L, _IEC4
        .equiv  IEC4H, _IEC4+1

        .extern IPC0
        .equiv  IPC0L, _IPC0
        .equiv  IPC0H, _IPC0+1

        .extern IPC1
        .equiv  IPC1L, _IPC1
        .equiv  IPC1H, _IPC1+1

        .extern IPC2
        .equiv  IPC2L, _IPC2
        .equiv  IPC2H, _IPC2+1

        .extern IPC3
        .equiv  IPC3L, _IPC3
        .equiv  IPC3H, _IPC3+1

        .extern IPC4
        .equiv  IPC4L, _IPC4
        .equiv  IPC4H, _IPC4+1

        .extern IPC5
        .equiv  IPC5L, _IPC5
        .equiv  IPC5H, _IPC5+1

        .extern IPC7
        .equiv  IPC7L, _IPC7
        .equiv  IPC7H, _IPC7+1

        .extern IPC15
        .equiv  IPC15L, _IPC15
        .equiv  IPC15H, _IPC15+1

        .extern IPC16
        .equiv  IPC16L, _IPC16
        .equiv  IPC16H, _IPC16+1

        .extern IPC18
        .equiv  IPC18L, _IPC18
        .equiv  IPC18H, _IPC18+1

        .extern IPC19
        .equiv  IPC19L, _IPC19
        .equiv  IPC19H, _IPC19+1

        .extern INTTREG
        .equiv  INTTREGL, _INTTREG
        .equiv  INTTREGH, _INTTREG+1

        .extern TMR1
        .equiv  TMR1L, _TMR1
        .equiv  TMR1H, _TMR1+1

        .extern PR1
        .equiv  PR1L, _PR1
        .equiv  PR1H, _PR1+1

        .extern T1CON
        .equiv  T1CONL, _T1CON
        .equiv  T1CONH, _T1CON+1

        .extern TMR2
        .equiv  TMR2L, _TMR2
        .equiv  TMR2H, _TMR2+1

        .extern TMR3HLD
        .equiv  TMR3HLDL, _TMR3HLD
        .equiv  TMR3HLDH, _TMR3HLD+1

        .extern TMR3
        .equiv  TMR3L, _TMR3
        .equiv  TMR3H, _TMR3+1

        .extern PR2
        .equiv  PR2L, _PR2
        .equiv  PR2H, _PR2+1

        .extern PR3
        .equiv  PR3L, _PR3
        .equiv  PR3H, _PR3+1

        .extern T2CON
        .equiv  T2CONL, _T2CON
        .equiv  T2CONH, _T2CON+1

        .extern T3CON
        .equiv  T3CONL, _T3CON
        .equiv  T3CONH, _T3CON+1

        .extern IC1BUF
        .equiv  IC1BUFL, _IC1BUF
        .equiv  IC1BUFH, _IC1BUF+1

        .extern IC1CON
        .equiv  IC1CONL, _IC1CON
        .equiv  IC1CONH, _IC1CON+1

        .extern OC1RS
        .equiv  OC1RSL, _OC1RS
        .equiv  OC1RSH, _OC1RS+1

        .extern OC1R
        .equiv  OC1RL, _OC1R
        .equiv  OC1RH, _OC1R+1

        .extern OC1CON
        .equiv  OC1CONL, _OC1CON
        .equiv  OC1CONH, _OC1CON+1

        .extern I2C1RCV
        .equiv  I2C1RCVL, _I2C1RCV
        .equiv  I2C1RCVH, _I2C1RCV+1

        .extern I2C1TRN
        .equiv  I2C1TRNL, _I2C1TRN
        .equiv  I2C1TRNH, _I2C1TRN+1

        .extern I2C1BRG
        .equiv  I2C1BRGL, _I2C1BRG
        .equiv  I2C1BRGH, _I2C1BRG+1

        .extern I2C1CON
        .equiv  I2C1CONL, _I2C1CON
        .equiv  I2C1CONH, _I2C1CON+1

        .extern I2C1STAT
        .equiv  I2C1STATL, _I2C1STAT
        .equiv  I2C1STATH, _I2C1STAT+1

        .extern I2C1ADD
        .equiv  I2C1ADDL, _I2C1ADD
        .equiv  I2C1ADDH, _I2C1ADD+1

        .extern I2C1MSK
        .equiv  I2C1MSKL, _I2C1MSK
        .equiv  I2C1MSKH, _I2C1MSK+1

        .extern U1MODE
        .equiv  U1MODEL, _U1MODE
        .equiv  U1MODEH, _U1MODE+1

        .extern U1STA
        .equiv  U1STAL, _U1STA
        .equiv  U1STAH, _U1STA+1

        .extern U1TXREG
        .equiv  U1TXREGL, _U1TXREG
        .equiv  U1TXREGH, _U1TXREG+1

        .extern U1RXREG
        .equiv  U1RXREGL, _U1RXREG
        .equiv  U1RXREGH, _U1RXREG+1

        .extern U1BRG
        .equiv  U1BRGL, _U1BRG
        .equiv  U1BRGH, _U1BRG+1

        .extern U2MODE
        .equiv  U2MODEL, _U2MODE
        .equiv  U2MODEH, _U2MODE+1

        .extern U2STA
        .equiv  U2STAL, _U2STA
        .equiv  U2STAH, _U2STA+1

        .extern U2TXREG
        .equiv  U2TXREGL, _U2TXREG
        .equiv  U2TXREGH, _U2TXREG+1

        .extern U2RXREG
        .equiv  U2RXREGL, _U2RXREG
        .equiv  U2RXREGH, _U2RXREG+1

        .extern U2BRG
        .equiv  U2BRGL, _U2BRG
        .equiv  U2BRGH, _U2BRG+1

        .extern SPI1STAT
        .equiv  SPI1STATL, _SPI1STAT
        .equiv  SPI1STATH, _SPI1STAT+1

        .extern SPI1CON1
        .equiv  SPI1CON1L, _SPI1CON1
        .equiv  SPI1CON1H, _SPI1CON1+1

        .extern SPI1CON2
        .equiv  SPI1CON2L, _SPI1CON2
        .equiv  SPI1CON2H, _SPI1CON2+1

        .extern SPI1BUF
        .equiv  SPI1BUFL, _SPI1BUF
        .equiv  SPI1BUFH, _SPI1BUF+1

        .extern TRISA
        .equiv  TRISAL, _TRISA
        .equiv  TRISAH, _TRISA+1

        .extern PORTA
        .equiv  PORTAL, _PORTA
        .equiv  PORTAH, _PORTA+1

        .extern LATA
        .equiv  LATAL, _LATA
        .equiv  LATAH, _LATA+1

        .extern ODCA
        .equiv  ODCAL, _ODCA
        .equiv  ODCAH, _ODCA+1

        .extern TRISB
        .equiv  TRISBL, _TRISB
        .equiv  TRISBH, _TRISB+1

        .extern PORTB
        .equiv  PORTBL, _PORTB
        .equiv  PORTBH, _PORTB+1

        .extern LATB
        .equiv  LATBL, _LATB
        .equiv  LATBH, _LATB+1

        .extern ODCB
        .equiv  ODCBL, _ODCB
        .equiv  ODCBH, _ODCB+1

        .extern PADCFG1
        .equiv  PADCFG1L, _PADCFG1
        .equiv  PADCFG1H, _PADCFG1+1

        .extern ADC1BUF0
        .equiv  ADC1BUF0L, _ADC1BUF0
        .equiv  ADC1BUF0H, _ADC1BUF0+1

        .extern ADC1BUF1
        .equiv  ADC1BUF1L, _ADC1BUF1
        .equiv  ADC1BUF1H, _ADC1BUF1+1

        .extern ADC1BUF2
        .equiv  ADC1BUF2L, _ADC1BUF2
        .equiv  ADC1BUF2H, _ADC1BUF2+1

        .extern ADC1BUF3
        .equiv  ADC1BUF3L, _ADC1BUF3
        .equiv  ADC1BUF3H, _ADC1BUF3+1

        .extern ADC1BUF4
        .equiv  ADC1BUF4L, _ADC1BUF4
        .equiv  ADC1BUF4H, _ADC1BUF4+1

        .extern ADC1BUF5
        .equiv  ADC1BUF5L, _ADC1BUF5
        .equiv  ADC1BUF5H, _ADC1BUF5+1

        .extern ADC1BUF6
        .equiv  ADC1BUF6L, _ADC1BUF6
        .equiv  ADC1BUF6H, _ADC1BUF6+1

        .extern ADC1BUF7
        .equiv  ADC1BUF7L, _ADC1BUF7
        .equiv  ADC1BUF7H, _ADC1BUF7+1

        .extern ADC1BUF8
        .equiv  ADC1BUF8L, _ADC1BUF8
        .equiv  ADC1BUF8H, _ADC1BUF8+1

        .extern ADC1BUF9
        .equiv  ADC1BUF9L, _ADC1BUF9
        .equiv  ADC1BUF9H, _ADC1BUF9+1

        .extern ADC1BUFA
        .equiv  ADC1BUFAL, _ADC1BUFA
        .equiv  ADC1BUFAH, _ADC1BUFA+1

        .extern ADC1BUFB
        .equiv  ADC1BUFBL, _ADC1BUFB
        .equiv  ADC1BUFBH, _ADC1BUFB+1

        .extern ADC1BUFC
        .equiv  ADC1BUFCL, _ADC1BUFC
        .equiv  ADC1BUFCH, _ADC1BUFC+1

        .extern ADC1BUFD
        .equiv  ADC1BUFDL, _ADC1BUFD
        .equiv  ADC1BUFDH, _ADC1BUFD+1

        .extern ADC1BUFE
        .equiv  ADC1BUFEL, _ADC1BUFE
        .equiv  ADC1BUFEH, _ADC1BUFE+1

        .extern ADC1BUFF
        .equiv  ADC1BUFFL, _ADC1BUFF
        .equiv  ADC1BUFFH, _ADC1BUFF+1

        .extern AD1CON1
        .equiv  AD1CON1L, _AD1CON1
        .equiv  AD1CON1H, _AD1CON1+1

        .extern AD1CON2
        .equiv  AD1CON2L, _AD1CON2
        .equiv  AD1CON2H, _AD1CON2+1

        .extern AD1CON3
        .equiv  AD1CON3L, _AD1CON3
        .equiv  AD1CON3H, _AD1CON3+1

        .extern AD1CHS
        .equiv  AD1CHSL, _AD1CHS
        .equiv  AD1CHSH, _AD1CHS+1

        .extern AD1PCFG
        .equiv  AD1PCFGL, _AD1PCFG
        .equiv  AD1PCFGH, _AD1PCFG+1

        .extern AD1CSSL
        .equiv  AD1CSSLL, _AD1CSSL
        .equiv  AD1CSSLH, _AD1CSSL+1

        .extern CTMUCON
        .equiv  CTMUCONL, _CTMUCON
        .equiv  CTMUCONH, _CTMUCON+1

        .extern CTMUICON
        .equiv  CTMUICONL, _CTMUICON
        .equiv  CTMUICONH, _CTMUICON+1

        .extern ALRMVAL
        .equiv  ALRMVALL, _ALRMVAL
        .equiv  ALRMVALH, _ALRMVAL+1

        .extern ALCFGRPT
        .equiv  ALCFGRPTL, _ALCFGRPT
        .equiv  ALCFGRPTH, _ALCFGRPT+1

        .extern RTCVAL
        .equiv  RTCVALL, _RTCVAL
        .equiv  RTCVALH, _RTCVAL+1

        .extern RCFGCAL
        .equiv  RCFGCALL, _RCFGCAL
        .equiv  RCFGCALH, _RCFGCAL+1

        .extern CMSTAT
        .equiv  CMSTATL, _CMSTAT
        .equiv  CMSTATH, _CMSTAT+1

        .extern CVRCON
        .equiv  CVRCONL, _CVRCON
        .equiv  CVRCONH, _CVRCON+1

        .extern CM1CON
        .equiv  CM1CONL, _CM1CON
        .equiv  CM1CONH, _CM1CON+1

        .extern CM2CON
        .equiv  CM2CONL, _CM2CON
        .equiv  CM2CONH, _CM2CON+1

        .extern CRCCON
        .equiv  CRCCONL, _CRCCON
        .equiv  CRCCONH, _CRCCON+1

        .extern CRCXOR
        .equiv  CRCXORL, _CRCXOR
        .equiv  CRCXORH, _CRCXOR+1

        .extern CRCDAT
        .equiv  CRCDATL, _CRCDAT
        .equiv  CRCDATH, _CRCDAT+1

        .extern CRCWDAT
        .equiv  CRCWDATL, _CRCWDAT
        .equiv  CRCWDATH, _CRCWDAT+1

        .extern RCON
        .equiv  RCONL, _RCON
        .equiv  RCONH, _RCON+1

        .extern OSCCON

        .extern OSCCONL
        .equiv  OSCCONLL, _OSCCONL
        .equiv  OSCCONLH, _OSCCONL+1

        .extern OSCCONH
        .equiv  OSCCONHL, _OSCCONH
        .equiv  OSCCONHH, _OSCCONH+1

        .extern CLKDIV
        .equiv  CLKDIVL, _CLKDIV
        .equiv  CLKDIVH, _CLKDIV+1

        .extern OSCTUN
        .equiv  OSCTUNL, _OSCTUN
        .equiv  OSCTUNH, _OSCTUN+1

        .extern REFOCON
        .equiv  REFOCONL, _REFOCON
        .equiv  REFOCONH, _REFOCON+1

        .extern HLVDCON
        .equiv  HLVDCONL, _HLVDCON
        .equiv  HLVDCONH, _HLVDCON+1

        .extern DSCON
        .equiv  DSCONL, _DSCON
        .equiv  DSCONH, _DSCON+1

        .extern DSWAKE
        .equiv  DSWAKEL, _DSWAKE
        .equiv  DSWAKEH, _DSWAKE+1

        .extern DSWSRC
        .equiv  DSWSRCL, _DSWSRC
        .equiv  DSWSRCH, _DSWSRC+1

        .extern DSGPR0
        .equiv  DSGPR0L, _DSGPR0
        .equiv  DSGPR0H, _DSGPR0+1

        .extern DSGPR1
        .equiv  DSGPR1L, _DSGPR1
        .equiv  DSGPR1H, _DSGPR1+1

        .extern NVMCON
        .equiv  NVMCONL, _NVMCON
        .equiv  NVMCONH, _NVMCON+1

        .extern NVMKEY
        .equiv  NVMKEYL, _NVMKEY
        .equiv  NVMKEYH, _NVMKEY+1

        .extern PMD1
        .equiv  PMD1L, _PMD1
        .equiv  PMD1H, _PMD1+1

        .extern PMD2
        .equiv  PMD2L, _PMD2
        .equiv  PMD2H, _PMD2+1

        .extern PMD3
        .equiv  PMD3L, _PMD3
        .equiv  PMD3H, _PMD3+1

        .extern PMD4
        .equiv  PMD4L, _PMD4
        .equiv  PMD4H, _PMD4+1

;----- SR Bits -----------------------------------------------------
        .equiv C,       0x0000
        .equiv Z,       0x0001
        .equiv OV,      0x0002
        .equiv N,       0x0003
        .equiv RA,      0x0004
        .equiv DC,      0x0008

        .equiv IPL0,    0x0005
        .equiv IPL1,    0x0006
        .equiv IPL2,    0x0007


;----- CORCON Bits -----------------------------------------------------
        .equiv PSV,     0x0002
        .equiv IPL3,    0x0003


;----- CNEN1 Bits -----------------------------------------------------
        .equiv CN0IE,   0x0000
        .equiv CN1IE,   0x0001
        .equiv CN2IE,   0x0002
        .equiv CN3IE,   0x0003
        .equiv CN4IE,   0x0004
        .equiv CN5IE,   0x0005
        .equiv CN6IE,   0x0006
        .equiv CN8IE,   0x0008
        .equiv CN9IE,   0x0009
        .equiv CN11IE,  0x000B
        .equiv CN12IE,  0x000C
        .equiv CN13IE,  0x000D
        .equiv CN14IE,  0x000E


;----- CNEN2 Bits -----------------------------------------------------
        .equiv CN21IE,  0x0005
        .equiv CN22IE,  0x0006
        .equiv CN23IE,  0x0007
        .equiv CN29IE,  0x000D
        .equiv CN30IE,  0x000E


;----- CNPU1 Bits -----------------------------------------------------
        .equiv CN0PUE,  0x0000
        .equiv CN1PUE,  0x0001
        .equiv CN2PUE,  0x0002
        .equiv CN3PUE,  0x0003
        .equiv CN4PUE,  0x0004
        .equiv CN5PUE,  0x0005
        .equiv CN6PUE,  0x0006
        .equiv CN8PUE,  0x0008
        .equiv CN11PUE, 0x000B
        .equiv CN12PUE, 0x000C
        .equiv CN13PUE, 0x000D
        .equiv CN14PUE, 0x000E


;----- CNPU2 Bits -----------------------------------------------------
        .equiv CN21PUE, 0x0005
        .equiv CN22PUE, 0x0006
        .equiv CN23PUE, 0x0007
        .equiv CN29PUE, 0x000D
        .equiv CN30PUE, 0x000E


;----- CNPD1 Bits -----------------------------------------------------
        .equiv CN0PDE,  0x0000
        .equiv CN1PDE,  0x0001
        .equiv CN2PDE,  0x0002
        .equiv CN3PDE,  0x0003
        .equiv CN4PDE,  0x0004
        .equiv CN5PDE,  0x0005
        .equiv CN6PDE,  0x0006
        .equiv CN8PDE,  0x0008
        .equiv CN11PDE, 0x000B
        .equiv CN12PDE, 0x000C
        .equiv CN13PDE, 0x000D
        .equiv CN14PDE, 0x000E


;----- CNPD2 Bits -----------------------------------------------------
        .equiv CN21PDE, 0x0005
        .equiv CN22PDE, 0x0006
        .equiv CN23PDE, 0x0007
        .equiv CN29PDE, 0x000D
        .equiv CN30PDE, 0x000E


;----- INTCON1 Bits -----------------------------------------------------
        .equiv OSCFAIL, 0x0001
        .equiv STKERR,  0x0002
        .equiv ADDRERR, 0x0003
        .equiv MATHERR, 0x0004
        .equiv NSTDIS,  0x000F


;----- INTCON2 Bits -----------------------------------------------------
        .equiv INT0EP,  0x0000
        .equiv INT1EP,  0x0001
        .equiv INT2EP,  0x0002
        .equiv DISI,    0x000E
        .equiv ALTIVT,  0x000F


;----- IFS0 Bits -----------------------------------------------------
        .equiv INT0IF,  0x0000
        .equiv IC1IF,   0x0001
        .equiv OC1IF,   0x0002
        .equiv T1IF,    0x0003
        .equiv T2IF,    0x0007
        .equiv T3IF,    0x0008
        .equiv SPF1IF,  0x0009
        .equiv SPI1IF,  0x000A
        .equiv U1RXIF,  0x000B
        .equiv U1TXIF,  0x000C
        .equiv AD1IF,   0x000D
        .equiv NVMIF,   0x000F


;----- IFS1 Bits -----------------------------------------------------
        .equiv SI2C1IF, 0x0000
        .equiv MI2C1IF, 0x0001
        .equiv CMIF,    0x0002
        .equiv CNIF,    0x0003
        .equiv INT1IF,  0x0004
        .equiv INT2IF,  0x000D
        .equiv U2RXIF,  0x000E
        .equiv U2TXIF,  0x000F


;----- IFS3 Bits -----------------------------------------------------
        .equiv RTCIF,   0x000E


;----- IFS4 Bits -----------------------------------------------------
        .equiv U1ERIF,  0x0001
        .equiv U2ERIF,  0x0002
        .equiv CRCIF,   0x0003
        .equiv HLVDIF,  0x0008
        .equiv CTMUIF,  0x000D


;----- IEC0 Bits -----------------------------------------------------
        .equiv INT0IE,  0x0000
        .equiv IC1IE,   0x0001
        .equiv OC1IE,   0x0002
        .equiv T1IE,    0x0003
        .equiv T2IE,    0x0007
        .equiv T3IE,    0x0008
        .equiv SPF1IE,  0x0009
        .equiv SPI1IE,  0x000A
        .equiv U1RXIE,  0x000B
        .equiv U1TXIE,  0x000C
        .equiv AD1IE,   0x000D
        .equiv NVMIE,   0x000F


;----- IEC1 Bits -----------------------------------------------------
        .equiv SI2C1IE, 0x0000
        .equiv MI2C1IE, 0x0001
        .equiv CMIE,    0x0002
        .equiv CNIE,    0x0003
        .equiv INT1IE,  0x0004
        .equiv INT2IE,  0x000D
        .equiv U2RXIE,  0x000E
        .equiv U2TXIE,  0x000F


;----- IEC3 Bits -----------------------------------------------------
        .equiv RTCIE,   0x000E


;----- IEC4 Bits -----------------------------------------------------
        .equiv U1ERIE,  0x0001
        .equiv U2ERIE,  0x0002
        .equiv CRCIE,   0x0003
        .equiv HLVDIE,  0x0008
        .equiv CTMUIE,  0x000D


;----- IPC0 Bits -----------------------------------------------------
        .equiv INT0IP0, 0x0000
        .equiv INT0IP1, 0x0001
        .equiv INT0IP2, 0x0002
        .equiv IC1IP0,  0x0004
        .equiv IC1IP1,  0x0005
        .equiv IC1IP2,  0x0006
        .equiv OC1IP0,  0x0008
        .equiv OC1IP1,  0x0009
        .equiv OC1IP2,  0x000A
        .equiv T1IP0,   0x000C
        .equiv T1IP1,   0x000D
        .equiv T1IP2,   0x000E


;----- IPC1 Bits -----------------------------------------------------
        .equiv T2IP0,   0x000C
        .equiv T2IP1,   0x000D
        .equiv T2IP2,   0x000E


;----- IPC2 Bits -----------------------------------------------------
        .equiv T3IP0,   0x0000
        .equiv T3IP1,   0x0001
        .equiv T3IP2,   0x0002
        .equiv SPF1IP0, 0x0004
        .equiv SPF1IP1, 0x0005
        .equiv SPF1IP2, 0x0006
        .equiv SPI1IP0, 0x0008
        .equiv SPI1IP1, 0x0009
        .equiv SPI1IP2, 0x000A
        .equiv U1RXIP0, 0x000C
        .equiv U1RXIP1, 0x000D
        .equiv U1RXIP2, 0x000E


;----- IPC3 Bits -----------------------------------------------------
        .equiv U1TXIP0, 0x0000
        .equiv U1TXIP1, 0x0001
        .equiv U1TXIP2, 0x0002
        .equiv AD1IP0,  0x0004
        .equiv AD1IP1,  0x0005
        .equiv AD1IP2,  0x0006
        .equiv NVMIP0,  0x000C
        .equiv NVMIP1,  0x000D
        .equiv NVMIP2,  0x000E


;----- IPC4 Bits -----------------------------------------------------
        .equiv SI2C1P0, 0x0000
        .equiv SI2C1P1, 0x0001
        .equiv SI2C1P2, 0x0002
        .equiv MI2C1P0, 0x0004
        .equiv MI2C1P1, 0x0005
        .equiv MI2C1P2, 0x0006
        .equiv CMIP0,   0x0008
        .equiv CMIP1,   0x0009
        .equiv CMIP2,   0x000A
        .equiv CNIP0,   0x000C
        .equiv CNIP1,   0x000D
        .equiv CNIP2,   0x000E

        .equiv SI2C1IP0, 0x0000
        .equiv SI2C1IP1, 0x0001
        .equiv SI2C1IP2, 0x0002
        .equiv MI2C1IP0, 0x0004
        .equiv MI2C1IP1, 0x0005
        .equiv MI2C1IP2, 0x0006


;----- IPC5 Bits -----------------------------------------------------
        .equiv INT1IP0, 0x0000
        .equiv INT1IP1, 0x0001
        .equiv INT1IP2, 0x0002


;----- IPC7 Bits -----------------------------------------------------
        .equiv INT2IP0, 0x0004
        .equiv INT2IP1, 0x0005
        .equiv INT2IP2, 0x0006
        .equiv U2RXIP0, 0x0008
        .equiv U2RXIP1, 0x0009
        .equiv U2RXIP2, 0x000A
        .equiv U2TXIP0, 0x000C
        .equiv U2TXIP1, 0x000D
        .equiv U2TXIP2, 0x000E


;----- IPC15 Bits -----------------------------------------------------
        .equiv RTCIP0,  0x0008
        .equiv RTCIP1,  0x0009
        .equiv RTCIP2,  0x000A


;----- IPC16 Bits -----------------------------------------------------
        .equiv U1ERIP0, 0x0004
        .equiv U1ERIP1, 0x0005
        .equiv U1ERIP2, 0x0006
        .equiv U2ERIP0, 0x0008
        .equiv U2ERIP1, 0x0009
        .equiv U2ERIP2, 0x000A
        .equiv CRCIP0,  0x000C
        .equiv CRCIP1,  0x000D
        .equiv CRCIP2,  0x000E


;----- IPC18 Bits -----------------------------------------------------
        .equiv HLVDIP0, 0x0000
        .equiv HLVDIP1, 0x0001
        .equiv HLVDIP2, 0x0002


;----- IPC19 Bits -----------------------------------------------------
        .equiv CTMUIP0, 0x0004
        .equiv CTMUIP1, 0x0005
        .equiv CTMUIP2, 0x0006


;----- INTTREG Bits -----------------------------------------------------
        .equiv VHOLD,   0x000D
        .equiv CPUIRQ,  0x000F

        .equiv VECNUM0, 0x0000
        .equiv VECNUM1, 0x0001
        .equiv VECNUM2, 0x0002
        .equiv VECNUM3, 0x0003
        .equiv VECNUM4, 0x0004
        .equiv VECNUM5, 0x0005
        .equiv VECNUM6, 0x0006
        .equiv ILR0,    0x0008
        .equiv ILR1,    0x0009
        .equiv ILR2,    0x000A
        .equiv ILR3,    0x000B


;----- T1CON Bits -----------------------------------------------------
        .equiv TCS,     0x0001
        .equiv TSYNC,   0x0002
        .equiv TGATE,   0x0006
        .equiv TSIDL,   0x000D
        .equiv TON,     0x000F

        .equiv TCKPS0,  0x0004
        .equiv TCKPS1,  0x0005


;----- T2CON Bits -----------------------------------------------------
       ;.equiv TCS,     0x0001
        .equiv T32,     0x0003
       ;.equiv TGATE,   0x0006
       ;.equiv TSIDL,   0x000D
       ;.equiv TON,     0x000F

       ;.equiv TCKPS0,  0x0004
       ;.equiv TCKPS1,  0x0005


;----- T3CON Bits -----------------------------------------------------
       ;.equiv TCS,     0x0001
       ;.equiv TGATE,   0x0006
       ;.equiv TSIDL,   0x000D
       ;.equiv TON,     0x000F

       ;.equiv TCKPS0,  0x0004
       ;.equiv TCKPS1,  0x0005


;----- IC1CON Bits -----------------------------------------------------
        .equiv ICBNE,   0x0003
        .equiv ICOV,    0x0004
        .equiv ICTMR,   0x0007
        .equiv ICSIDL,  0x000D

        .equiv ICM0,    0x0000
        .equiv ICM1,    0x0001
        .equiv ICM2,    0x0002
        .equiv ICI0,    0x0005
        .equiv ICI1,    0x0006


;----- OC1CON Bits -----------------------------------------------------
        .equiv OCTSEL,  0x0003
        .equiv OCFLT,   0x0004
        .equiv OCSIDL,  0x000D

        .equiv OCM0,    0x0000
        .equiv OCM1,    0x0001
        .equiv OCM2,    0x0002


;----- I2C1CON Bits -----------------------------------------------------
        .equiv SEN,     0x0000
        .equiv RSEN,    0x0001
        .equiv PEN,     0x0002
        .equiv RCEN,    0x0003
        .equiv ACKEN,   0x0004
        .equiv ACKDT,   0x0005
        .equiv STREN,   0x0006
        .equiv GCEN,    0x0007
        .equiv SMEN,    0x0008
        .equiv DISSLW,  0x0009
        .equiv A10M,    0x000A
        .equiv IPMIEN,  0x000B
        .equiv SCLREL,  0x000C
        .equiv I2CSIDL, 0x000D
        .equiv I2CEN,   0x000F


;----- I2C1STAT Bits -----------------------------------------------------
        .equiv TBF,     0x0000
        .equiv RBF,     0x0001
        .equiv R_NOT_W, 0x0002
        .equiv S,       0x0003
        .equiv P,       0x0004
        .equiv D_NOT_A, 0x0005
        .equiv I2COV,   0x0006
        .equiv IWCOL,   0x0007
        .equiv ADD10,   0x0008
        .equiv GCSTAT,  0x0009
        .equiv BCL,     0x000A
        .equiv TRSTAT,  0x000E
        .equiv ACKSTAT, 0x000F

        .equiv R_W,     0x0002
        .equiv D_A,     0x0005


;----- I2C1MSK Bits -----------------------------------------------------
        .equiv AMSK0,   0x0000
        .equiv AMSK1,   0x0001
        .equiv AMSK2,   0x0002
        .equiv AMSK3,   0x0003
        .equiv AMSK4,   0x0004
        .equiv AMSK5,   0x0005
        .equiv AMSK6,   0x0006
        .equiv AMSK7,   0x0007
        .equiv AMSK8,   0x0008
        .equiv AMSK9,   0x0009


;----- U1MODE Bits -----------------------------------------------------
        .equiv STSEL,   0x0000
        .equiv BRGH,    0x0003
        .equiv RXINV,   0x0004
        .equiv ABAUD,   0x0005
        .equiv LPBACK,  0x0006
        .equiv WAKE,    0x0007
        .equiv RTSMD,   0x000B
        .equiv IREN,    0x000C
        .equiv USIDL,   0x000D
        .equiv UARTEN,  0x000F

        .equiv PDSEL0,  0x0001
        .equiv PDSEL1,  0x0002
        .equiv UEN0,    0x0008
        .equiv UEN1,    0x0009


;----- U1STA Bits -----------------------------------------------------
        .equiv URXDA,   0x0000
        .equiv OERR,    0x0001
        .equiv FERR,    0x0002
        .equiv PERR,    0x0003
        .equiv RIDLE,   0x0004
        .equiv ADDEN,   0x0005
        .equiv TRMT,    0x0008
        .equiv UTXBF,   0x0009
        .equiv UTXEN,   0x000A
        .equiv UTXBRK,  0x000B
        .equiv UTXISEL0, 0x000D
        .equiv UTXINV,  0x000E
        .equiv UTXISEL1, 0x000F

        .equiv URXISEL0, 0x0006
        .equiv URXISEL1, 0x0007


;----- U2MODE Bits -----------------------------------------------------
       ;.equiv STSEL,   0x0000
       ;.equiv BRGH,    0x0003
       ;.equiv RXINV,   0x0004
       ;.equiv ABAUD,   0x0005
       ;.equiv LPBACK,  0x0006
       ;.equiv WAKE,    0x0007
       ;.equiv RTSMD,   0x000B
       ;.equiv IREN,    0x000C
       ;.equiv USIDL,   0x000D
       ;.equiv UARTEN,  0x000F

       ;.equiv PDSEL0,  0x0001
       ;.equiv PDSEL1,  0x0002
       ;.equiv UEN0,    0x0008
       ;.equiv UEN1,    0x0009


;----- U2STA Bits -----------------------------------------------------
       ;.equiv URXDA,   0x0000
       ;.equiv OERR,    0x0001
       ;.equiv FERR,    0x0002
       ;.equiv PERR,    0x0003
       ;.equiv RIDLE,   0x0004
       ;.equiv ADDEN,   0x0005
       ;.equiv TRMT,    0x0008
       ;.equiv UTXBF,   0x0009
       ;.equiv UTXEN,   0x000A
       ;.equiv UTXBRK,  0x000B
       ;.equiv UTXISEL0, 0x000D
       ;.equiv UTXINV,  0x000E
       ;.equiv UTXISEL1, 0x000F

       ;.equiv URXISEL0, 0x0006
       ;.equiv URXISEL1, 0x0007


;----- SPI1STAT Bits -----------------------------------------------------
        .equiv SPIRBF,  0x0000
        .equiv SPITBF,  0x0001
        .equiv SR1MPT,  0x0005
        .equiv SPIROV,  0x0006
        .equiv SRMPT,   0x0007
        .equiv SPISIDL, 0x000D
        .equiv SPIEN,   0x000F

        .equiv SISEL0,  0x0002
        .equiv SISEL1,  0x0003
        .equiv SISEL2,  0x0004
        .equiv SPIBEC0, 0x0008
        .equiv SPIBEC1, 0x0009
        .equiv SPIBEC2, 0x000A


;----- SPI1CON1 Bits -----------------------------------------------------
        .equiv MSTEN,   0x0005
        .equiv CKP,     0x0006
        .equiv SSEN,    0x0007
        .equiv CKE,     0x0008
        .equiv SMP,     0x0009
        .equiv MODE16,  0x000A
        .equiv DISSDO,  0x000B
        .equiv DISSCK,  0x000C

        .equiv PPRE0,   0x0000
        .equiv PPRE1,   0x0001
        .equiv SPRE0,   0x0002
        .equiv SPRE1,   0x0003
        .equiv SPRE2,   0x0004


;----- SPI1CON2 Bits -----------------------------------------------------
        .equiv SPIBEN,  0x0000
        .equiv SPIFE,   0x0001
        .equiv SPIFPOL, 0x000D
        .equiv SPIFSD,  0x000E
        .equiv FRMEN,   0x000F


;----- TRISA Bits -----------------------------------------------------
        .equiv TRISA0,  0x0000
        .equiv TRISA1,  0x0001
        .equiv TRISA2,  0x0002
        .equiv TRISA3,  0x0003
        .equiv TRISA4,  0x0004
        .equiv TRISA5,  0x0005
        .equiv TRISA6,  0x0006


;----- PORTA Bits -----------------------------------------------------
        .equiv RA0,     0x0000
        .equiv RA1,     0x0001
        .equiv RA2,     0x0002
        .equiv RA3,     0x0003
        .equiv RA4,     0x0004
        .equiv RA5,     0x0005
        .equiv RA6,     0x0006


;----- LATA Bits -----------------------------------------------------
        .equiv LATA0,   0x0000
        .equiv LATA1,   0x0001
        .equiv LATA2,   0x0002
        .equiv LATA3,   0x0003
        .equiv LATA4,   0x0004
        .equiv LATA5,   0x0005
        .equiv LATA6,   0x0006


;----- ODCA Bits -----------------------------------------------------
        .equiv ODA0,    0x0000
        .equiv ODA1,    0x0001
        .equiv ODA2,    0x0002
        .equiv ODA3,    0x0003
        .equiv ODA4,    0x0004
        .equiv ODA5,    0x0005
        .equiv ODA6,    0x0006


;----- TRISB Bits -----------------------------------------------------
        .equiv TRISB0,  0x0000
        .equiv TRISB1,  0x0001
        .equiv TRISB2,  0x0002
        .equiv TRISB4,  0x0004
        .equiv TRISB7,  0x0007
        .equiv TRISB8,  0x0008
        .equiv TRISB9,  0x0009
        .equiv TRISB12, 0x000C
        .equiv TRISB13, 0x000D
        .equiv TRISB14, 0x000E
        .equiv TRISB15, 0x000F


;----- PORTB Bits -----------------------------------------------------
        .equiv RB0,     0x0000
        .equiv RB1,     0x0001
        .equiv RB2,     0x0002
        .equiv RB4,     0x0004
        .equiv RB7,     0x0007
        .equiv RB8,     0x0008
        .equiv RB9,     0x0009
        .equiv RB12,    0x000C
        .equiv RB13,    0x000D
        .equiv RB14,    0x000E
        .equiv RB15,    0x000F


;----- LATB Bits -----------------------------------------------------
        .equiv LATB0,   0x0000
        .equiv LATB1,   0x0001
        .equiv LATB2,   0x0002
        .equiv LATB4,   0x0004
        .equiv LATB7,   0x0007
        .equiv LATB8,   0x0008
        .equiv LATB9,   0x0009
        .equiv LATB12,  0x000C
        .equiv LATB13,  0x000D
        .equiv LATB14,  0x000E
        .equiv LATB15,  0x000F


;----- ODCB Bits -----------------------------------------------------
        .equiv ODB0,    0x0000
        .equiv ODB1,    0x0001
        .equiv ODB2,    0x0002
        .equiv ODB4,    0x0004
        .equiv ODB7,    0x0007
        .equiv ODB8,    0x0008
        .equiv ODB9,    0x0009
        .equiv ODB12,   0x000C
        .equiv ODB13,   0x000D
        .equiv ODB14,   0x000E
        .equiv ODB15,   0x000F


;----- PADCFG1 Bits -----------------------------------------------------
        .equiv OC1TRIS, 0x0003
        .equiv SMBUSDEL, 0x0004

        .equiv RTSECSEL0, 0x0001
        .equiv RTSECSEL1, 0x0002


;----- AD1CON1 Bits -----------------------------------------------------
        .equiv DONE,    0x0000
        .equiv SAMP,    0x0001
        .equiv ASAM,    0x0002
        .equiv ADSIDL,  0x000D
        .equiv ADON,    0x000F

        .equiv SSRC0,   0x0005
        .equiv SSRC1,   0x0006
        .equiv SSRC2,   0x0007
        .equiv FORM0,   0x0008
        .equiv FORM1,   0x0009


;----- AD1CON2 Bits -----------------------------------------------------
        .equiv ALTS,    0x0000
        .equiv BUFM,    0x0001
        .equiv BUFS,    0x0007
        .equiv CSCNA,   0x000A
        .equiv OFFCAL,  0x000C

        .equiv SMPI0,   0x0002
        .equiv SMPI1,   0x0003
        .equiv SMPI2,   0x0004
        .equiv SMPI3,   0x0005
        .equiv VCFG0,   0x000D
        .equiv VCFG1,   0x000E
        .equiv VCFG2,   0x000F


;----- AD1CON3 Bits -----------------------------------------------------
        .equiv ADRC,    0x000F

        .equiv ADCS0,   0x0000
        .equiv ADCS1,   0x0001
        .equiv ADCS2,   0x0002
        .equiv ADCS3,   0x0003
        .equiv ADCS4,   0x0004
        .equiv ADCS5,   0x0005
        .equiv SAMC0,   0x0008
        .equiv SAMC1,   0x0009
        .equiv SAMC2,   0x000A
        .equiv SAMC3,   0x000B
        .equiv SAMC4,   0x000C


;----- AD1CHS Bits -----------------------------------------------------
        .equiv CH0NA,   0x0007
        .equiv CH0NB,   0x000F

        .equiv CH0SA0,  0x0000
        .equiv CH0SA1,  0x0001
        .equiv CH0SA2,  0x0002
        .equiv CH0SA3,  0x0003
        .equiv CH0SB0,  0x0008
        .equiv CH0SB1,  0x0009
        .equiv CH0SB2,  0x000A
        .equiv CH0SB3,  0x000B


;----- AD1PCFG Bits -----------------------------------------------------
        .equiv PCFG0,   0x0000
        .equiv PCFG1,   0x0001
        .equiv PCFG2,   0x0002
        .equiv PCFG3,   0x0003
        .equiv PCFG4,   0x0004
        .equiv PCFG5,   0x0005
        .equiv PCFG10,  0x000A
        .equiv PCFG11,  0x000B
        .equiv PCFG12,  0x000C
        .equiv PCFG14,  0x000E
        .equiv PCFG15,  0x000F


;----- AD1CSSL Bits -----------------------------------------------------
        .equiv CSSL0,   0x0000
        .equiv CSSL1,   0x0001
        .equiv CSSL2,   0x0002
        .equiv CSSL3,   0x0003
        .equiv CSSL4,   0x0004
        .equiv CSSL5,   0x0005
        .equiv CSSL10,  0x000A
        .equiv CSSL11,  0x000B
        .equiv CSSL12,  0x000C


;----- CTMUCON Bits -----------------------------------------------------
        .equiv EDG1STAT, 0x0000
        .equiv EDG2STAT, 0x0001
        .equiv EDG1POL, 0x0004
        .equiv EDG2POL, 0x0007
        .equiv CTTRIG,  0x0008
        .equiv IDISSEN, 0x0009
        .equiv EDGSEQEN, 0x000A
        .equiv EDGEN,   0x000B
        .equiv TGEN,    0x000C
        .equiv CTMUSIDL, 0x000D
        .equiv CTMUEN,  0x000F

        .equiv EDG1SEL0, 0x0002
        .equiv EDG1SEL1, 0x0003
        .equiv EDG2SEL0, 0x0005
        .equiv EDG2SEL1, 0x0006


;----- CTMUICON Bits -----------------------------------------------------
        .equiv IRNG0,   0x0008
        .equiv IRNG1,   0x0009
        .equiv ITRIM0,  0x000A
        .equiv ITRIM1,  0x000B
        .equiv ITRIM2,  0x000C
        .equiv ITRIM3,  0x000D
        .equiv ITRIM4,  0x000E
        .equiv ITRIM5,  0x000F


;----- ALCFGRPT Bits -----------------------------------------------------
        .equiv CHIME,   0x000E
        .equiv ALRMEN,  0x000F

        .equiv ARPT0,   0x0000
        .equiv ARPT1,   0x0001
        .equiv ARPT2,   0x0002
        .equiv ARPT3,   0x0003
        .equiv ARPT4,   0x0004
        .equiv ARPT5,   0x0005
        .equiv ARPT6,   0x0006
        .equiv ARPT7,   0x0007
        .equiv ALRMPTR0, 0x0008
        .equiv ALRMPTR1, 0x0009
        .equiv AMASK0,  0x000A
        .equiv AMASK1,  0x000B
        .equiv AMASK2,  0x000C
        .equiv AMASK3,  0x000D


;----- RCFGCAL Bits -----------------------------------------------------
        .equiv RTCOE,   0x000A
        .equiv HALFSEC, 0x000B
        .equiv RTCSYNC, 0x000C
        .equiv RTCWREN, 0x000D
        .equiv RTCEN,   0x000F

        .equiv CAL0,    0x0000
        .equiv CAL1,    0x0001
        .equiv CAL2,    0x0002
        .equiv CAL3,    0x0003
        .equiv CAL4,    0x0004
        .equiv CAL5,    0x0005
        .equiv CAL6,    0x0006
        .equiv CAL7,    0x0007
        .equiv RTCPTR0, 0x0008
        .equiv RTCPTR1, 0x0009


;----- CMSTAT Bits -----------------------------------------------------
        .equiv C1OUT,   0x0000
        .equiv C2OUT,   0x0001
        .equiv C1EVT,   0x0008
        .equiv C2EVT,   0x0009
        .equiv CMIDL,   0x000F


;----- CVRCON Bits -----------------------------------------------------
        .equiv CVRSS,   0x0004
        .equiv CVRR,    0x0005
        .equiv CVROE,   0x0006
        .equiv CVREN,   0x0007

        .equiv CVR0,    0x0000
        .equiv CVR1,    0x0001
        .equiv CVR2,    0x0002
        .equiv CVR3,    0x0003


;----- CM1CON Bits -----------------------------------------------------
        .equiv CREF,    0x0004
        .equiv COUT,    0x0008
        .equiv CEVT,    0x0009
        .equiv CLPWR,   0x000C
        .equiv CPOL,    0x000D
        .equiv COE,     0x000E
        .equiv CON,     0x000F

        .equiv CCH0,    0x0000
        .equiv CCH1,    0x0001
        .equiv EVPOL0,  0x0006
        .equiv EVPOL1,  0x0007


;----- CM2CON Bits -----------------------------------------------------
       ;.equiv CREF,    0x0004
       ;.equiv COUT,    0x0008
       ;.equiv CEVT,    0x0009
       ;.equiv CLPWR,   0x000C
       ;.equiv CPOL,    0x000D
       ;.equiv COE,     0x000E
       ;.equiv CON,     0x000F

       ;.equiv CCH0,    0x0000
       ;.equiv CCH1,    0x0001
       ;.equiv EVPOL0,  0x0006
       ;.equiv EVPOL1,  0x0007


;----- CRCCON Bits -----------------------------------------------------
        .equiv CRCGO,   0x0004
        .equiv CRCMPT,  0x0006
        .equiv CRCFUL,  0x0007
        .equiv CSIDL,   0x000D

        .equiv PLEN0,   0x0000
        .equiv PLEN1,   0x0001
        .equiv PLEN2,   0x0002
        .equiv PLEN3,   0x0003
        .equiv VWORD0,  0x0008
        .equiv VWORD1,  0x0009
        .equiv VWORD2,  0x000A
        .equiv VWORD3,  0x000B
        .equiv VWORD4,  0x000C


;----- RCON Bits -----------------------------------------------------
        .equiv POR,     0x0000
        .equiv BOR,     0x0001
        .equiv IDLE,    0x0002
        .equiv SLEEP,   0x0003
        .equiv WDTO,    0x0004
        .equiv SWDTEN,  0x0005
        .equiv SWR,     0x0006
        .equiv EXTR,    0x0007
        .equiv VREGS,   0x0008
        .equiv DPSLP,   0x000A
        .equiv SBOREN,  0x000D
        .equiv IOPUWR,  0x000E
        .equiv TRAPR,   0x000F

        .equiv PMSLP,   0x0008


;----- OSCCON Bits -----------------------------------------------------
        .equiv OSWEN,   0x0000
        .equiv SOSCEN,  0x0001
        .equiv CF,      0x0003
        .equiv LOCK,    0x0005
        .equiv CLKLOCK, 0x0007

        .equiv NOSC0,   0x0008
        .equiv NOSC1,   0x0009
        .equiv NOSC2,   0x000A
        .equiv COSC0,   0x000C
        .equiv COSC1,   0x000D
        .equiv COSC2,   0x000E


;----- CLKDIV Bits -----------------------------------------------------
        .equiv DOZEN,   0x000B
        .equiv ROI,     0x000F

        .equiv RCDIV0,  0x0008
        .equiv RCDIV1,  0x0009
        .equiv RCDIV2,  0x000A
        .equiv DOZE0,   0x000C
        .equiv DOZE1,   0x000D
        .equiv DOZE2,   0x000E


;----- OSCTUN Bits -----------------------------------------------------
        .equiv TUN0,    0x0000
        .equiv TUN1,    0x0001
        .equiv TUN2,    0x0002
        .equiv TUN3,    0x0003
        .equiv TUN4,    0x0004
        .equiv TUN5,    0x0005


;----- REFOCON Bits -----------------------------------------------------
        .equiv ROSEL,   0x000C
        .equiv ROSSLP,  0x000D
        .equiv ROEN,    0x000F

        .equiv RODIV0,  0x0008
        .equiv RODIV1,  0x0009
        .equiv RODIV2,  0x000A
        .equiv RODIV3,  0x000B


;----- HLVDCON Bits -----------------------------------------------------
        .equiv IRVST,   0x0005
        .equiv BGVST,   0x0006
        .equiv VDIR,    0x0007
        .equiv HLSIDL,  0x000D
        .equiv HLVDEN,  0x000F

        .equiv HLVDL0,  0x0000
        .equiv HLVDL1,  0x0001
        .equiv HLVDL2,  0x0002
        .equiv HLVDL3,  0x0003


;----- DSCON Bits -----------------------------------------------------
        .equiv RELEASE, 0x0000
        .equiv DSBOR,   0x0001
        .equiv DSEN,    0x000F


;----- DSWAKE Bits -----------------------------------------------------
        .equiv DSPOR,   0x0000
        .equiv DSMCLR,  0x0002
        .equiv DSRTCC,  0x0003
        .equiv DSWDT,   0x0004
        .equiv DSFLT,   0x0007
        .equiv DSINT0,  0x0008


;----- DSWSRC Bits -----------------------------------------------------
       ;.equiv DSPOR,   0x0000
       ;.equiv DSMCLR,  0x0002
       ;.equiv DSRTCC,  0x0003
       ;.equiv DSWDT,   0x0004
       ;.equiv DSFLT,   0x0007
       ;.equiv DSINT0,  0x0008


;----- NVMCON Bits -----------------------------------------------------
        .equiv ERASE,   0x0006
        .equiv PGMONLY, 0x000C
        .equiv WRERR,   0x000D
        .equiv WREN,    0x000E
        .equiv WR,      0x000F

        .equiv NVMOP0,  0x0000
        .equiv NVMOP1,  0x0001
        .equiv NVMOP2,  0x0002
        .equiv NVMOP3,  0x0003
        .equiv NVMOP4,  0x0004
        .equiv NVMOP5,  0x0005


;----- NVMKEY Bits -----------------------------------------------------
        .equiv NVMKEY0, 0x0000
        .equiv NVMKEY1, 0x0001
        .equiv NVMKEY2, 0x0002
        .equiv NVMKEY3, 0x0003
        .equiv NVMKEY4, 0x0004
        .equiv NVMKEY5, 0x0005
        .equiv NVMKEY6, 0x0006
        .equiv NVMKEY7, 0x0007


;----- PMD1 Bits -----------------------------------------------------
        .equiv ADC1MD,  0x0000
        .equiv SPI1MD,  0x0003
        .equiv U1MD,    0x0005
        .equiv U2MD,    0x0006
        .equiv I2C1MD,  0x0007
        .equiv T1MD,    0x000B
        .equiv T2MD,    0x000C
        .equiv T3MD,    0x000D


;----- PMD2 Bits -----------------------------------------------------
        .equiv OC1MD,   0x0000
        .equiv IC1MD,   0x0008


;----- PMD3 Bits -----------------------------------------------------
        .equiv CRCPMD,  0x0007
        .equiv RTCCMD,  0x0009
        .equiv CMPMD,   0x000A


;----- PMD4 Bits -----------------------------------------------------
        .equiv HLVDMD,  0x0001
        .equiv CTMUMD,  0x0002
        .equiv REFOMD,  0x0003
        .equiv EEMD,    0x0004




;  Some useful macros for PWRSAV instructions

   .equiv SLEEP_MODE, 0x0
   .equiv IDLE_MODE, 0x1


;==========================================================================
;
;   Configuration Words
;
;   Configuration words exist in Program Space and their locations are
;   defined in the device linker script. They can be set in source code
;   or in the MPLAB IDE. Each configuration word should be specified
;   only once (multiple settings may be combined.)
;
;==========================================================================
;
;  Setting configuration words using macros:
;
;  The following macro named 'config' can be used to set configuration words:
;
        .macro config REG, VALUE
        .pushsection \REG.sec, code
        .global \REG
\REG:   .pword \VALUE
        .popsection
        .endm
;
;  For example, to set CONFIG_REG using the macro above, the following line
;  can be pasted only at the beginning of the assembly source code,
;  immediately below the '.include' directive.
;
;        config __CONFIG_REG, SETTING_A & SETTING_B
;
;  Note that the 'config' macro takes two arguments: the first is the name
;  of a configuration word (in this case, __CONFIG_REG), the second argument
;  is a boolean expression that may include multiple settings.
;  The example above would enable SETTING_A and also SETTING_B.
;
;  A description of all possible settings for each configuration word
;  appears below.
;
;==========================================================================
;
;  Setting configuration words without using macros:
;
;  To set configuration words without using the 'config' macro,
;  use the following format:
;
;           .section __CONFIG_REG.sec, code
;           .global __CONFIG_REG
;__CONFIG_REG: .pword SETTING_A & SETTING_B
;
;  This source code is equivalent to the expanded macro from the previous
;  example.
;
;==========================================================================


;----- FBS (0xf80000) --------------------------------------------------
;
;  The following settings are available for FBS:
;
;   Table Write Protect Boot:
;     BWRP_ON              Boot segment is write-protected
;     BWRP_OFF             Boot segment may be written
;
;   Boot segment Protect:
;     BSS_HI2K             High security, boot program Flash segment starts at 200h, ends at 0015FEh
;     BSS_HI1K             High security boot program Flash segment starts at 200h, ends at 000AFEh
;     BSS_STD2K            Standard security, boot program Flash segment starts at 200h, ends at 0015FEh
;     BSS_STD1K            Standard security, boot program Flash segment starts at 200h, ends at 000AFEh
;     BSS_OFF              No boot program Flash segment
;
        .equiv BWRP_ON,         0xFFFE ; Boot segment is write-protected
        .equiv BWRP_OFF,        0xFFFF ; Boot segment may be written

        .equiv BSS_HI2K,        0xFFF3 ; High security, boot program Flash segment starts at 200h, ends at 0015FEh
        .equiv BSS_HI1K,        0xFFF5 ; High security boot program Flash segment starts at 200h, ends at 000AFEh
        .equiv BSS_STD2K,       0xFFFB ; Standard security, boot program Flash segment starts at 200h, ends at 0015FEh
        .equiv BSS_STD1K,       0xFFFD ; Standard security, boot program Flash segment starts at 200h, ends at 000AFEh
        .equiv BSS_OFF,         0xFFFF ; No boot program Flash segment


;----- FGS (0xf80004) --------------------------------------------------
;
;  The following settings are available for FGS:
;
;   General Segment Code Flash Write Protection bit:
;     GWRP_ON              General segment is write-protected
;     GWRP_OFF             General segment may be written
;
;   General Segment Code Flash Code Protection bit:
;     GCP_ON               Standard security enabled
;     GCP_OFF              No protection
;
        .equiv GWRP_ON,         0xFFFE ; General segment is write-protected
        .equiv GWRP_OFF,        0xFFFF ; General segment may be written

        .equiv GCP_ON,          0xFFFD ; Standard security enabled
        .equiv GCP_OFF,         0xFFFF ; No protection


;----- FOSCSEL (0xf80006) --------------------------------------------------
;
;  The following settings are available for FOSCSEL:
;
;   Oscillator Select:
;     FNOSC_FRC            Fast RC oscillator (FRC)
;     FNOSC_FRCPLL         Fast RC oscillator with divide-by-N with PLL module (FRCDIV+PLL)
;     FNOSC_PRI            Primary oscillator (XT, HS, EC)
;     FNOSC_PRIPLL         Primary oscillator with PLL module (HS+PLL, EC+PLL)
;     FNOSC_SOSC           Secondary oscillator (SOSC)
;     FNOSC_LPRC           Low-Power RC oscillator (LPRC)
;     FNOSC_LPFRC          500 kHz Low-Power FRC oscillator with divide-by-N (LPFRCDIV)
;     FNOSC_FRCDIV         8 MHz FRC oscillator with divide-by-N (FRCDIV)
;
;   Internal External Switch Over bit:
;     IESO_OFF             Internal External Switchover mode disabled (Two-Speed Start-up disabled)
;     IESO_ON              Internal External Switchover mode enabled (Two-Speed Start-up enabled)
;
        .equiv FNOSC_FRC,       0xFFF8 ; Fast RC oscillator (FRC)
        .equiv FNOSC_FRCPLL,    0xFFF9 ; Fast RC oscillator with divide-by-N with PLL module (FRCDIV+PLL)
        .equiv FNOSC_PRI,       0xFFFA ; Primary oscillator (XT, HS, EC)
        .equiv FNOSC_PRIPLL,    0xFFFB ; Primary oscillator with PLL module (HS+PLL, EC+PLL)
        .equiv FNOSC_SOSC,      0xFFFC ; Secondary oscillator (SOSC)
        .equiv FNOSC_LPRC,      0xFFFD ; Low-Power RC oscillator (LPRC)
        .equiv FNOSC_LPFRC,     0xFFFE ; 500 kHz Low-Power FRC oscillator with divide-by-N (LPFRCDIV)
        .equiv FNOSC_FRCDIV,    0xFFFF ; 8 MHz FRC oscillator with divide-by-N (FRCDIV)

        .equiv IESO_OFF,        0xFF7F ; Internal External Switchover mode disabled (Two-Speed Start-up disabled)
        .equiv IESO_ON,         0xFFFF ; Internal External Switchover mode enabled (Two-Speed Start-up enabled)


;----- FOSC (0xf80008) --------------------------------------------------
;
;  The following settings are available for FOSC:
;
;   Primary Oscillator Configuration bits:
;     POSCMOD_EC           External clock mode selected
;     POSCMOD_XT           XT oscillator mode selected
;     POSCMOD_HS           HS oscillator mode selected
;     POSCMOD_NONE         Primary oscillator disabled
;
;   CLKO Enable Configuration bit:
;     OSCIOFNC_ON          CLKO output disabled; pin functions as port I/O
;     OSCIOFNC_OFF         CLKO output signal is active on the OSCO pin
;
;   Primary Oscillator Frequency Range Configuration bits:
;     POSCFREQ_LS          Primary oscillator/external clock input frequency less than 100 kHz
;     POSCFREQ_MS          Primary oscillator/external clock input frequency between 100 kHz and 8 MHz
;     POSCFREQ_HS          Primary oscillator/external clock input frequency greater than 8 MHz
;
;   SOSC Power Selection Configuration bits:
;     SOSCSEL_SOSCLP       Secondary oscillator configured for low-power operation
;     SOSCSEL_SOSCHP       Secondary oscillator configured for high-power operation
;
;   Clock Switching and Monitor Selection:
;     FCKSM_CSECME         Both Clock Switching and Fail-safe Clock Monitor are enabled
;     FCKSM_CSECMD         Clock switching is enabled, Fail-Safe Clock Monitor is disabled
;     FCKSM_CSDCMD         Both Clock Switching and Fail-safe Clock Monitor are disabled
;
        .equiv POSCMOD_EC,      0xFFFC ; External clock mode selected
        .equiv POSCMOD_XT,      0xFFFD ; XT oscillator mode selected
        .equiv POSCMOD_HS,      0xFFFE ; HS oscillator mode selected
        .equiv POSCMOD_NONE,    0xFFFF ; Primary oscillator disabled

        .equiv OSCIOFNC_ON,     0xFFFB ; CLKO output disabled; pin functions as port I/O
        .equiv OSCIOFNC_OFF,    0xFFFF ; CLKO output signal is active on the OSCO pin

        .equiv POSCFREQ_LS,     0xFFEF ; Primary oscillator/external clock input frequency less than 100 kHz
        .equiv POSCFREQ_MS,     0xFFF7 ; Primary oscillator/external clock input frequency between 100 kHz and 8 MHz
        .equiv POSCFREQ_HS,     0xFFFF ; Primary oscillator/external clock input frequency greater than 8 MHz

        .equiv SOSCSEL_SOSCLP,  0xFFDF ; Secondary oscillator configured for low-power operation
        .equiv SOSCSEL_SOSCHP,  0xFFFF ; Secondary oscillator configured for high-power operation

        .equiv FCKSM_CSECME,    0xFF3F ; Both Clock Switching and Fail-safe Clock Monitor are enabled
        .equiv FCKSM_CSECMD,    0xFF7F ; Clock switching is enabled, Fail-Safe Clock Monitor is disabled
        .equiv FCKSM_CSDCMD,    0xFFBF ; Both Clock Switching and Fail-safe Clock Monitor are disabled


;----- FWDT (0xf8000a) --------------------------------------------------
;
;  The following settings are available for FWDT:
;
;   Watchdog Timer Postscale Select bits:
;     WDTPS_PS1            1:1
;     WDTPS_PS2            1:2
;     WDTPS_PS4            1:4
;     WDTPS_PS8            1:8
;     WDTPS_PS16           1:16
;     WDTPS_PS32           1:32
;     WDTPS_PS64           1:64
;     WDTPS_PS128          1:128
;     WDTPS_PS256          1:256
;     WDTPS_PS512          1:512
;     WDTPS_PS1024         1:1,024
;     WDTPS_PS2048         1:2,048
;     WDTPS_PS4096         1:4,096
;     WDTPS_PS8192         1:8,192
;     WDTPS_PS16384        1:16,384
;     WDTPS_PS32768        1:32,768
;
;   WDT Prescaler:
;     FWPSA_PR32           WDT prescaler ratio of 1:32
;     FWPSA_PR128          WDT prescaler ratio of 1:128
;
;   Windowed Watchdog Timer Disable bit:
;     WINDIS_ON            Windowed WDT enabled
;     WINDIS_OFF           Standard WDT selected; windowed WDT disabled
;
;   Watchdog Timer Enable bit:
;     FWDTEN_OFF           WDT disabled (control is placed on the SWDTEN bit)
;     FWDTEN_ON            WDT enabled
;
        .equiv WDTPS_PS1,       0xFFF0 ; 1:1
        .equiv WDTPS_PS2,       0xFFF1 ; 1:2
        .equiv WDTPS_PS4,       0xFFF2 ; 1:4
        .equiv WDTPS_PS8,       0xFFF3 ; 1:8
        .equiv WDTPS_PS16,      0xFFF4 ; 1:16
        .equiv WDTPS_PS32,      0xFFF5 ; 1:32
        .equiv WDTPS_PS64,      0xFFF6 ; 1:64
        .equiv WDTPS_PS128,     0xFFF7 ; 1:128
        .equiv WDTPS_PS256,     0xFFF8 ; 1:256
        .equiv WDTPS_PS512,     0xFFF9 ; 1:512
        .equiv WDTPS_PS1024,    0xFFFA ; 1:1,024
        .equiv WDTPS_PS2048,    0xFFFB ; 1:2,048
        .equiv WDTPS_PS4096,    0xFFFC ; 1:4,096
        .equiv WDTPS_PS8192,    0xFFFD ; 1:8,192
        .equiv WDTPS_PS16384,   0xFFFE ; 1:16,384
        .equiv WDTPS_PS32768,   0xFFFF ; 1:32,768

        .equiv FWPSA_PR32,      0xFFEF ; WDT prescaler ratio of 1:32
        .equiv FWPSA_PR128,     0xFFFF ; WDT prescaler ratio of 1:128

        .equiv WINDIS_ON,       0xFFBF ; Windowed WDT enabled
        .equiv WINDIS_OFF,      0xFFFF ; Standard WDT selected; windowed WDT disabled

        .equiv FWDTEN_OFF,      0xFF7F ; WDT disabled (control is placed on the SWDTEN bit)
        .equiv FWDTEN_ON,       0xFFFF ; WDT enabled


;----- FPOR (0xf8000c) --------------------------------------------------
;
;  The following settings are available for FPOR:
;
;   Brown-out Reset Enable bits:
;     BOREN_BOR0           Brown-out Reset disabled in hardware; SBOREN bit disabled
;     BOREN_BOR1           Brown-out Reset controlled with the SBOREN bit setting
;     BOREN_BOR2           Brown-out Reset enabled only while device is active and disabled in Sleep; SBOREN bit disabled
;     BOREN_BOR3           Brown-out Reset enabled in hardware; SBOREN bit disabled
;
;   Power-up Timer Enable bit:
;     PWRTEN_OFF           PWRT disabled
;     PWRTEN_ON            PWRT enabled
;
;   Alternate I2C1 Pin Mapping bit:
;     I2C1SEL_SEC          Alternate location for SCL1/SDA1 pins
;     I2C1SEL_PRI          Default location for SCL1/SDA1 pins
;
;   Brown-out Reset Voltage bits:
;     BORV_LPBOR           Low-power Brown-Out Reset occurs around 2.0V
;     BORV_V27             Brown-out Reset set to Highest Voltage (2.7V)
;     BORV_V20             Brown-out Reset set at 2.0V
;     BORV_V18             Brown-out Reset set to lowest voltage (1.8V)
;
;   MCLR Pin Enable bit:
;     MCLRE_OFF            RA5 input pin enabled; MCLR disabled
;     MCLRE_ON             MCLR pin enabled; RA5 input pin disabled
;
        .equiv BOREN_BOR0,      0xFFFC ; Brown-out Reset disabled in hardware; SBOREN bit disabled
        .equiv BOREN_BOR1,      0xFFFD ; Brown-out Reset controlled with the SBOREN bit setting
        .equiv BOREN_BOR2,      0xFFFE ; Brown-out Reset enabled only while device is active and disabled in Sleep; SBOREN bit disabled
        .equiv BOREN_BOR3,      0xFFFF ; Brown-out Reset enabled in hardware; SBOREN bit disabled

        .equiv PWRTEN_OFF,      0xFFF7 ; PWRT disabled
        .equiv PWRTEN_ON,       0xFFFF ; PWRT enabled

        .equiv I2C1SEL_SEC,     0xFFEF ; Alternate location for SCL1/SDA1 pins
        .equiv I2C1SEL_PRI,     0xFFFF ; Default location for SCL1/SDA1 pins

        .equiv BORV_LPBOR,      0xFF9F ; Low-power Brown-Out Reset occurs around 2.0V
        .equiv BORV_V27,        0xFFBF ; Brown-out Reset set to Highest Voltage (2.7V)
        .equiv BORV_V20,        0xFFDF ; Brown-out Reset set at 2.0V
        .equiv BORV_V18,        0xFFFF ; Brown-out Reset set to lowest voltage (1.8V)

        .equiv MCLRE_OFF,       0xFF7F ; RA5 input pin enabled; MCLR disabled
        .equiv MCLRE_ON,        0xFFFF ; MCLR pin enabled; RA5 input pin disabled


;----- FICD (0xf8000e) --------------------------------------------------
;
;  The following settings are available for FICD:
;
;   ICD Pin Placement Select bits:
;     ICS_PGx3             PGC3/PGD3 are used for programming and debugging the device
;     ICS_PGx2             PGC2/PGD2 are used for programming and debugging the device
;     ICS_PGx1             PGC1/PGD1 are used for programming and debugging the device
;
;   Set Clip On Emulation bit:
;     COE_ON               Device will reset into Clip On Emulation Mode
;     COE_OFF              Device will reset into Operational Mode
;
;   Background Debugger Enable bit:
;     BKBUG_ON             Background debugger functions enabled
;     BKBUG_OFF            Background debugger disabled
;
        .equiv ICS_PGx3,        0xFFFD ; PGC3/PGD3 are used for programming and debugging the device
        .equiv ICS_PGx2,        0xFFFE ; PGC2/PGD2 are used for programming and debugging the device
        .equiv ICS_PGx1,        0xFFFF ; PGC1/PGD1 are used for programming and debugging the device

        .equiv COE_ON,          0xFFFF ; Device will reset into Clip On Emulation Mode
        .equiv COE_OFF,         0xFFFF ; Device will reset into Operational Mode

        .equiv BKBUG_ON,        0xFFFF ; Background debugger functions enabled
        .equiv BKBUG_OFF,       0xFFFF ; Background debugger disabled


;----- FDS (0xf80010) --------------------------------------------------
;
;  The following settings are available for FDS:
;
;   Deep Sleep Watchdog Timer Postscale Select bits:
;     DSWDTPS_DSWDTPS0     1:2 (2.1 ms)
;     DSWDTPS_DSWDTPS1     1:8 (8.3 ms)
;     DSWDTPS_DSWDTPS2     1:32 (33 ms)
;     DSWDTPS_DSWDTPS3     1:128 (132 ms)
;     DSWDTPS_DSWDTPS4     1:512 (528 ms)
;     DSWDTPS_DSWDTPS5     1:2048 (2.1 Seconds)
;     DSWDTPS_DSWDTPS6     1:8192 (8.5 Seconds)
;     DSWDTPS_DSWDTPS7     1:32,768 (34 Seconds)
;     DSWDTPS_DSWDTPS8     1:131,072 (135 Seconds)
;     DSWDTPS_DSWDTPS9     1:524,288 (9 Minutes)
;     DSWDTPS_DSWDTPSA     1:2,097,152 (36 Minutes)
;     DSWDTPS_DSWDTPSB     1:8,388,608 (2.4 Hours)
;     DSWDTPS_DSWDTPSC     1:33,554,432 (9.6 Hours)
;     DSWDTPS_DSWDTPSD     1:134,217,728 (38.5 Hours)
;     DSWDTPS_DSWDTPSE     1:536,870,912 (6.4 Days)
;     DSWDTPS_DSWDTPSF     1:2,147,483,648 (25.7 Days)
;
;   DSWDT Reference Clock Select bit:
;     DSWDTOSC_SOSC        DSWDT uses SOSC as reference clock
;     DSWDTOSC_LPRC        DSWDT uses LPRC as reference clock
;
;   RTCC Reference Clock Select bit:
;     RTCOSC_LPRC          RTCC uses LPRC as reference clock
;     RTCOSC_SOSC          RTCC uses SOSC as reference clock
;
;   Deep Sleep Zero-Power BOR Enable bit:
;     DSBOREN_OFF          Deep Sleep BOR disabled in Deep Sleep
;     DSBOREN_ON           Deep Sleep BOR enabled in Deep Sleep
;
;   Deep Sleep Watchdog Timer Enable bit:
;     DSWDTEN_OFF          DSWDT disabled
;     DSWDTEN_ON           DSWDT enabled
;
        .equiv DSWDTPS_DSWDTPS0, 0xFFF0 ; 1:2 (2.1 ms)
        .equiv DSWDTPS_DSWDTPS1, 0xFFF1 ; 1:8 (8.3 ms)
        .equiv DSWDTPS_DSWDTPS2, 0xFFF2 ; 1:32 (33 ms)
        .equiv DSWDTPS_DSWDTPS3, 0xFFF3 ; 1:128 (132 ms)
        .equiv DSWDTPS_DSWDTPS4, 0xFFF4 ; 1:512 (528 ms)
        .equiv DSWDTPS_DSWDTPS5, 0xFFF5 ; 1:2048 (2.1 Seconds)
        .equiv DSWDTPS_DSWDTPS6, 0xFFF6 ; 1:8192 (8.5 Seconds)
        .equiv DSWDTPS_DSWDTPS7, 0xFFF7 ; 1:32,768 (34 Seconds)
        .equiv DSWDTPS_DSWDTPS8, 0xFFF8 ; 1:131,072 (135 Seconds)
        .equiv DSWDTPS_DSWDTPS9, 0xFFF9 ; 1:524,288 (9 Minutes)
        .equiv DSWDTPS_DSWDTPSA, 0xFFFA ; 1:2,097,152 (36 Minutes)
        .equiv DSWDTPS_DSWDTPSB, 0xFFFB ; 1:8,388,608 (2.4 Hours)
        .equiv DSWDTPS_DSWDTPSC, 0xFFFC ; 1:33,554,432 (9.6 Hours)
        .equiv DSWDTPS_DSWDTPSD, 0xFFFD ; 1:134,217,728 (38.5 Hours)
        .equiv DSWDTPS_DSWDTPSE, 0xFFFE ; 1:536,870,912 (6.4 Days)
        .equiv DSWDTPS_DSWDTPSF, 0xFFFF ; 1:2,147,483,648 (25.7 Days)

        .equiv DSWDTOSC_SOSC,   0xFFEF ; DSWDT uses SOSC as reference clock
        .equiv DSWDTOSC_LPRC,   0xFFFF ; DSWDT uses LPRC as reference clock

        .equiv RTCOSC_LPRC,     0xFFDF ; RTCC uses LPRC as reference clock
        .equiv RTCOSC_SOSC,     0xFFFF ; RTCC uses SOSC as reference clock

        .equiv DSBOREN_OFF,     0xFFBF ; Deep Sleep BOR disabled in Deep Sleep
        .equiv DSBOREN_ON,      0xFFFF ; Deep Sleep BOR enabled in Deep Sleep

        .equiv DSWDTEN_OFF,     0xFF7F ; DSWDT disabled
        .equiv DSWDTEN_ON,      0xFFFF ; DSWDT enabled


.LIST
