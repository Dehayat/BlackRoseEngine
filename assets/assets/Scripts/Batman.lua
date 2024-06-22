Is_dead = false

function on_event(me,event)
    if not Is_dead and event.name=="SensorEntered" then
        local entityName = get_name(event.target)
        if entityName=="hitBox" then
            play_anim(me,"OrbExplodeAnim")
            Is_dead = true
        end
    end
end