-- This is a lua comment

vars = {
    speed = 5;
}

sprite_entity = no_entity
is_attacking = false
walkDir = 0

function setup(me)
    sprite_entity = get_child(me, "sprite")
    is_attacking = false
end

function update(me, dt)
    print("update " .. me)
    print(walkDir)

    if not is_attacking then
        if walkDir ~= 0 then
            move(me, walkDir*vars.speed * dt, 0)
        end
    end
end

function on_event(me, event)
    print(me .. " " .. event)
    if event == "LeftKeyReleased" or event == "RightKeyReleased" then
        walkDir = 0
    end
    if is_attacking and event=="AnimationFinished" then
        play_anim(sprite_entity,"sekiroIdleAnim")
        is_attacking = false
    end
    if not is_attacking then
        if event == "RightKeyPressed" then
            walkDir = 1
            face(me,walkDir)
        elseif event == "LeftKeyPressed" then
            walkDir = -1
            face(me,walkDir)
        end
        if sprite_entity ~=no_entity then
            if walkDir==0 then
                play_anim(sprite_entity,"sekiroIdleAnim")
            else
                play_anim(sprite_entity,"sekiroRunAnim")
            end
        end
        
        if event == "LeftMousePressed" and sprite_entity~=no_entity then
            play_anim(sprite_entity,"sekiroAttackAnim")
            is_attacking = true;
        end
    end
end