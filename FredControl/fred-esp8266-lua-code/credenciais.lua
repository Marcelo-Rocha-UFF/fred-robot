-- arquivo de configuracao
-- ssid e senha

config = {}
config.ssid = "Fred"
config.pwd  = "Fred2023"

device_name = "FRED" .. "_" .. node.chipid() -- cria um nome único para o robô FRED_chipID
host = "192.168.99.100" -- endereco mqtt broker
top_base = device_name -- topico base do dispositivo
top_broadcast = "FRED"


