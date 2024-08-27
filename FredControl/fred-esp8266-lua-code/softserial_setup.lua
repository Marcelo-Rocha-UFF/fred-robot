-- Softserial setup

-- Create new software UART with baudrate of 9600, D2 as Tx pin and D3 as Rx pin
TxPIN = 2
RxPIN = 3
print("Configuring softserial port: Tx=" .. TxPIN .. " and Rx=" .. RxPIN)
s = nil
s = softuart.setup(9600, TxPIN, RxPIN)
-- Set callback to run when 10 characters show up in the buffer

