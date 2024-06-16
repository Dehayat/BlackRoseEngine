
function setup(me)
    print("I'm batman")
end

function on_event(me, event)
    if event~="AnimationFinished" then
        print("BatEvent");
    end
end