--PRINT SOMETHING OUTSIDE MAKE LUA EVERY TICK CALL THIS, BETTER USE LOOP FEATURES IN MainTick
--NO NATIVES RIGHT NOW, ADD IT BY YOURSELF

init = function() --CALLED ON SCRIPT TOGGLED ON
Notify("This lua inited")
end

uninit = function() --CALLED ON SCRIPT TOGGLED OFF
Notify("This lua unloaded")
end

MainTick = function() --MAIN LOOP TICK
--HERE MAIN LOOP
end