-- Firmware de controle do robô FRED
-- Autor: Marcelo Rocha
-- Universidade Federal Fluminense
-- MidiaCOM Lab
-- Orientadora: Débora C. Muchaluat-Saade

print("Device Name:" .. device_name)

-- cria o cliente mqtt com as opcoes especificadas
mqtt_client = mqtt.Client(device_name, 120)

mqtt_client:lwt("/lwt", "offline", 0, 0)

mqtt_client:on("offline", function(client) print ("offline") end)

-- on publish message receive event
mqtt_client:on("message", function(client, topic, data)
  s:on("data", 10, function(data)
    client:publish(top_base .. "/status", data, 0, 0, function(client) end)
end)
  -- EXPRESSION
  if (topic == top_base .. "/expression") or (topic == top_broadcast .. "/expression") then
    print(top_base .. "/expression")
    if data ~= nil then
      if data == "greetings" then
        s:write("ef")
      elseif data == "in_love" then
        s:write("ei")
      elseif data == "broken" then
        s:write("eb")
      elseif data == "neutral" then
        s:write("en")
      elseif data == "pleased" then
        s:write("ep")
      elseif data == "happy" then
        s:write("eh")
      elseif data == "sad" then
        s:write("es")
      elseif data == "angry" then
        s:write("ea")
      elseif data == "angry2" then
        s:write("eA")
      elseif data == "surprised" then
        s:write("eu")
      elseif data == "afraid" then
        s:write("er")
      elseif data == "speech_on_1" then
        s:write("eT")
      elseif data == "speech_off_1" then
        s:write("et")
      elseif data == "speech_on_2" then
        s:write("eC")
      elseif data == "speech_off_2" then
        s:write("ec")
      end
    end
  end

  -- LEDS
  if (topic == top_base .. "/leds") or (topic == top_broadcast .. "/leds") then
    print(top_base .. "/leds")
    if data ~= nil then
      if data == "red" then
        s:write("lr")
      elseif data == "blue" then
        s:write("lb")
      elseif data == "green" then
        s:write("lg")
      elseif data == "white" then
        s:write("lw")
      elseif data == "blue" then
        s:write("lb")
      elseif data == "black" then -- off
        s:write("lk")
      elseif data == "pink" then
        s:write("lp")
      elseif data == "yellow" then
        s:write("ly")
      elseif data == "rainbow" then
        s:write("ln")
      end
    end
  end

  -- POSE
  if (topic == top_base .. "/pose") or (topic == top_broadcast .. "/pose") then
    print(top_base .. "/pose")
    if data ~= nil then
      if data == "neutral" then
        s:write("pi")
      elseif data == "left_foot1" then
        s:write("pl")
      elseif data == "left_foot2" then
        s:write("pL")
      elseif data == "right_foot1" then
        s:write("pr")
      elseif data == "right_foot2" then
        s:write("pR")
      elseif data == "sad_foot_slow" then -- broken (slow)
        s:write("pd")
      elseif data == "sad_foot_fast" then -- broken (fast)
        s:write("pD")
      elseif data == "tiptoe_foot" then
        s:write("pu")
      end
    end
  end

  -- MOVEMENT
  if (topic == top_base .. "/move") or (topic == top_broadcast .. "/move") then
    print(top_base .. "/move")
    if data ~= nil then
      if data == "forward" then -- one cycle
        s:write("mf")
      elseif data == "forward2" then --  two cycles
        s:write("mF")
      elseif data == "backward" then -- one cycle
        s:write("mb")
      elseif data == "backward2" then --  two cycles
        s:write("mB")
      elseif data == "left" then 
        s:write("ml")
      elseif data == "left_moon" then -- moonwalk to left
        s:write("mL")
      elseif data == "right" then 
        s:write("mr")
      elseif data == "right_moon" then -- moonwalk to left
        s:write("mR")
      elseif data == "moonwalk" then -- both sides
        s:write("mw")
      elseif data == "moonwalk2" then -- both sides (two cycles)
        s:write("mW")
      elseif data == "dance1" then -- dance 1
        s:write("md")
      elseif data == "dance1_2" then -- dance 1 (two cycles)
        s:write("mD")
      elseif data == "dance2" then -- dance 2
        s:write("me")
      elseif data == "dance2_2" then -- dance 2 (two cycles)
        s:write("mE")
      elseif data == "stomping_foot_r" then -- 
        s:write("ms")
      elseif data == "stomping_foot_l" then -- 
        s:write("mS")
      end
    end
  end 
end)


-- conecta ao mqtt broker

-- for TLS: m:connect("192.168.11.118", secure-port, 1)
mqtt_client:connect(host, 1883, false, function(client)
  s:write("lg") -- Led verde indica que conectou ao broker
  print("MQTT Broker connected...")
  
  -- subscribe topic with qos = 0
  client:subscribe(top_base .. "/expression", 0, function(client) print("Expression topic subscribed.") 
   client:subscribe(top_base .. "/leds", 0, function(client) print("Leds topic subscribed.")
    client:subscribe(top_base .. "/pose", 0, function(client) print("Pose topic subscribed.")
     client:subscribe(top_base .. "/move", 0, function(client) print("Move topic subscribed.")
      client:subscribe(top_broadcast .. "/move", 0, function(client) print("Top broadcast Move subscribed.")
       client:subscribe(top_broadcast .. "/pose", 0, function(client) print("Top broadcast Pose subscribed.")
        client:subscribe(top_broadcast .. "/leds", 0, function(client) print("Top broadcast Leds subscribed.")
         client:subscribe(top_broadcast .. "/expression", 0, function(client) print("Top broadcast Expression subscribed.")
         end)
        end)
       end)
      end)
     end)
    end)
   end)
  end)
  
  -- publish a message with data = hello, QoS = 0, retain = 0
  -- client:publish("top_base", "hello_from_node", 0, 0, function(client) print("sent") end)
end,
function(client, reason)
  print("Connection failed reason: " .. reason)
end)

