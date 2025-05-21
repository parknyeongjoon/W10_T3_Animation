Script = {}

function Script.BeginPlay()
    PrintObject(Script.this:GetOwner().GetUSceneComponent)
end

function Script.Tick(deltaTime)
end

function Script.EndPlay()
    print("End")
end

return Script