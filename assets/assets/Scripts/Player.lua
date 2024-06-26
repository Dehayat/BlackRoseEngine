-- This is a lua comment

Vars = {
    speed = 0.5;
}

Sprite_entity = no_entity
Is_attacking = false
WalkDir = 0
HitBox_entity = no_entity

function setup(me)
    Sprite_entity = get_child(me, "sprite")
    HitBox_entity = get_child(me, "hitBox")
    Is_attacking = false
end

function update(me, dt)
    if not Is_attacking then
        if WalkDir ~= 0 then
            move(me, WalkDir*Vars.speed * dt, 0)
        end
    end
end

function on_event(me, event)
    if event.name == "disableHitBox" then
        disable(HitBox_entity)
    end
    if event.name == "enableHitBox" then
        enable(HitBox_entity)
    end
    if event.name == "KeyReleased"then
        if event.input_key=="D" or event.input_key=="A" then
            WalkDir = 0
        end
    end
    if Is_attacking and event.name=="AnimationFinished" then
        play_anim(Sprite_entity,"IdleAnim")
        Is_attacking = false
    end
    if not Is_attacking then
        if event.name == "KeyPressed" and event.input_key=="D" then
            WalkDir = 1
            play_anim(Sprite_entity,"WalkAnim")
            --face(me,WalkDir)
        elseif event.name == "KeyPressed" and event.input_key=="A" then
            WalkDir = -1
            play_anim(Sprite_entity,"WalkBackAnim")
            --face(me,WalkDir)
        end
        if Sprite_entity ~=no_entity then
            if WalkDir==0 then
                play_anim(Sprite_entity,"IdleAnim")
            end
        end
        
        if event.name == "MousePressed" and event.input_key=="Left Button" and Sprite_entity~=no_entity then
            play_anim(Sprite_entity,"AttackAnim")
            Is_attacking = true;
        end
    end
end