Vars = {
    speed = 2
}

States = {
    Idle = 0,
    Walking = 1,
    Attacking = 2,
}

local Sprite_entity = no_entity
local WalkDir = 0
local HitBox_entity = no_entity
local state = States.Idle


function setup(me)
    Sprite_entity = get_child(me, "sprite")
    HitBox_entity = get_child(me, "hitBox")
    Is_attacking = false
end

function update(me, dt)
    if state == States.Walking then
        if WalkDir ~= 0 then
            move(me, WalkDir * Vars.speed * dt, 0)
        end
    end
end

local function PlayAnim(entity, anim)
    if Sprite_entity ~= no_entity then
        play_anim(Sprite_entity, anim)
    end
end

local function Walk(me, dir)
    WalkDir = dir
    face(me, WalkDir)
    if state == States.Idle then
        PlayAnim(Sprite_entity, "sekiroRunAnim")
        state = States.Walking
    end
end

local function StopWalking(me)
    WalkDir = 0
    if state == States.Walking then
        PlayAnim(Sprite_entity, "sekiroIdleAnim")
        state = States.Idle
    end
end
local function Attack(me)
    if state ~= States.Attacking then
        PlayAnim(Sprite_entity, "sekiroAttackAnim")
        state = States.Attacking
    end
end

local function key_pressed(me, event)
    if event.input_key == "D" then
        Walk(me, 1)
    elseif event.input_key == "A" then
        Walk(me, -1)
    elseif event.input_key == "Left Button" then
        Attack(me)
    end
end

local function key_released(me, event)
    if event.input_key == "D" then
        if WalkDir == 1 then
            StopWalking(me)
        end
    elseif event.input_key == "A" then
        if WalkDir == -1 then
            StopWalking(me)
        end
    end
end

local function FinishAttack(me)
    if WalkDir == 0 then
        PlayAnim(Sprite_entity, "sekiroIdleAnim")
        state = States.Idle
    else
        face(me, WalkDir)
        PlayAnim(Sprite_entity, "sekiroRunAnim")
        state = States.Walking
    end
end

local function animation_finished(me, event)
    if state == States.Attacking then
        FinishAttack(me)
    end
end

function on_event(me, event)
    if event.name == "KeyPressed" then
        key_pressed(me, event)
    elseif event.name == "KeyReleased" then
        key_released(me, event)
    elseif event.name == "MousePressed" then
        key_pressed(me, event)
    elseif event.name == "disableHitBox" then
        if HitBox_entity ~= no_entity then
            disable(HitBox_entity)
        end
    elseif event.name == "enableHitBox" then
        if HitBox_entity ~= no_entity then
            enable(HitBox_entity)
        end
    elseif event.name == "AnimationFinished" then
        animation_finished(me, event)
    end
end
