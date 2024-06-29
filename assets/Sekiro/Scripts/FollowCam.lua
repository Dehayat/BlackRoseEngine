function setup(me)
    Target_entity = find("Player")
    if Target_entity~=no_entity then
        Last_pos = get_position(Target_entity)
    end
end

function update(me, dt)
    if Target_entity~=no_entity then
        local newPos = get_position(Target_entity)
        local offset = newPos-Last_pos
        print(offset.x .. offset.y)
        move(me,offset)
        print("cool")
        Last_pos = newPos
    end
end