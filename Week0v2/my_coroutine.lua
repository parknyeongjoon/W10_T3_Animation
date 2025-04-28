-- my_coroutine.lua

local elapsedTime = 0
local isReady = false

function my_coroutine()
    Log("[Lua] Start Coroutine")

    Log("[Lua] Waiting for 2 seconds...")
    coroutine.yield(WaitForSeconds(5.0))
    Log("[Lua] 2 seconds passed")

    Log("[Lua] Waiting for 3 frames...")
    coroutine.yield(WaitForFrames(3))
    Log("[Lua] 3 frames passed")

    elapsedTime = 0
    isReady = false
    Log("[Lua] Starting 5-second timer...")

    coroutine.yield(WaitWhile(function()
        elapsedTime = elapsedTime + 0.016
        if elapsedTime >= 5.0 then
            isReady = true
        end
        return not isReady
    end))

    Log("[Lua] Timer done, 5 seconds passed!")

    local count = 5
    Log("[Lua] Waiting while count > 0 ...")
    coroutine.yield(WaitWhile(function()
        count = count - 1
        Log("[Lua] count: " .. tostring(count))
        return count > 0
    end))

    elapsedTime = 0

    Log("[Lua] 5초 동안 DeltaTime Print Start")

    coroutine.yield(WaitWhile(function()
        local dt = DeltaTime()
        elapsedTime = elapsedTime + dt
        Log("[Lua] DeltaTime = " .. tostring(dt) .. ", ElapsedTime = " .. tostring(elapsedTime))
        return elapsedTime < 5.0
    end))

    Log("[Lua] Count finished, exiting coroutine")
end
