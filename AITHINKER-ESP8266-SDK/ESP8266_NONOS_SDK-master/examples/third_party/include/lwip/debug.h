et rx fifo trigger
        WRITE_PERI_REG(UART_CONF1(uart_no),
        ((100 & UART_RXFIFO_FULL_THRHD) << UART_RXFIFO_FULL_THRHD_S) |
        #if UART_HW_RTS
        ((110 & UART_RX_FLOW_THRHD) << UART_RX_FLOW_THRHD_S) |
        UART_RX_FLOW_EN |   //enbale rx flow control
        #endif
        (0x02 & UART_RX_TOUT_THRHD) << UART_RX_TOUT_THRHD_S |
        UART_RX_TOUT_EN|
        ((0x10 & UART_TXFIFO_EMPTY_THRHD)<<UART_TXFIFO_EMPTY_THRHD_S));//wjl 
        #if UART_HW_CTS
        SET_PERI_REG_MASK( UART_CONF0(uart_no),UART_TX_FLOW_EN);  //add this sentense to add a tx flow control via MTCK( CTS )
        #endif
        SET_PERI_REG_MASK(UART_INT_ENA(uart_no), UART_RXFIFO_TOUT_INT_ENA |UART_FRM_ERR_INT_ENA);
    }else{
        WRITE_PERI_REG(UART_CONF1(uart_no),((UartDev.rcv_buff.TrigLvl & UART_RXFIFO_FULL_THRHD) << UART_RXFIFO_FULL_THRHD_S));//TrigLvl default val == 1
    }
    //clear all interrupt
    WRITE_PERI_REG(UART_INT_CLR(uart_no), 0xffff);
    //enable rx_interrupt
    SET_PERI_REG_MASK(UART_INT_ENA(uart_no), UART_RXFIFO_FULL_INT_ENA|UART_RXFIFO_OVF_INT_ENA);
}

/******************************************************************************
 * FunctionName : uart1_tx_one_char
 * Description  : Internal used function
 *                Use uart1 interface to transfer one char
 * Parameters   : uint8 TxChar - character to tx
 * Returns      : OK
*******************************************************************************/
 STATUS uart_tx_one_char(uint8 uart, uint8 TxChar)
{
    while (true){
        uint32 fifo_cnt = READ_PERI_REG(UART_STATUS(uart)) & (UART_TXFIFO_CNT<<UART_TXFIFO_CNT_S);
        if ((fifo_cnt >> UART_TXFIFO_CNT_S & UART_TXFIFO_CNT) < 126) {
            break;
        }
    }
    WRITE_PERI_REG(UART_FIFO(uart) , TxChar);
    return OK;
}

/******************************************************************************
 * FunctionName : uart1_write_char
 * Description  : Internal used function
 *                Do some special deal while tx char is '\r' or '\n'
 * Parameters   : char c - character to tx
 * Returns      : NONE
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR
uart1_write_char(char c)
{
    if (c == '\n'){
        uart_tx_one_char(UART1, '\r');
        uart_tx_one_char(UART1, '\n');
    }else if (c == '\r'){
    
    }else{
        uart_tx_one_char(UART1, c);
    }
}

//os_printf output to fifo or to the tx buffer
LOCAL void ICACHE_FLASH_ATTR
uart0_write_char_no_wait(char c)
{
#if UART_BUFF_EN    //send to uart0 fifo but do not wait 
    uint8 chr;
    if (c == '\n'){
        chr = '\r';
        tx_buff_enq(&chr, 1);
        chr = '\n';
        tx_buff_enq(&chr, 1);
    }else if (c == '\r'){
    
    }else{
        tx_buff_enq(&c,1);
    }
#else //send to uart tx buffer
    if (c == '\n'){
        uart_tx_one_char_no_wait(UART0, '\r');
        uart_tx_one_char_no_wait(UART0, '\n');
    }else if (c == '\r'){
    
    }
    else{
        uart_tx_one_char_no_wait(UART0, c);
    }
#endif
}

/******************************************************************************
 * FunctionName : uart0_tx_buffer
 * Description  : use uart0 to transfer buffer
 * Parameters   : uint8 *buf - point to send buffer
 *                uint16 len - buffer len
 * Returns      :
*******************************************************************************/
void ICACHE_FLASH_ATTR
uart0_tx_buffer(uint8 *buf, uint16 len)
{
    uint16 i;
    for (i = 0; i < len; i++)
    {
        uart_tx_one_char(UART0, buf[i]);
    }
}

/******************************************************************************
 * FunctionName : uart0_sendStr
 * Description  : use uart0 to transfer buffer
 * Parameters   : uint8 *buf - point to send buffer
 *                uint16 len - buffer len
 * Returns      :
*******************************************************************************/
void ICACHE_FLASH_ATTR
uart0_sendStr(const char *str)
{
    while(*str){
        uart_tx_one_char(UART0, *str++);
    }
}
void at_port_print(const char *str) __attribute__((alias("uart0_sendStr")));
/*********************************