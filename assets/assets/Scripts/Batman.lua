Is_dead = false

function on_event(me, event)
    if not Is_dead and event.name == "Hit" then
        play_anim(me, "OrbExplodeAnim")
        Is_dead = true
    end
    if Is_dead and event.name == "AnimationFinished" then
        destroy(me)
    end
end
