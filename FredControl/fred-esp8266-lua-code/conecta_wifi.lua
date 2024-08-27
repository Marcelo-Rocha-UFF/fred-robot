-- conecta ao wifi
-- conectando ao roteador

dofile("tabelas.lua") -- importa as tabelas de indices
dofile("credenciais.lua") -- importa tabela config com ssid e senha definida no arquivo credenciais.lua
dofile("softserial_setup.lua") -- configura a porta de comunicacao serial com o Arduino

-- s:write("eb")
-- s:write("lr")

wifi.setmode(wifi.STATION) -- define o modo da conexao wifi

wifi.sta.config(config) -- configura a conexao

print("Conectando ao wifi...")

-- tenta conectar a cada 1s
mytimer = tmr.create()
mytimer:register(1000, tmr.ALARM_AUTO, function ()
    if wifi.sta.getip() == nil
        then
            print("Estado WIFI... " .. estado_wifi[wifi.sta.status()])
            print("IP indisponivel, aguardando...") -- falha ao conectar
        else
            s:write("eh")
            meu_ip = wifi.sta.getip()
            meu_mac = wifi.sta.getmac()
            print("Estado WIFI... " .. estado_wifi[wifi.sta.status()])
            print("Conectado, o IP é " .. meu_ip)
            print("O End. MAC é " .. meu_mac)
            mytimer:unregister() -- libera o timer
            dofile("fred-webserver.lc") -- servidor web
            dofile("fred-control-lua.lc") -- carrega o firmware principal
    end
end)

mytimer:start()

