module library libgpsim_modules
node gsm_tx
node gsm_rx

module load usart repeater

repeater.txbaud = 115200
repeater.rxbaud = 115200
repeater.loop = true

attach gsm_tx repeater.TXPIN portb2
attach gsm_rx repeater.RXPIN portb5
run
