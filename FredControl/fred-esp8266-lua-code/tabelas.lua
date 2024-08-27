-- tabela com os modos do wifi
modo_wifi = {}
modo_wifi[1] = 'wifi.STATION'
modo_wifi[2] = 'wifi.SOFTAP'
modo_wifi[3] = 'wifi.STATIONAP'
modo_wifi[4] = 'wifi.NULLMODE'

-- tabela com os estados da conexao wifi
estado_wifi = {}
estado_wifi[0] = 'wifi.STA_IDLE'
estado_wifi[1] = 'wifi.STA_CONNECTING'
estado_wifi[2] = 'wifi.STA_WRONGPWD'
estado_wifi[3] = 'wifi.STA_APNOTFOUND'
estado_wifi[4] = 'wifi.STA_FAIL'
estado_wifi[5] = 'wifi.STA_GOTIP'


-- tabela com as razoes dos erros de conexao com o mqtt broker
-- 0 esta OK - nunca levanta uma excecao de numero 0
err_conn = {}
err_conn[-5] = 'mqtt.CONN_FAIL_SERVER_NOT_FOUND'
err_conn[-4] = 'mqtt.CONN_FAIL_NOT_A_CONNACK_MSG'
err_conn[-3] = 'mqtt.CONN_FAIL_DNS'
err_conn[-2] = 'mqtt.CONN_FAIL_TIMEOUT_RECEIVING'
err_conn[-1] = 'mqtt.CONN_FAIL_TIMEOUT_SENDING'
err_conn[0]  = 'mqtt.CONNACK_ACCEPTED'
err_conn[1]  = 'mqtt.CONNACK_REFUSED_PROTOCOL_VER'
err_conn[2]  = 'mqtt.CONNACK_REFUSED_ID_REJECTED'
err_conn[3]  = 'mqtt.CONNACK_REFUSED_SERVER_UNAVAILABLE'
err_conn[4]  = 'mqtt.CONNACK_REFUSED_BAD_USER_OR_PASS'
err_conn[5]  = 'mqtt.CONNACK_REFUSED_NOT_AUTHORIZED'


