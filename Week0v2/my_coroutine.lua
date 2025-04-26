-- my_coroutine.lua

local elapsedTime = 0
local isReady = false

function my_coroutine()
    print("[Lua] Start Coroutine")

    print("[Lua] Waiting for 2 seconds...")
    coroutine.yield(WaitForSeconds(2.0))
    print("[Lua] 2 seconds passed")

    print("[Lua] Waiting for 3 frames...")
    coroutine.yield(WaitForFrames(3))
    print("[Lua] 3 frames passed")

    -- 5초 타이머 시작
    elapsedTime = 0
    isReady = false

    print("[Lua] Starting 5-second timer...")

    coroutine.yield(WaitWhile(function()
        elapsedTime = elapsedTime + 0.016  -- 매 틱마다 0.016초 추가 (60FPS 가정)
        if elapsedTime >= 5.0 then
            isReady = true
        end
        return not isReady
    end))

    print("[Lua] Timer done, 5 seconds passed!")

    -- 카운트다운
    local count = 5
    print("[Lua] Waiting while count > 0 ...")
    coroutine.yield(WaitWhile(function()
        count = count - 1
        print("[Lua] count:", count)
        return count > 0
    end))

    print("[Lua] Count finished, exiting coroutine")
end
