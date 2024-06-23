function on_event(me,event)
    if event.name=="EnteringSensor" then
        local entityName = get_name(event.target)
        if entityName=="hitBox" then
            destroy(me)
        end
    end
end